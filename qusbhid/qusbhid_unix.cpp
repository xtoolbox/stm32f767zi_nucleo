#include "qusbhid.h"
#include <QDebug>
#include <QThread>

#define DEF_TIMEOUT  1000

extern "C" void get_usbfs_path(struct libusb_device *dev, char *path);


// HID Class-Specific Requests values. See section 7.2 of the HID specifications
 #define HID_GET_REPORT                0x01
 #define HID_GET_IDLE                  0x02
 #define HID_GET_PROTOCOL              0x03
 #define HID_SET_REPORT                0x09
 #define HID_SET_IDLE                  0x0A
 #define HID_SET_PROTOCOL              0x0B
 #define HID_REPORT_TYPE_INPUT         0x01
 #define HID_REPORT_TYPE_OUTPUT        0x02
 #define HID_REPORT_TYPE_FEATURE       0x03


#define  ERROR_DEVICE_NOT_SUPPORT     -20

#define CTRL_IN     LIBUSB_ENDPOINT_IN|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE
#define CTRL_OUT    LIBUSB_ENDPOINT_OUT|LIBUSB_REQUEST_TYPE_CLASS|LIBUSB_RECIPIENT_INTERFACE

class QHidWorker :public QThread
{
public:
    QHidWorker(QUsbHid* hid):mHid(hid){}
protected:
    virtual void run();
private:
    QUsbHid* mHid;
};

static int init = libusb_init(0);

QUsbHid::QUsbHid(QObject *parent,QueryMode mode) :
    QIODevice(parent),
    m_queryMode(mode),
    m_path(QString("")),
    handle(0)
{
    initial();
}

QUsbHid::QUsbHid(const QString& path, QObject *parent,QueryMode mode) :
    QIODevice(parent),
    m_queryMode(mode),
    m_path(path),
    handle(0)
{
    initial();
}

QUsbHid::~QUsbHid()
{
    deinitial();
}

QByteArray resize_array(const QByteArray& in_data, int len)
{
    QByteArray buffer(len,0);
    memcpy(buffer.data(), in_data.data(), buffer.length()>in_data.length() ? in_data.length(): buffer.length());
    return buffer;
}

void QUsbHid::initial()
{
    mutex = new QMutex(QMutex::Recursive);
    bytesToWriteLock = new QReadWriteLock;
    _bytesToWrite = 0;
    readBufferLock = new QReadWriteLock;
    //enumerator = new QUsbHidEnumerator(this);
    //connect(enumerator, SIGNAL(deviceDiscovered(QUsbHidInfo)), this, SIGNAL(connected(QUsbHidInfo)));
    //connect(enumerator, SIGNAL(deviceRemoved(QUsbHidInfo)), this, SIGNAL(disconnected(QUsbHidInfo)));
    m_timeout = 30000;
    lastErr = 0;
    m_isOpen = false;
    interface = -1;
    epForRead = 0;
    epForWrite = 0;
}

void QUsbHid::deinitial()
{
    delete bytesToWriteLock;
    delete readBufferLock;
    close();
    delete mutex;
}

bool QUsbHid::open(OpenMode mode)
{
    if (queryMode() == QUsbHid::EventDriven)
    QMutexLocker lock(mutex);
    if (mode == QIODevice::NotOpen)
        return isOpen();
    libusb_device* dev = QUsbHidEnumerator::get_device(this->m_path);
    if(!dev){
        return false;
    }

    libusb_get_device_descriptor(dev, &device_desc);

    lastErr = libusb_open(dev, &handle);
    if(lastErr < 0 ){
        if(lastErr == LIBUSB_ERROR_ACCESS){
            //qDebug()<<"Permission denied, run the following command in root mode";
            QString str = QString("echo SUBSYSTEM==\\\"usb\\\", ATTRS{idVendor}==\\\"%1\\\", ATTRS{idProduct}==\\\"%2\\\", MODE=\\\"0666\\\" > /etc/udev/rules.d/90-lxyppc.rules");
            str = str.arg(device_desc.idVendor,4,16,QChar('0')).arg(device_desc.idProduct,4,16,QChar('0'));
            //qDebug()<<str;
            permissionError = "\r\nTry run follow command as root\r\n";
            permissionError += str;
        }
        handle = 0;
    }


    if (handle != 0) {
        //HidD_GetPreparsedData (Win_Handle, &hidPrepParsedData);
        //HidP_GetCaps (hidPrepParsedData, &hidCaps);
        // here we should parse the report descriptor
        //init event driven approach
        hidCaps.featureReportLength = 64;
        hidCaps.inputReportLength = 64;
        hidCaps.outputReportLength = 64;
        hidCaps.uasgePage = 0;
        hidCaps.usage = 0;

        libusb_config_descriptor* cfg;
        int r = libusb_get_config_descriptor(dev,0, &cfg);

        if(r == 0){
            for(int j=0; j<cfg->bNumInterfaces; j++){
                const struct libusb_interface_descriptor * interface_desc = cfg->interface->altsetting+j;
                if(interface_desc->bInterfaceClass == LIBUSB_CLASS_HID){
                    for(int i=0;i<interface_desc->bNumEndpoints;i++){
                        const libusb_endpoint_descriptor* ep = cfg->interface->altsetting->endpoint+i;
                        if(ep->bEndpointAddress & 0x80){
                            hidCaps.inputReportLength = ep->wMaxPacketSize;
                            epForRead = ep->bEndpointAddress;
                        }else{
                            epForWrite = ep->bEndpointAddress;
                            hidCaps.outputReportLength = ep->wMaxPacketSize;
                        }
                    }
                    interface = j;
                    break;
                }
                // interface class must be LIBUSB_CLASS_HID
            }
            libusb_free_config_descriptor(cfg);
        }

        if(interface<0){
            lastErr = ERROR_DEVICE_NOT_SUPPORT;
        }else{

            lastErr = libusb_detach_kernel_driver(handle, 0);

            lastErr = libusb_set_configuration(handle, 1);

            lastErr = libusb_claim_interface(handle, interface);
            if(lastErr >= 0){
                QIODevice::open(mode);
                //epForRead = 0x81;
                //epForWrite = 0x01;
                if (queryMode() == QUsbHid::EventDriven) {
                    //winEventNotifier = new QWinEventNotifier(overlap.hEvent, this);
                    //connect(winEventNotifier, SIGNAL(activated(HANDLE)), this, SLOT(onWinEvent(HANDLE)));
                    //rawData[0] = 0;
                    //::ReadFile(Win_Handle, rawData, hidCaps.InputReportByteLength, 0, &overlap);
                    worker = new QHidWorker(this);
                    worker->start();
                }

            }else{
                libusb_close(handle);
            }
        }
    }
    m_isOpen = isOpen();
    return isOpen();
}

void QUsbHid::close()
{
    QMutexLocker lock(mutex);
    if (m_isOpen) {
        flush();
        if(interface >=0){
            libusb_release_interface(handle, interface);
        }
        libusb_attach_kernel_driver(handle, 0);
        libusb_close(handle);
        handle = 0;

        if(worker){
            worker->wait(500);
            if(!worker->isFinished()){
                worker->terminate();
            }
            delete worker;
        }
        QIODevice::close(); // mark ourselves as closed
        _bytesToWrite = 0;
    }
    m_isOpen = false;
    lastErr = 0;
}

void QUsbHid::flush()
{
}

qint64 QUsbHid::bytesAvailable() const
{
    QReadLocker rl(readBufferLock);
    return readBuffer.length();
}

qint64 QUsbHid::bytesToWrite() const
{
    QReadLocker rl(bytesToWriteLock);
    return _bytesToWrite;
}

QByteArray QUsbHid::readAll()
{
    QByteArray res = readBuffer;
    QWriteLocker wr(readBufferLock);
    readBuffer.clear();
    return res;
}

QByteArray QUsbHid::readData()
{
    return readData(this->hidCaps.inputReportLength);
}

QByteArray QUsbHid::readData(qint64 maxSize)
{
    QByteArray res(maxSize,0);
    int retVal;
    QMutexLocker lock(mutex);
    retVal = 0;
    if (queryMode() == QUsbHid::EventDriven && false) {

    } else{
        if(!handle) return false;
        int actLen = 0;
        int timeout = DEF_TIMEOUT;
        int errorCode = libusb_interrupt_transfer(handle, epForRead, (uint8_t*)res.data(), maxSize, &actLen, timeout);
        if(errorCode < 0) retVal = 0;
        else retVal = actLen;
        lastErr = errorCode;
        qDebug()<<res;
    }
    if(retVal == 0){
        res.clear();
    }else{
        res.resize(retVal);
    }
    return res;
}

qint64 QUsbHid::writeData(int reportID, const QByteArray& data)
{
    QByteArray arr;
    arr.append(char(reportID));
    arr.append(data);
    return writeData(arr);
}
static QString formatError(int err);
qint64 QUsbHid::writeData(const QByteArray& in_data)
{
    //QByteArray buffer(hidCaps.OutputReportByteLength,0);
    //memcpy(buffer.data(), in_data.data(), buffer.length()>in_data.length() ? in_data.length(): buffer.length());
    QByteArray data = resize_array(in_data, hidCaps.outputReportLength + 1);
    QMutexLocker lock( mutex );
    int retVal = 0;
    if(handle){
        int actLen = 0;
        int timeout = DEF_TIMEOUT;
        uint8_t* p = (uint8_t *)data.data();
        if(*p == 0){
            // report ID is 0, ignore it
            p++;
        }
        int errorCode = libusb_interrupt_transfer(handle, epForWrite, p, hidCaps.outputReportLength, &actLen, timeout);
        lastErr = errorCode;
        if(errorCode < 0) retVal = 0;
        else retVal = actLen;
        //qDebug()<<formatError(lastErr);
    }
    return (qint64)retVal;
}


qint64 QUsbHid::readData(char *data, qint64 maxlen)
{
    QByteArray res = readData(maxlen);
    memcpy(data, res.data(), res.length());
    return res.length();
}

qint64 QUsbHid::writeData(const char *data, qint64 len)
{
    return writeData(QByteArray::fromRawData(data,len));
}

void QUsbHid::onHidData(void* p, int len)
{
    QMutexLocker lock(mutex);
    {
        // got data
        readBuffer.append((char*)p,len);
        if (bytesAvailable() > 0)
            emit readyRead();
    }
}

#ifdef Q_OS_UNIX
static QString formatError(int err)
{
    switch(err){
    case LIBUSB_SUCCESS: return "Success";

        /** Input/output error */
    case LIBUSB_ERROR_IO: return "Input/output error";

        /** Invalid parameter */
    case LIBUSB_ERROR_INVALID_PARAM: return "Invalid parameter";

        /** Access denied (insufficient permissions) */
    case LIBUSB_ERROR_ACCESS: return "Access denied (insufficient permissions)";// = -3,

        /** No such device (it may have been disconnected) */
    case LIBUSB_ERROR_NO_DEVICE: return "No such device (it may have been disconnected)";// = -4,

        /** Entity not found */
    case LIBUSB_ERROR_NOT_FOUND: return "Entity not found";// = -5,

        /** Resource busy */
    case LIBUSB_ERROR_BUSY: return "Resource busy";// = -6,

        /** Operation timed out */
    case LIBUSB_ERROR_TIMEOUT: return "Operation timed out";// = -7,

        /** Overflow */
    case LIBUSB_ERROR_OVERFLOW: return "Overflow";// = -8,

        /** Pipe error */
    case LIBUSB_ERROR_PIPE: return "Pipe error";// = -9,

        /** System call interrupted (perhaps due to signal) */
    case LIBUSB_ERROR_INTERRUPTED: return "System call interrupted (perhaps due to signal)";// = -10,

        /** Insufficient memory */
    case LIBUSB_ERROR_NO_MEM: return "Insufficient memory";// = -11,

        /** Operation not supported or unimplemented on this platform */
    case LIBUSB_ERROR_NOT_SUPPORTED: return "Operation not supported or unimplemented on this platform";// = -12,

    case ERROR_DEVICE_NOT_SUPPORT: return "Device not support";
        /** Other error */
    case LIBUSB_ERROR_OTHER:
    default:
        return "Other error";// = -99
    }
    return "Other error";// = -99
}
#endif

QString QUsbHid::errorString()const
{
    QString res;
#ifdef Q_OS_WIN
    LPTSTR lpMsgBuf = 0;
    DWORD ret = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
                  0,
                  lastErr,
                  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  (LPTSTR) &lpMsgBuf, 0, 0);
#ifdef UNICODE
    res = QString::fromWCharArray( (LPTSTR)lpMsgBuf);
#else
    res =  QString::fromLocal8Bit((LPTSTR) lpMsgBuf);
#endif
    res.remove(QChar('\n'));
    LocalFree(lpMsgBuf);
    (void)ret;
#endif

#ifdef Q_OS_UNIX
    res = formatError(lastErr);
#endif
    if(LIBUSB_ERROR_ACCESS == lastErr ){
        res += permissionError;
    }
    return res;
}

QList<QUsbHidInfo> QUsbHid::enumDevices(int vid, int pid)
{
    return QUsbHidEnumerator::getPorts((unsigned short)vid,(unsigned short)pid);
}


QHidAttr QUsbHid::GetAttributes(bool* r)
{
    QHidAttr res;
    res.productID = device_desc.idProduct;
    res.vendorID = device_desc.idVendor;
    res.version = device_desc.bcdDevice;
    if(r)*r = true;
    return res;
}

QByteArray hid_get_report(libusb_device_handle* handle,int interface, uint16_t reportType, uint8_t reportID, int maxLen, int* lastErr, bool *r)
{
    int error = 0;
    if(!lastErr)lastErr = &error;
    QByteArray ret;
    if(!handle){
        if(r) *r = false;
        return ret;
    }
    ret.resize(maxLen);
    *lastErr = libusb_control_transfer(
            handle,
            CTRL_IN,
            HID_GET_REPORT,
            (reportType<<8)|reportID,
            interface,
            (uint8_t*)ret.data(),
            maxLen,
            DEF_TIMEOUT);

    if(error<0 && r){
        *r =false;
    }
    return ret;
}

bool hid_set_report(libusb_device_handle* handle,int interface, uint16_t reportType, uint8_t reportID, QByteArray arr, int* lastErr)
{
    int error = 0;
    if(!lastErr)lastErr = &error;
    if(!handle){
        return false;
    }
    *lastErr = libusb_control_transfer(
            handle,
            CTRL_OUT,
            HID_SET_REPORT,
            (reportType<<8)|reportID,
            interface,
            (uint8_t*)arr.data(),
            arr.size(),
            DEF_TIMEOUT);
    return error >= 0;
}


QByteArray QUsbHid::getFeature(bool* r)
{
    return getFeature(0,this->hidCaps.featureReportLength,r);
}

QByteArray QUsbHid::getFeature(int maxLen, bool* r)
{
    return getFeature(0,maxLen,r);
}

QByteArray QUsbHid::getFeature(int reportID, int maxLen,bool* r)
{
    return hid_get_report(handle, interface, HID_REPORT_TYPE_FEATURE, reportID, maxLen, &lastErr, r);
}

QByteArray QUsbHid::getInputReport(bool* r)
{
    return getInputReport(0,hidCaps.inputReportLength,r);
}


QByteArray QUsbHid::getInputReport(int maxLen,bool* r)
{
    return getInputReport(0,maxLen,r);
}

QByteArray QUsbHid::getInputReport(int reportID, int maxLen,bool* r)
{
    return hid_get_report(handle, interface, HID_REPORT_TYPE_INPUT, reportID, maxLen, &lastErr, r);
}

int QUsbHid::getNumInputBuffers(bool* r)
{
    int num = 0;
    //bool res = HidD_GetNumInputBuffers(Win_Handle, &num);
    //libusb_control_transfer
    if(r) *r = false;
    lastErr = LIBUSB_ERROR_NOT_SUPPORTED;
    return num;
}

QByteArray QUsbHid::getPhysicalDescriptor(int maxLen, bool* r)
{
    QByteArray ret;
    ret.resize(maxLen);
    //bool res = HidD_GetPhysicalDescriptor(Win_Handle, (char*)ret.data(), ret.length());
    //libusb_control_transfer
    lastErr = LIBUSB_ERROR_NOT_SUPPORTED;
    if(r) *r = false;
    return ret;
}

static QString translateUSBString(const char* str, int len)
{
    if(len < 4) return QString("<null>");
    return QString((QChar*)(str+2), (len - 2)/2);
}

QString QUsbHid::getIndexedString(int index,bool* r)
{
    char ret[256];
    if(!handle || index ==0){
        if(r) *r = false;
        return QString();
    }
    //bool res = HidD_GetIndexedString(Win_Handle, index, ret, sizeof(ret));
    int res = libusb_get_string_descriptor(handle,index,0,(uint8_t*)ret,256);
    if(r) *r = res > 0;
    return translateUSBString(ret,res);
}

QString QUsbHid::getManufacturerString(bool* r)
{
    return getIndexedString(device_desc.iManufacturer,r);
}

QString QUsbHid::getProductString(bool* r)
{
    return getIndexedString(device_desc.iProduct,r);
}

QString QUsbHid::getSerialNumberString(bool* r)
{
    return getIndexedString(device_desc.iSerialNumber,r);
}

bool QUsbHid::setFeature(const QByteArray& arr)
{
    return hid_set_report(handle, interface, HID_REPORT_TYPE_FEATURE, arr[0], arr.mid(1), &lastErr);
    //QByteArray s = resize_array(arr, hidCaps.FeatureReportByteLength);
    //bool res =  HidD_SetFeature(Win_Handle,s.data(), s.length());
    //lastErr = GetLastError();
    //return res;
    //(void)arr;
    //return false;
}

bool QUsbHid::setFeature(int reportID, const QByteArray& arr)
{
    QByteArray x;
    x.append(char(reportID));
    x.append(arr);
    return setFeature(x);
}

bool QUsbHid::setOutputReport(const QByteArray& arr)
{
    //QByteArray s = resize_array(arr, hidCaps.OutputReportByteLength);
    //bool res =  HidD_SetOutputReport(Win_Handle,s.data(), arr.length());
    //lastErr = GetLastError();
    //return res;
    return hid_set_report(handle, interface, HID_REPORT_TYPE_OUTPUT, arr[0], arr.mid(1), &lastErr);
    //(void)arr;
    //return false;
}

bool QUsbHid::setOutputReport(int reportID, const QByteArray& arr)
{
    QByteArray x;
    x.append(char(reportID));
    x.append(arr);
    return setOutputReport(x);
}

bool QUsbHid::setNumInputBuffers(int num)
{
    //bool res = HidD_SetNumInputBuffers(Win_Handle,(ULONG)num);
    //lastErr = GetLastError();
    //return res;
    (void)num;
    lastErr = LIBUSB_ERROR_NOT_SUPPORTED;
    return false;
}

QHidCaps  QUsbHid::caps() const
{
    return hidCaps;
}

struct CB_Data
{
    QUsbHid* device;
    unsigned char* buffer;
    struct libusb_transfer* transfer;
    bool   bRun;
};

static void request_exit(CB_Data* p)
{
    p->bRun = false;
}
static void showBytes(const void* p, int len, const char* desc)
{
    qDebug()<<desc<<":"<<len;
    const unsigned char* data = (const unsigned char*)p;
    QString s;
    for(int i=0;i<len;i++){
        s+=QString("%1,").arg(data[i],2,16, QChar('0'));
        if( (i&15) == 15){
            qDebug()<<s;
            s = "";
        }
    }
    if(s.length())qDebug()<<s;
}

static void cb_data(struct libusb_transfer *transfer)
{
    CB_Data * p = (CB_Data*) transfer->user_data;
        if (transfer->status != LIBUSB_TRANSFER_COMPLETED) {
                qDebug()<<"img transfer status "<<transfer->status<<"?";
                libusb_free_transfer(transfer);
                p->transfer = 0;
                request_exit(p);
                return;
        }

        p->device->onHidData(transfer->buffer, transfer->actual_length);
        //showBytes(transfer->buffer, transfer->actual_length, "read");

        if (libusb_submit_transfer(p->transfer) < 0)
                request_exit(p);
}


void QHidWorker::run()
{
    bool bRun = true;
    if(mHid->epForRead == 0 || mHid->handle == 0) return;

    while(bRun){
        //qDebug()<<mHid->openMode();
        uint8_t buf[128];
        int actLen = 0;
        int maxSize = mHid->caps().inputReportLength;
        int timeout = DEF_TIMEOUT;
        int errorCode = libusb_interrupt_transfer(mHid->handle, mHid->epForRead, buf, maxSize, &actLen, timeout);
        if(errorCode < 0 && errorCode != LIBUSB_ERROR_TIMEOUT) {
            mHid->lastErr = errorCode;
            bRun = false;
            break;
        }
        mHid->onHidData(buf,actLen);
    }
}

//void QHidWorker::run()
//{
//    if(mHid->epForRead == 0 || mHid->handle == 0) return;
//
//    unsigned char buffer[64];
//
//    QByteArray arr = mHid->readData(64);
//    showBytes(arr.data(),arr.length(),"read");
//
//    CB_Data data = {this->mHid, buffer, 0, true};
//
//    data.transfer = libusb_alloc_transfer(0);
//
//    libusb_fill_interrupt_transfer(data.transfer, mHid->handle, mHid->epForRead, buffer,
//                    64, cb_data, &data, 0);
//
//    while(data.bRun){
//        struct timeval tv = { 1, 0 };
//        libusb_handle_events_timeout(0, &tv);
//    }
//    if(data.transfer){
//        libusb_cancel_transfer(data.transfer);
//    }
//
//    while (data.transfer)
//        if (libusb_handle_events(NULL) < 0)
//            break;
//
//    libusb_free_transfer(data.transfer);
//}



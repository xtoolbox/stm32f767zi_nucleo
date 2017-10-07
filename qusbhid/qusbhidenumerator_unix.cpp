#include "qusbhidenumerator.h"
#include <QMetaType>
#include <QDebug>
#include "libusb.h"
#include <QThread>
#include <QMutex>

class QHidMonitor: public QThread
{
public:
    QHidMonitor(QUsbHidEnumerator* parent):QThread(parent),hid(parent){
        //mutex = new QMutex(QMutex::Recursive);
    }
    ~QHidMonitor(){
        //delete mutex;
    }
    virtual void run();
    bool bRun;
private:
    QUsbHidEnumerator* hid;
    void kick_disconnected();
    void scan_new_devices();
    QList<QUsbHidInfo>  lastInfoList;
};

#if defined(QT_GUI_LIB) && defined(Q_OS_WIN)
bool QUsbHidRegistrationWidget::winEvent( MSG* message, long* result )
{
    if ( message->message == WM_DEVICECHANGE ) {
        qese->onDeviceChangeWin( message->wParam, message->lParam );
        *result = 1;
        return true;
    }
    return false;
}
#endif

QUsbHidEnumerator::QUsbHidEnumerator(QObject *parent) :
    QObject(parent)
{
    m_vid = 0;
    m_pid = 0;
    int r = QMetaType::type("QUsbHidInfo");
    if( r == 0 || !QMetaType::isRegistered(r) )
        qRegisterMetaType<QUsbHidInfo>("QUsbHidInfo");

#if (defined QT_GUI_LIB)  && (defined Q_OS_WIN)
    notificationWidget = 0;
#endif // QT_GUI_LIB
    worker = 0;
}

QMutex* QUsbHidEnumerator::mutex = new QMutex(QMutex::Recursive);

QUsbHidEnumerator::~QUsbHidEnumerator()
{
#if (defined QT_GUI_LIB)  && (defined Q_OS_WIN)
    if( notificationWidget )
        delete notificationWidget;
#endif
    if( worker ){
        worker->bRun = false;
        worker->wait(1000);
        worker->terminate();
    }
}

/* Gordon Schumacher's macros for TCHAR -> QString conversions and vice versa */
#ifdef UNICODE
    #define QStringToTCHAR(x)     (wchar_t*) x.utf16()
    #define PQStringToTCHAR(x)    (wchar_t*) x->utf16()
    #define TCHARToQString(x)     QString::fromUtf16((ushort*)(x))
    #define TCHARToQStringN(x,y)  QString::fromUtf16((ushort*)(x),(y))
#else
    #define QStringToTCHAR(x)     x.local8Bit().constData()
    #define PQStringToTCHAR(x)    x->local8Bit().constData()
    #define TCHARToQString(x)     QString::fromLocal8Bit((x))
    #define TCHARToQStringN(x,y)  QString::fromLocal8Bit((x),(y))
#endif /*UNICODE*/

typedef QMap<QString, libusb_device*> DeviceMap_t;
static DeviceMap_t deviceMap;
//static libusb_device **device_list;

extern "C"  void get_usbfs_path(struct libusb_device *dev, char *path);

static QString getDevicePath(libusb_device* dev)
{
    char path[4096];
    get_usbfs_path(dev,path);
    return QString(path);
}


libusb_device* QUsbHidEnumerator::get_device(QString path)
{
    if(deviceMap.find(path) != deviceMap.end()){
        libusb_device* dev = deviceMap.value(path);
        return dev;
    }
    return 0;
}

//Android ADB, source code at: system/core/adb/usb_libusb.c
static bool check_device(libusb_device*  dev, unsigned short vid, unsigned short pid, QUsbHidInfo& info)
{
    // here I only check the VID and PID, we can also need check the interface type, endpoint tpye
    struct libusb_device_descriptor desc;
    libusb_get_device_descriptor(dev, &desc);
    if( (desc.idVendor == vid || 0 == vid) && (desc.idProduct == pid || 0 == pid)){
        QString str = getDevicePath(dev);
        //res = libusb_open(dev, &handle);
        //if(res < 0) handle = 0;
        //if(SN && res >= 0 && desc.iSerialNumber != 0){
        //    res = libusb_get_string_descriptor(handle,desc.iSerialNumber,0,(uint8_t*)sn,256);
        //}
        info.vendorID = desc.idVendor;
        info.productID = desc.idProduct;
        info.version = desc.bcdDevice;
        info.path = str;
        info.physName = str;
        info.friendName = str;
        info.enumName = str;
        return true;
    }
    return false;
}

QList<QUsbHidInfo> QUsbHidEnumerator::getPorts(unsigned short vid, unsigned short pid)
{
    QList<QUsbHidInfo> ports;
    QMutexLocker locker(mutex);
    //deviceMap.clear();
    //vid = 0; pid = 0;
    libusb_device **device_list = 0;
    ssize_t cnt = libusb_get_device_list(NULL, &device_list);
    if(cnt < 0) return ports;
    for (int i = 0; device_list[i]; i++)
    {
        QUsbHidInfo info;
        if(check_device(device_list[i], vid, pid, info)){
            if( !deviceMap.contains(info.path) ){
                deviceMap.insert(info.path, device_list[i]);
                libusb_ref_device(device_list[i]);
            }
            ports.append(info);
        }
    }
    if(device_list){
        libusb_free_device_list(device_list,1);
    }
    return ports;
}

void remove_device(QString path)
{
    if(deviceMap.contains(path)){
        libusb_device* dev = deviceMap.value(path);
        libusb_unref_device(dev);
        deviceMap.remove(path);
    }
}

void QUsbHidEnumerator::setUpNotifications(unsigned short vid, unsigned short pid)
{
    m_vid = vid;
    m_pid = pid;
    // do nothing in linux
    if(worker == 0){
        this->worker = new QHidMonitor(this);
        worker->bRun = true;
        worker->start();
    }
}

bool is_device_removed(libusb_device* dev)
{
    return false;
    bool res = false;
    libusb_device_handle* handle;
    if(dev == 0) {
        qDebug()<<"We should not run to here";
        return true;
    }
    if(libusb_open(dev, &handle) == 0){
        int r = libusb_kernel_driver_active(handle, 0);
        if(r == LIBUSB_ERROR_NO_DEVICE){
            res = true;
        }
        libusb_close(handle);
    }
    return res;
}

void QHidMonitor::kick_disconnected()
{
    QMutexLocker locker(hid->mutex);
    QList<QUsbHidInfo> remainList;
    foreach(const QUsbHidInfo& info, lastInfoList){
        qDebug()<<"try kick:"<<info.path;
        if(is_device_removed( QUsbHidEnumerator::get_device(info.path) )){
            remove_device(info.path);
            emit hid->deviceRemoved(info);
        }else{
            remainList.append(info);
        }
    }
    lastInfoList = remainList;
}


enum{
    DEV_NEW,
    DEV_OLD,
    DEV_REMOVE,
};
void QHidMonitor::scan_new_devices()
{
    QList<QUsbHidInfo> newList = hid->getPorts(hid->m_vid,hid->m_pid);
    QMap<QString,int>  lastInfoMap;
    // mark all device as remove
    foreach(const QUsbHidInfo& info, lastInfoList){
        lastInfoMap.insert(info.path,DEV_REMOVE);
    }
    QList<QUsbHidInfo> incomingList;
    QList<QUsbHidInfo> currentList;

    foreach(const QUsbHidInfo& infoNew, newList){
        if(lastInfoMap.contains(infoNew.path)){
            // mark the device as old
            lastInfoMap[infoNew.path] = DEV_OLD;
        }else{
            // append a new device
            incomingList.append(infoNew);
        }
    }

    foreach(const QUsbHidInfo& info, lastInfoList){
        if(lastInfoMap[info.path] == DEV_REMOVE){
            remove_device(info.path);
            emit hid->deviceRemoved(info);
        }else{
            currentList.append(info);
        }
    }

    foreach(const QUsbHidInfo& info, incomingList){
        emit hid->deviceDiscovered(info);
        currentList.append(info);
    }
    lastInfoList = currentList;
}

void QHidMonitor::run()
{
    while(bRun){
        msleep(200);
        //kick_disconnected();
        scan_new_devices();
    }
}

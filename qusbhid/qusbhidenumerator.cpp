#include "qusbhidenumerator.h"
#include <QMetaType>
#include <QDebug>
#include "./inc/hidsdi++.h"


#ifdef QT_GUI_LIB
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
    HidD_GetHidGuid(&hidGUID);
    if( !QMetaType::isRegistered( QMetaType::type("QUsbHidInfo") ) )
        qRegisterMetaType<QUsbHidInfo>("QUsbHidInfo");
#if (defined QT_GUI_LIB)
    notificationWidget = 0;
#endif // QT_GUI_LIB
}

QUsbHidEnumerator::~QUsbHidEnumerator()
{
#if (defined QT_GUI_LIB)
    if( notificationWidget )
        delete notificationWidget;
#endif
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


QString QUsbHidEnumerator::getRegKeyValue(HKEY key, LPCTSTR property)
{
    unsigned long size = 0;
    unsigned long type;
    RegQueryValueEx(key, property, NULL, NULL, NULL, & size);
    unsigned char* buff = new BYTE[size];
    QString result;
    if( RegQueryValueEx(key, property, NULL, &type, buff, & size) == ERROR_SUCCESS )
        result = TCHARToQString(buff);
    RegCloseKey(key);
    delete [] buff;
    return result;
}

//static
QString QUsbHidEnumerator::getDeviceProperty(HDEVINFO devInfo, PSP_DEVINFO_DATA devData, DWORD property)
{
    DWORD buffSize = 0;
    SetupDiGetDeviceRegistryProperty(devInfo, devData, property, NULL, NULL, 0, & buffSize);
    BYTE* buff = new BYTE[buffSize];
    SetupDiGetDeviceRegistryProperty(devInfo, devData, property, NULL, buff, buffSize, NULL);
    QString result = TCHARToQString(buff);
    delete [] buff;
    return result;
}


LRESULT QUsbHidEnumerator::onDeviceChangeWin( WPARAM wParam, LPARAM lParam )
{
    if ( DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam )
    {
        PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
        if( pHdr->dbch_devicetype == DBT_DEVTYP_DEVICEINTERFACE )
        {
            PDEV_BROADCAST_DEVICEINTERFACE pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
             // delimiters are different across APIs...change to backslash.  ugh.
            QString deviceID = TCHARToQString(pDevInf->dbcc_name);//.toUpper();//.replace("#", "\\");

            matchAndDispatchChangedDevice(deviceID, hidGUID, wParam);
        }
    }
    return 0;
}

bool QUsbHidEnumerator::matchAndDispatchChangedDevice(const QString & deviceID, const GUID & guid, WPARAM wParam)
{
    bool rv = false;
    DWORD dwFlag = (DBT_DEVICEARRIVAL == wParam) ? DIGCF_PRESENT : DIGCF_ALLCLASSES;
    dwFlag |= DIGCF_INTERFACEDEVICE;
    HDEVINFO devInfo;
    QString s;
    if(m_vid && m_pid){
        s = QString("VID_%1&PID_%2").arg((ulong)m_vid,4,16,QChar('0')).arg((ulong)m_pid,4,16,QChar('0'));
    }else if(m_vid){
        s = QString("VID_%1").arg((ulong)m_vid,4,16,QChar('0'));
    }else if(m_pid){
        s = QString("PID_%1").arg((ulong)m_pid,4,16,QChar('0'));
    }
    QRegExp idRx(s.toUpper());
    if( (devInfo = SetupDiGetClassDevs(&guid,NULL,NULL,dwFlag)) != INVALID_HANDLE_VALUE )
    {
        SP_INTERFACE_DEVICE_DATA ifData;
        ifData.cbSize=sizeof(ifData);
        for(int i=0; SetupDiEnumDeviceInterfaces(devInfo, NULL, &guid, i, &ifData); i++)
        {
            DWORD needed;
            SetupDiGetDeviceInterfaceDetail(devInfo, &ifData, NULL, 0, &needed, NULL);
            PSP_INTERFACE_DEVICE_DETAIL_DATA detail=(PSP_INTERFACE_DEVICE_DETAIL_DATA)new BYTE[needed];
            detail->cbSize=sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
            SP_DEVINFO_DATA did={sizeof(SP_DEVINFO_DATA)};
            if (SetupDiGetDeviceInterfaceDetail(devInfo, &ifData, detail, needed, NULL, &did)){
                QString path = TCHARToQString(detail->DevicePath);
                if(deviceID.toUpper().contains(path.toUpper())){
                    rv = true;
                    QUsbHidInfo info;
                    info.path = path;
                    bool ready = false;
                    if(m_vid || m_pid){
                        ready = (info.path.toUpper().contains(idRx));
                    }else{
                        ready = true;
                    }
                    if(ready){
                        info.productID = info.vendorID = info.version = 0;
                        getDeviceDetailsWin( &info, devInfo, &did, wParam );
                        if( wParam == DBT_DEVICEARRIVAL )
                            emit deviceDiscovered(info);
                        else if( wParam == DBT_DEVICEREMOVECOMPLETE )
                            emit deviceRemoved(info);
                    }
                    break;
                }
            }
            delete [] detail;
        }
        SetupDiDestroyDeviceInfoList(devInfo);
    }
    return rv;
}

QList<QUsbHidInfo> QUsbHidEnumerator::getPorts(unsigned short vid, unsigned short pid)
{
    QList<QUsbHidInfo> ports;
    GUID guid;
    HidD_GetHidGuid(&guid);
    enumerateDevicesWin(guid, &ports,vid, pid);
    return ports;
}

void QUsbHidEnumerator::enumerateDevicesWin( const GUID & guid, QList<QUsbHidInfo>* infoList , unsigned short vid, unsigned short pid)
{
    HDEVINFO devInfo;
    QString s;
    if(vid && pid){
        s = QString("VID_%1&PID_%2").arg((ulong)vid,4,16,QChar('0')).arg((ulong)pid,4,16,QChar('0'));
    }else if(vid){
        s = QString("VID_%1").arg((ulong)vid,4,16,QChar('0'));
    }else if(pid){
        s = QString("PID_%1").arg((ulong)vid,4,16,QChar('0'));
    }
    QRegExp idRx(s.toUpper());
    if( (devInfo = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE)) != INVALID_HANDLE_VALUE)
    {
        SP_INTERFACE_DEVICE_DATA ifData;
        ifData.cbSize=sizeof(ifData);
        for (int devIndex=0;SetupDiEnumDeviceInterfaces(devInfo, NULL, &guid, devIndex, &ifData);++devIndex){
            DWORD needed;
            SetupDiGetDeviceInterfaceDetail(devInfo, &ifData, NULL, 0, &needed, NULL);
            PSP_INTERFACE_DEVICE_DETAIL_DATA detail=(PSP_INTERFACE_DEVICE_DETAIL_DATA)new BYTE[needed];
            detail->cbSize=sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
            SP_DEVINFO_DATA did={sizeof(SP_DEVINFO_DATA)};
            if (SetupDiGetDeviceInterfaceDetail(devInfo, &ifData, detail, needed, NULL, &did)){
                QUsbHidInfo info;
                info.path = TCHARToQString(detail->DevicePath);
                bool ready = false;
                if(vid || pid){
                    ready = (info.path.toUpper().contains(idRx));
                }else{
                    ready = true;
                }
                if(ready){
                    info.vendorID = info.productID = info.version;
                    getDeviceDetailsWin( &info, devInfo, &did );
                    infoList->append(info);
                }
            }
            delete [] detail;
        }




//        SP_DEVINFO_DATA devInfoData;
//        devInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
//        for(int i = 0; SetupDiEnumDeviceInfo(devInfo, i, &devInfoData); i++)
//        {
//            QUsbHidInfo info;
//            info.productID = info.vendorID = 0;
//            getDeviceDetailsWin( &info, devInfo, &devInfoData );
//            infoList->append(info);
//        }
        SetupDiDestroyDeviceInfoList(devInfo);
    }
}


void QUsbHidEnumerator::setUpNotifications(unsigned short vid, unsigned short pid)
{
    m_vid = vid;
    m_pid = pid;
    #ifdef QT_GUI_LIB
    if(notificationWidget)
        return;
    notificationWidget = new QUsbHidRegistrationWidget(this);

    DEV_BROADCAST_DEVICEINTERFACE dbh;
    ZeroMemory(&dbh, sizeof(dbh));
    dbh.dbcc_size = sizeof(dbh);
    dbh.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
    CopyMemory(&dbh.dbcc_classguid, &hidGUID, sizeof(GUID));
    if( RegisterDeviceNotification( notificationWidget->winId( ), &dbh, DEVICE_NOTIFY_WINDOW_HANDLE ) == NULL)
        qWarning() << "RegisterDeviceNotification failed:" << GetLastError();
    // setting up notifications doesn't tell us about devices already connected
    // so get those manually
    foreach( QUsbHidInfo port, getPorts(m_vid, m_pid) )
      emit deviceDiscovered( port );
    #else
    qWarning("QextSerialEnumerator: GUI not enabled - can't register for device notifications.");
    #endif // QT_GUI_LIB
}

bool QUsbHidEnumerator::getDeviceDetailsWin( QUsbHidInfo* portInfo, HDEVINFO devInfo, PSP_DEVINFO_DATA devData, WPARAM wParam )
{
    portInfo->friendName = getDeviceProperty(devInfo, devData, SPDRP_DEVICEDESC);
    //portInfo->friendName = getDeviceProperty(devInfo, devData, SPDRP_FRIENDLYNAME);
    if( wParam == DBT_DEVICEARRIVAL)
        portInfo->physName = getDeviceProperty(devInfo, devData, SPDRP_PHYSICAL_DEVICE_OBJECT_NAME);
    portInfo->enumName = getDeviceProperty(devInfo, devData, SPDRP_ENUMERATOR_NAME);
    QString hardwareIDs = getDeviceProperty(devInfo, devData, SPDRP_HARDWAREID);
    //HKEY devKey = SetupDiOpenDevRegKey(devInfo, devData, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_READ);
    //portInfo->path = ( getRegKeyValue(devKey, TEXT("PortName")) );
    QRegExp idRx("VID_(\\w+)&PID_(\\w+)&REV_(\\w+)");
    if( hardwareIDs.toUpper().contains(idRx) )
    {
        bool dummy;
        portInfo->vendorID = idRx.cap(1).toInt(&dummy, 16);
        portInfo->productID = idRx.cap(2).toInt(&dummy, 16);
        portInfo->version = idRx.cap(3).toInt(&dummy, 16);
        //qDebug() << "got vid:" << vid << "pid:" << pid;
    }
    return true;
}

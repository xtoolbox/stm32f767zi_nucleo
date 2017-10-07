#ifndef QUSBHIDENUMERATOR_H
#define QUSBHIDENUMERATOR_H

#include <QObject>
#ifdef Q_OS_WIN
    #include <windows.h>
    #include <setupapi.h>
    #include <dbt.h>
#endif /*Q_OS_WIN*/

#ifdef Q_OS_MAC
    #include <IOKit/usb/IOUSBLib.h>
#endif

#ifdef Q_OS_UNIX
#include "libusb.h"
#include <QMutex>
#endif

class QUsbHidEnumerator;

struct QUsbHidInfo {
    QString physName;   ///< Physical name.
    QString friendName; ///< Friendly name.
    QString enumName;   ///< Enumerator name.
    QString path;
    int vendorID;       ///< Vendor ID.
    int productID;      ///< Product ID
    int version;        ///< Version No.
};

#ifdef Q_OS_WIN
#ifdef QT_GUI_LIB
#include <QWidget>

class QUsbHidRegistrationWidget : public QWidget
{
    Q_OBJECT
    public:
        QUsbHidRegistrationWidget( QUsbHidEnumerator* qese ) {
            this->qese = qese;
        }
        ~QUsbHidRegistrationWidget( ) { }

    protected:
        QUsbHidEnumerator* qese;
        bool winEvent( MSG* message, long* result );
};
#endif // QT_GUI_LIB
#endif // Q_OS_WIN

class QHidMonitor;
class QUsbHidEnumerator : public QObject
{
    Q_OBJECT
public:
    explicit QUsbHidEnumerator(QObject *parent = 0);
    ~QUsbHidEnumerator();
#ifdef Q_OS_WIN
    LRESULT onDeviceChangeWin( WPARAM wParam, LPARAM lParam );
private:
    static QString getRegKeyValue(HKEY key, LPCTSTR property);
    static QString getDeviceProperty(HDEVINFO devInfo, PSP_DEVINFO_DATA devData, DWORD property);
    bool matchAndDispatchChangedDevice(const QString & deviceID, const GUID & guid, WPARAM wParam);
    static bool getDeviceDetailsWin( QUsbHidInfo* portInfo, HDEVINFO devInfo,
                          PSP_DEVINFO_DATA devData, WPARAM wParam = DBT_DEVICEARRIVAL );
    static void enumerateDevicesWin( const GUID & guid, QList<QUsbHidInfo>* infoList, WORD vid, WORD pid);
    GUID hidGUID;
#ifdef QT_GUI_LIB
     QUsbHidRegistrationWidget* notificationWidget;
#endif
#endif /* Q_OS_WIN */


public:
#ifdef Q_OS_UNIX
    static  libusb_device* get_device(QString path);
    static QMutex*  mutex;
    QHidMonitor* worker;
    friend class QHidMonitor;
#endif
    /*!
      Get list of ports.
      \return list of ports currently available in the system.
    */
    static QList<QUsbHidInfo> getPorts(){ return getPorts(0,0); }
    static QList<QUsbHidInfo> getPorts(unsigned short vid){ return getPorts(vid,0); }
    static QList<QUsbHidInfo> getPorts(unsigned short vid, unsigned short pid);
    /*!
      Enable event-driven notifications of board discovery/removal.
    */
    void setUpNotifications(){ setUpNotifications(0,0); }
    void setUpNotifications(unsigned short vid){ setUpNotifications(vid,0); }
    void setUpNotifications(unsigned short vid, unsigned short pid);


signals:
    void deviceDiscovered( const QUsbHidInfo & info );
    void deviceRemoved( const QUsbHidInfo & info );
public slots:
protected:
    unsigned short m_vid;
    unsigned short m_pid;
};

#endif // QUSBHIDENUMERATOR_H

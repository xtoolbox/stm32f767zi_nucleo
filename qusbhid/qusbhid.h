#ifndef QUSBHID_H
#define QUSBHID_H

#include <QIODevice>
#include <QMutex>
#include <QMutexLocker>
#include <QReadWriteLock>

#ifdef Q_OS_WIN
    #include <windows.h>
    #include <setupapi.h>
    #include <dbt.h>
    #include "qwineventnotifier_p.h"
    #include "./inc/hidsdi++.h"
#else
#include "libusb.h"
#endif /*Q_OS_WIN*/
#include "qusbhidenumerator.h"

struct QHidAttr
{
    QHidAttr(){ memset(this,0,sizeof(QHidAttr)); }
#ifdef Q_OS_WIN
    QHidAttr(const HIDD_ATTRIBUTES& attr){
        vendorID = attr.VendorID;
        productID = attr.ProductID;
        version = attr.VersionNumber;
    }
#endif
    QHidAttr(unsigned short vid, unsigned short pid, unsigned short ver){
        vendorID = vid;
        productID = pid;
        version = ver;
    }
    int vendorID;
    int productID;
    int version;
};

struct QHidCaps
{
    QHidCaps(){ memset(this,0,sizeof(QHidCaps));}
#ifdef Q_OS_WIN
    QHidCaps(const HIDP_CAPS& cap){
        usage = cap.Usage;
        uasgePage = cap.UsagePage;
        inputReportLength = cap.InputReportByteLength;
        outputReportLength = cap.OutputReportByteLength;
        featureReportLength = cap.FeatureReportByteLength;
    }
#endif
    QHidCaps(int usage, int page, int inputLen, int outputLen, int featureLen){
        this->usage = usage;
        uasgePage = page;
        inputReportLength = inputLen;
        outputReportLength = outputLen;
        featureReportLength = featureLen;
    }
    int usage;
    int uasgePage;
    int inputReportLength;
    int outputReportLength;
    int featureReportLength;
};

#ifdef Q_OS_UNIX
class QHidWorker;
#endif

class QUsbHid : public QIODevice
{
    Q_OBJECT
public:
    enum QueryMode {
        Polling,
        EventDriven
    };

    QUsbHid(QObject* parent = 0, QueryMode mode = EventDriven);
    QUsbHid(const QString& path, QObject* parent = 0, QueryMode mode = EventDriven);
    ~QUsbHid();

    void setPath(const QString& path){ m_path = path; }
    QString path()const { return m_path; }

    void setQueryMode(int mode) {m_queryMode = QueryMode(mode); }
    int queryMode() const { return m_queryMode; }

    void setTimeout(int timeout){ m_timeout = timeout; }
    int timeout()const { return m_timeout; }

    bool open(OpenMode mode);
    bool isSequential() const {return true; }
    void close();
    void flush();

    qint64 bytesAvailable() const;
    qint64 bytesToWrite() const;
    QByteArray readAll();
    QByteArray readData(qint64 len);
    QByteArray readData();
    qint64 writeData(const QByteArray& data);
    qint64 writeData(int reportID, const QByteArray& data);

    virtual qint64 readData(char *data, qint64 maxlen);
    virtual qint64 writeData(const char *data, qint64 len);


    QHidCaps  caps() const;

    static QList<QUsbHidInfo> enumDevices(int vid, int pid);
    static QList<QUsbHidInfo> enumDevices(int vid){ return enumDevices(vid,0); }
    static QList<QUsbHidInfo> enumDevices(){ return enumDevices(0,0); }

    void monitor(int vid, int pid){ if(enumerator) enumerator->setUpNotifications((unsigned short)vid,(unsigned short)pid);}
    void monitor(int vid){ monitor(vid,0); }
    void monitor(){ monitor(0,0); }

    QString errorString()const;

    QHidAttr GetAttributes(bool* r);
    QByteArray getFeature(bool* r);
    QByteArray getFeature(int maxLen, bool* r);
    QByteArray getFeature(int reportID, int maxLen,bool* r);

    QByteArray getInputReport(bool* r);
    QByteArray getInputReport(int maxLen,bool* r);
    QByteArray getInputReport(int reportID, int maxLen,bool* r);

    int getNumInputBuffers(bool* r);
    QByteArray getPhysicalDescriptor(int maxLen, bool* r);

    QString getIndexedString(int index,bool* r);
    QString getManufacturerString(bool* r);
    QString getProductString(bool* r);
    QString getSerialNumberString(bool* r);

    bool setFeature(const QByteArray& arr);
    bool setFeature(int reportID, const QByteArray& arr);
    bool setOutputReport(const QByteArray& arr);
    bool setOutputReport(int reportID, const QByteArray& arr);

    bool setNumInputBuffers(int num);

signals:
    void connected( const QUsbHidInfo & info );
    void disconnected( const QUsbHidInfo & info );
public slots:
#ifdef Q_OS_WIN
    private slots:
        void onWinEvent(HANDLE h);
#else
    public slots:
        void onHidData(void* p, int len);
#endif
protected:
    void initial();
    void deinitial();

    QueryMode m_queryMode;
    QString m_path;
    int m_timeout;
    QMutex* mutex;

#if (defined Q_OS_WIN)
        HANDLE Win_Handle;
        OVERLAPPED overlap;
        QWinEventNotifier *winEventNotifier;
        DWORD eventMask;
        QList<OVERLAPPED*> pendingWrites;
        QReadWriteLock* bytesToWriteLock;
        qint64 _bytesToWrite;
        QReadWriteLock* readBufferLock;
        QByteArray  readBuffer;
        PHIDP_PREPARSED_DATA  hidPrepParsedData;
        HIDP_CAPS             hidCaps;
        char   rawData[128];
        DWORD lastErr;
#endif
#if (defined Q_OS_UNIX)
    libusb_device_handle*  handle;
    libusb_device_descriptor device_desc;
    int epForRead;
    int epForWrite;
    int interface;

    unsigned long eventMask;
    QReadWriteLock* bytesToWriteLock;
    qint64 _bytesToWrite;
    QReadWriteLock* readBufferLock;
    QByteArray  readBuffer;
    char   rawData[128];
    int lastErr;
    QHidCaps hidCaps;
    QString permissionError;

    friend class QHidWorker;

    QHidWorker* worker;
#endif
    private:
        QUsbHidEnumerator *enumerator;
        bool m_isOpen;
};

#endif // QUSBHID_H

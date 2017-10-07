#include "mainwindow.h"
#include "qusbhidenumerator.h"
#include "qusbhid.h"

QUsbHidEnumerator* hidenum = 0;
QUsbHid* hid = 0;
QString hidpath;
void test_hid_devices(MainWindow* mainWindow);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    this->setWindowIcon(QIcon(":res/123.png"));
    mdi = new QMdiArea(this);
    this->setCentralWidget(mdi);
    QDockWidget* dock = new QDockWidget(tr("Script log"),this);
    dock->setObjectName(tr("ScriptLogDock"));
    logText = new QTextEdit(dock);
    logText->setReadOnly(true);
    logText->setLineWrapMode(QTextEdit::NoWrap);
    dock->setWidget(logText);
    this->addDockWidget(Qt::BottomDockWidgetArea, dock);
    if(hidenum == 0){
        hidenum = new QUsbHidEnumerator(this);
    }
    if(hid == 0){
        hid = new QUsbHid(this);
    }
    connect(hidenum, SIGNAL(deviceDiscovered(QUsbHidInfo)), this, SLOT(devconnect(QUsbHidInfo)));
    connect(hidenum, SIGNAL(deviceRemoved(QUsbHidInfo)), this, SLOT(devdisconnect(QUsbHidInfo)));
    connect(hid, SIGNAL(readyRead()), this, SLOT(readyReadData()));
    hidenum->setUpNotifications(0x250,0x250);

    QList<QUsbHidInfo> devs = QUsbHid::enumDevices(0x250,0x250);
    if(devs.size()){
        hidpath = devs.at(0).path;
    }
    QMenu* menu = 0;
    QList<QAction*> list = menuBar()->actions();
    foreach(QAction* act, list){
        if( act->text().contains(tr("help"),Qt::CaseInsensitive) ){
            menu = act->menu();
        }
    }

    if(menu == 0){
        menu = menuBar()->addMenu(tr("&Help"));
    }
    menu->addAction(dock->toggleViewAction());
    menu->addSeparator();
    QAction* act = menu->addAction(tr("&About..."));
    connect(act,SIGNAL(triggered()),this,SLOT(my_about()));
    act = menu->addAction(tr("Send"));
    connect(act,SIGNAL(triggered()),this,SLOT(my_send()));
    this->setWindowTitle(QString::fromLocal8Bit("Tool Box"));

    //test_hid_devices(this);
}

MainWindow::~MainWindow()
{

}

void MainWindow::addLog(const QString& log)
{
    logText->append(log);
}

void MainWindow::my_send()
{
    hid->writeData("123123",16);
    qDebug()<<hid->getManufacturerString(0);
    qDebug()<<hid->getProductString(0);
    qDebug()<<hid->getSerialNumberString(0);
}

void MainWindow::my_about()
{
    //QMessageBox::about(this, "Toolbox", QString::fromLocal8Bit("<p><b>Toolbox</b></p><p>by lxyppc</p>lxyppc@163.com"));
    if(hid->isOpen()){
        hid->close();
        qDebug()<<"close device";
    }else{
        if(hidpath != ""){
            hid->setPath(hidpath);
            hid->open(QUsbHid::ReadWrite);
            qDebug()<<"open device";
        }
    }
}

void dbg_arr(const QByteArray& arr)
{
    QString s;
    for(int i=0; i< arr.size(); i++){
        s += QString("%1,").arg((uint)arr.at(i)&0xff,2,16,QChar('0'));
        if( (i&15) == 15) {
            qDebug()<<s;
            s = "";
        }
    }
    if(!s.isEmpty()) qDebug()<<s;
}
void MainWindow::readyReadData()
{
    dbg_arr(hid->readAll());
    //qDebug()<<hid->readAll().length();
}

void MainWindow::devconnect(const QUsbHidInfo& info)
{
    qDebug()<<"devconnect";
    qDebug()<<info.enumName;
    qDebug()<<info.friendName;
    qDebug()<<info.physName;
    qDebug()<<info.path;
    qDebug()<<info.productID;
    qDebug()<<info.vendorID;
    hidpath = info.path;
}

void MainWindow::devdisconnect(const QUsbHidInfo& info)
{
    qDebug()<<"devdisconnect";
    qDebug()<<info.enumName;
    qDebug()<<info.friendName;
    qDebug()<<info.physName;
    qDebug()<<info.path;
    qDebug()<<info.productID;
    qDebug()<<info.vendorID;
    hidpath = "";
}


void test_hid_devices(MainWindow* mainWindow)
{
    QList<QUsbHidInfo> devs = QUsbHid::enumDevices(0x250,0x250);
    if(devs.size()){
        qDebug()<<"find "<<devs.size()<<"devices";
        foreach(const QUsbHidInfo& info , devs){
            QString str("vid:%1 pid:%2");
            str = str.arg(info.vendorID,4,16,QChar('0')).arg(info.productID,4,16,QChar('0'));
            qDebug()<<info.path<<str;
        }

        const QUsbHidInfo& info = devs.at(0);
        QUsbHid* hid = new QUsbHid(mainWindow);
        hid->setPath(info.path);
        bool res = hid->open(QIODevice::ReadWrite);
        qDebug()<<"open "<<info.path<<"="<<res;
        if(!res)
            qDebug()<<hid->errorString();
    }
}












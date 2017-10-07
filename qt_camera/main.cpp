#include <QApplication>
#include <QMainWindow>
#include <QFontDialog>
#include <QtGui>
#include <QThread>
#include <QDialog>
#include "qusbhid.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <rtthread.h>
#include <rtgui/kbddef.h>
#ifdef __cplusplus
}
#endif
#include <opencv2/opencv.hpp>
int translate_qt_key_to_rt(int key)
{
    //printf("%d %x\n", key, key);
    switch(key){
    case Qt::Key_Enter: return RTGUIK_KP_ENTER;
    case Qt::Key_Escape:  return RTGUIK_ESCAPE;
    case Qt::Key_Backspace: return RTGUIK_BACKSPACE;
    case Qt::Key_Delete: return RTGUIK_DELETE;
    }
    if(key>= Qt::Key_A && key <= Qt::Key_Z){
        key = key - Qt::Key_A + RTGUIK_a;
    }
    return key;
}
#define  CT_MOUSE  0
#define  CT_KEY    1
typedef struct{
        uint32_t x;
        uint32_t y;
        uint32_t button;
        uint32_t is_press;
}mouse_data_t;

typedef struct{
        uint32_t key;
        uint32_t modifier;
        uint32_t padding;
        uint32_t is_press;
}key_data_t;

typedef struct
{
        uint32_t type;  // 0 mouse, 1 keyboard
        union{
                mouse_data_t mouse;
                key_data_t key;
        }data;
}control_t;


class QMyMainWindow : public QMainWindow
{
public:
    QMyMainWindow():myLabel(0){
        QList<QUsbHidInfo> list = QUsbHid::enumDevices(0x0483, 0x5740);
        if(list.size()>0){
            mHid.setPath(list.at(0).path);
            mHid.setQueryMode(QUsbHid::Polling);
            mHid.open(QUsbHid::ReadWrite);
        }else{
            info("Hid device not found!\r\nInput simulation not work!");
        }
        mCapId = 0;
        mCap = 0;
        open_camera();
        startTimer(50);
    }
    ~QMyMainWindow(){
        mHid.close();
    }
    void open_camera(void){
        QString cam_name = QString("Camera - %1").arg(mCapId);
        mCap = cvCreateCameraCapture(mCapId);
        if(!mCap){
            QString info_str = cam_name + " not found!\n\nUse PageUP or PageDown to select other Camera!";
            //info(info_str);
            QImage img(320, 240, QImage::Format_ARGB32);
            QPainter pt(&img);
            pt.setBrush(QBrush(QColor("lightgray")));
            pt.setPen(QColor("lightgray"));
            pt.drawRect(0,0,320,240);
            pt.setPen(QColor("darkred"));
            QFont f = pt.font();
            f.setBold(true);
            f.setPixelSize(18);
            pt.setFont(f);
            pt.drawText(20,20,280, 200, Qt::TextWordWrap | Qt::AlignHCenter, info_str);
            myLabel->setPixmap(QPixmap::fromImage(img));

        }
        setWindowTitle(cam_name);
    }
    void keyPressEvent(QKeyEvent* evt){
        key_action(translate_qt_key_to_rt(evt->key()), (int)evt->modifiers(), 1);
        QMainWindow::keyPressEvent(evt);
    }
    void keyReleaseEvent(QKeyEvent* evt){
        bool new_camera = false;
        if(evt->key() == Qt::Key_PageUp){
            mCapId = mCapId - 1;
            if(mCapId<0)mCapId = 5;
            new_camera = true;
        }
        if(evt->key() == Qt::Key_PageDown){
            mCapId = mCapId + 1;
            if(mCapId>5)mCapId = 0;
            new_camera = true;
        }
        if(new_camera = true){
            if(mCap){
                cvReleaseCapture(&mCap);
                mCap = 0;
            }
            open_camera();
        }else{
            key_action(translate_qt_key_to_rt(evt->key()), (int)evt->modifiers(), 0);
        }
        QMainWindow::keyReleaseEvent(evt);
    }
    void timerEvent(QTimerEvent* evt){
        if(mCap){
            IplImage* frame = cvQueryFrame(mCap);
            if(frame){
                QImage img((const uchar *)frame->imageData, frame->width, frame->height, QImage::Format_RGB888);
                if(myLabel){
                    myLabel->setPixmap(QPixmap::fromImage(img.rgbSwapped()));
                }
            }
        }
    }

    void mouse_action(int x, int y, int btn, int is_down)
    {
        control_t t;
        t.type = CT_MOUSE;
        t.data.mouse.x = x;
        t.data.mouse.y = y;
        t.data.mouse.button = btn;
        t.data.mouse.is_press = is_down;
        if(mHid.isOpen())
        mHid.writeData(0,QByteArray((char*)&t, sizeof(t)));
    }

    void key_action(int btn, int mod, int is_press)
    {
        control_t t;
        t.type = CT_KEY;
        t.data.key.key = btn;
        t.data.key.modifier = mod;
        t.data.key.padding = 0;
        t.data.key.is_press = is_press;
        if(mHid.isOpen())
        mHid.writeData(0,QByteArray((char*)&t, sizeof(t)));
    }
    void info(const QString& str){
        QMessageBox::information(this, this->windowTitle(), str);
    }
    QLabel* myLabel;
protected:
    QUsbHid mHid;
    CvCapture* mCap;
    int mCapId;
};
class QMyLabel : public QLabel
{
public:
    QMyLabel(QMyMainWindow& mainWindow):mMainWindow(mainWindow){mMainWindow.myLabel = this;}
   void mousePressEvent(QMouseEvent* evt){
       mMainWindow.mouse_action(evt->x(), evt->y(), evt->button(), 1);
       QLabel::mousePressEvent(evt);
   }
   void mouseReleaseEvent(QMouseEvent* evt){
       mMainWindow.mouse_action(evt->x(), evt->y(), evt->button(), 0);
       QLabel::mouseReleaseEvent(evt);
   }
   QMyMainWindow& mMainWindow;
};

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QMyMainWindow w;
    QMyLabel* label = new QMyLabel(w);
    label->setPixmap(QPixmap::fromImage(QImage(320, 240, QImage::Format_RGB888)));
    w.setCentralWidget(label);
    w.show();
    a.exec();
    return 0;
}



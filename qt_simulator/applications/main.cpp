#include <QApplication>
#include <QMainWindow>
#include <QFontDialog>
#include <QtGui>
#include <QThread>
#include <QDialog>
#include "usbd_video.h"

class MyThread : public QThread
{
public:
    void run();
};


extern "C" void mouse_action(int x, int y, int btn, int is_down);

class QMyLabel : public QLabel
{
public:
    QMyLabel():new_data(0){}
   void paintEvent(QPaintEvent* event){
       if(new_data){
           setPixmap(QPixmap::fromImage(QImage(new_data, 320,240, QImage::Format_RGB16)));
           delete [] new_data;
           new_data = 0;
       }
       QLabel::paintEvent(event);
       //event->accept();
   }
#define  PRINT_EVT(name) printf(name " (%8d,%8d) %d\n",evt->x(), evt->y(), evt->button())
   void mouseMoveEvent(QMouseEvent* evt){
       PRINT_EVT("MouseMove ");
       QLabel::mouseMoveEvent(evt);
   }
   void mousePressEvent(QMouseEvent* evt){
       PRINT_EVT("MouseDown ");
       mouse_action(evt->x(), evt->y(), evt->button(), 1);
       QLabel::mousePressEvent(evt);
   }
   void mouseReleaseEvent(QMouseEvent* evt){
       PRINT_EVT("MouseUp   ");
       mouse_action(evt->x(), evt->y(), evt->button(), 0);
       QLabel::mouseReleaseEvent(evt);
   }
   void keyPressEvent(QKeyEvent* evt){
       printf("KeyD      %d\n",evt->key());
       QLabel::keyPressEvent(evt);
   }
   void update(const void* data, int size){
       new_data = new uchar[size];
       memcpy(new_data, data, size);
       QLabel::update();
   }
   uchar* new_data;
};

QMyLabel* glabel =0;
uint16_t buf[320*240*2];
extern "C" uint32_t get_usb_lcd_address(void);

extern "C" int update_image(int color)
{
    if(glabel){
        memset(buf, color, sizeof(buf));
        glabel->update((void*)get_usb_lcd_address(), WIDTH*HEIGHT*2);
    }
}


extern "C" int rt_usb_lcd_init(void);
extern "C" int rt_gui_demo_init(void);

void MyThread::run()
{
    int argc = 1;
    char p[] = "test.ext";
    char *argv [] = { p };
    QApplication a(argc, argv);

    //QDialog* dlg = new QDialog();
    //dlg->exec();
    //delete dlg;


    QMainWindow w;
    QMyLabel* label = new QMyLabel();
    label->setPixmap(QPixmap::fromImage(QImage(320,240, QImage::Format_RGB16)));
    glabel = label;
    w.setCentralWidget(label);
    w.show();
    a.exec();
    glabel = 0;
}
MyThread thread;
extern "C" void rt_hw_qt_simulator_start(void)
{
    rt_usb_lcd_init();
    rt_gui_demo_init();
    thread.start();
    return;
}

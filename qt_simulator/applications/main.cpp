#include <QApplication>
#include <QMainWindow>
#include <QFontDialog>
#include <QtGui>
#include <QThread>
#include <QDialog>
#include "usbd_video.h"
#ifdef __cplusplus
extern "C" {
#endif
#include <rtthread.h>
#include <rtgui/kbddef.h>
#ifdef __cplusplus
}
#endif
class MyThread : public QThread
{
public:
    MyThread():ready(0){}
    void run();
    volatile int ready;
};
extern "C" void mouse_action(int x, int y, int btn, int is_down);
extern "C" void key_action(int btn, int mod, int is_press);

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

class QMyMainWindow : public QMainWindow
{
public:
    QMyMainWindow(){}
    void keyPressEvent(QKeyEvent* evt){
        //printf("KeyD      %d\n",evt->key());
        key_action(translate_qt_key_to_rt(evt->key()), (int)evt->modifiers(), 1);
        QMainWindow::keyPressEvent(evt);
    }
    void keyReleaseEvent(QKeyEvent* evt){
        //printf("KeyU      %d\n",evt->key());
        key_action(translate_qt_key_to_rt(evt->key()), (int)evt->modifiers(), 0);
        QMainWindow::keyReleaseEvent(evt);
    }
};
class QMyLabel : public QLabel
{
public:
    QMyLabel():has_new_data(0){}
   void paintEvent(QPaintEvent* event){
       if(has_new_data){
           setPixmap(QPixmap::fromImage(QImage(new_data, 320,240, QImage::Format_RGB16)));
           has_new_data = 0;
       }
       QLabel::paintEvent(event);
       //event->accept();
   }
#define  PRINT_EVT(name) //printf(name " (%8d,%8d) %d\n",evt->x(), evt->y(), evt->button())
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
       has_new_data = 1;
       memcpy(new_data, data, size);
       QLabel::update();
   }
   uchar new_data[WIDTH*HEIGHT*2];
   int has_new_data;
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
    return 0;
}


extern "C" int rt_usb_lcd_init(void);
extern "C" int rt_gui_demo_init(void);
extern "C" int calc_application_init(void);

void MyThread::run()
{
    int argc = 1;
    char p[] = "test.ext";
    char *argv [] = { p };
    QApplication a(argc, argv);
    QMyMainWindow w;
    QMyLabel* label = new QMyLabel();
    label->setPixmap(QPixmap::fromImage(QImage(320,240, QImage::Format_RGB16)));
    glabel = label;
    ready = 1;
    w.setCentralWidget(label);
    w.show();
    a.exec();
    glabel = 0;
}
MyThread thread;
extern "C" void rt_hw_qt_simulator_start(void)
{
    calc_application_init();
    thread.start();
    while(!thread.ready);
    return;
}

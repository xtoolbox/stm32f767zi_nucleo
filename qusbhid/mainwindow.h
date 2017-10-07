#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "qusbhidenumerator.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void addLog(const QString& log);

    QTextEdit* getLogEdit() const { return logText;}
    QMdiArea* getMdiArea()const { return mdi; }

public slots:
    void my_about();
    void my_send();
    void devconnect(const QUsbHidInfo& info);
    void devdisconnect(const QUsbHidInfo& info);
    void readyReadData();
private:
    QMdiArea* mdi;
    QTextEdit* logText;
};

#endif // MAINWINDOW_H

# -------------------------------------------------
# Project created by QtCreator 2012-03-22T13:33:06
# -------------------------------------------------
TARGET = usbhid
TEMPLATE = app
QT += network
SOURCES += main.cpp \
    mainwindow.cpp
HEADERS += mainwindow.h \
    qusbhidenumerator.h \
    qusbhid.h

win32 {
    DEFINES += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
    LIBS += -lsetupapi -lhid
    SOURCES += qusbhidenumerator.cpp \
    qusbhid.cpp
    HEADERS += qwineventnotifier_p.h
}

unix {
    INCLUDEPATH += ./libusb ./qusbhid ../libusb
    SOURCES += qusbhidenumerator_unix.cpp \
    qusbhid_unix.cpp
    SOURCES += ../usb/usbdevice_unix.cpp \
        ../libusb/core.c \
        ../libusb/descriptor.c \
        ../libusb/io.c \
        ../libusb/sync.c
}
unix:!macx: SOURCES += ../libusb/os/linux_usbfs.c
macx: SOURCES += ../libusb/os/darwin_usb.c
macx {
    LIBS += -framework \
        IOKit \
        -framework \
        CoreFoundation
}

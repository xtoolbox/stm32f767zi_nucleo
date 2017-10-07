# Qt project file create by rt-thread
QT += core gui
TARGET = qcamera
TEMPLATE = app

# source files
SOURCES += main.cpp
# USB HID source file
SOURCES += ../qusbhid/qusbhid.cpp \
        ../qusbhid/qusbhidenumerator.cpp
    HEADERS += ../qusbhid/qusbhidenumerator.h \
        ../qusbhid/qusbhid.h
    DEFINES += WINVER=0x0501 # needed for mingw to pull in appropriate dbt business...probably a better way to do this
    LIBS += -lsetupapi \
        -lhid

# head files


# Include path
INCLUDEPATH += .
INCLUDEPATH += ../../rt-thread/components/drivers/include
INCLUDEPATH += ../../rt-thread/components/finsh
INCLUDEPATH += ../../rt-thread/components/gui/include
INCLUDEPATH += ../../rt-thread/include
INCLUDEPATH += ../qusbhid
INCLUDEPATH += ../opencv/build/include


LIBS += -L$$OUT_PWD/../../opencv/qt_build/staticlib
# open CV library
LIBS += -lopencv_highgui2413
LIBS += -lopencv_core2413








DEFINES += __fsymtab_start=__start_FSymTab


# libs
# lib paths

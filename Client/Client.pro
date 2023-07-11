#-------------------------------------------------
#
# Project created by QtCreator 2011-09-23T22:07:00
#
#-------------------------------------------------

QT       += core gui network sql
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
RC_ICONS = icon/friend1.ico
TARGET = FeiQ_3_lk
TEMPLATE = app





SOURCES += main.cpp\
    findpsd.cpp \
    login.cpp \
    mainwindow.cpp \
    p2p.cpp \
    register.cpp \
    sendfile.cpp

HEADERS  += \
    findpsd.h \
    login.h \
    mainwindow.h \
    p2p.h \
    register.h \
    sendfile.h

FORMS    += \
    findpsd.ui \
    login.ui \
    mainwindow.ui \
    p2p.ui \
    register.ui \
    sendfile.ui

RESOURCES += \
      Resource.qrc

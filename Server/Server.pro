#-------------------------------------------------
#
# Project created by QtCreator 2011-09-21T09:49:59
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Servers_1
TEMPLATE = app
SOURCES += main.cpp\
        mysql.cpp \
        server.cpp
HEADERS  += \
    mysql.h \
    server.h
FORMS    += \
    server.ui

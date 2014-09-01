#-------------------------------------------------
#
# Project created by QtCreator 2014-08-11T12:49:36
#
#-------------------------------------------------

QT       += core network gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Cheker
TEMPLATE = app


SOURCES += \
    Account.cpp \
    Cheker.cpp \
    Main.cpp \
    pop3client.cpp \
    information.cpp

HEADERS  += \
    Account.h \
    Cheker.h \
    pop3client.h \
    information.h

FORMS    += \
    Cheker.ui

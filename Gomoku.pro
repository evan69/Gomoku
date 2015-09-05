#-------------------------------------------------
#
# Project created by QtCreator 2015-09-01T15:17:29
#
#-------------------------------------------------

QT       += core gui
QT += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gomoku
TEMPLATE = app


SOURCES += main.cpp\
        dialog.cpp \
    showip.cpp \
    choosemode.cpp \
    inputip.cpp

HEADERS  += dialog.h \
    showip.h \
    choosemode.h \
    inputip.h

FORMS    += dialog.ui \
    showip.ui \
    choosemode.ui \
    inputip.ui

######################################################################
# Automatically generated by qmake (3.0) Mon Sep 25 23:02:59 2017
######################################################################

QT       += core gui bluetooth
QT       += serialport
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
CONFIG += c++11 qt thread

TEMPLATE = app
TARGET = Application
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -limg_proc -lopencv_imgproc -lopencv_core -lopencv_highgui -lopencv_videoio

# Input
HEADERS += ./include/*
FORMS += mainwindow.ui
SOURCES += ./src/*

# Output
OBJECTS_DIR=./obj
MOC_DIR=./moc

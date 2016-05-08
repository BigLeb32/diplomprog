QT       += core gui
QT       += sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = diplomprog
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    mainwindowgrafik.cpp \
    qcpdocumentobject.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    mainwindowgrafik.h \
    qcpdocumentobject.h \
    qcustomplot.h

FORMS    += mainwindow.ui \
    reportrowcount.ui \
    mainwindowgrafik.ui



include(/home/bigleb32/diplomprog/LimeReport/report-lib.pri)



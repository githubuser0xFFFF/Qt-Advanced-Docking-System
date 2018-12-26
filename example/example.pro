#-------------------------------------------------
#
# Project created by QtCreator 2018-12-14T22:42:14
#
#-------------------------------------------------
ADS_ROOT = $${PWD}/..
ADS_OUT_ROOT = $${OUT_PWD}/..

QT       += core gui widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Example1
DESTDIR = $${ADS_OUT_ROOT}/lib
TEMPLATE = app
CONFIG *= c++14

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        MainWindow.cpp

HEADERS += \
        MainWindow.h

FORMS += \
        MainWindow.ui
        
#RESOURCES += main.qrc

LIBS += -L$${ADS_OUT_ROOT}/lib

# Dependency: AdvancedDockingSystem (shared)
win32:CONFIG(release, debug|release): LIBS += -lqtadvanceddocking
else:win32:CONFIG(debug, debug|release): LIBS += -lqtadvanceddockingd
else:unix: LIBS += -lqtadvanceddocking

INCLUDEPATH += ../src
DEPENDPATH += ../src    


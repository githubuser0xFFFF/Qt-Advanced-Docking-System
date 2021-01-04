ADS_OUT_ROOT = $${OUT_PWD}/..

TARGET = AdvancedDockingSystemDemo
DESTDIR = $${ADS_OUT_ROOT}/lib
QT += core gui widgets

include(../ads.pri)

lessThan(QT_MAJOR_VERSION, 6) {
    win32 {
        QT += axcontainer
    }
}

CONFIG += c++14
CONFIG += debug_and_release
DEFINES += QT_DEPRECATED_WARNINGS

adsBuildStatic {
    DEFINES += ADS_STATIC
}

SOURCES += \
	main.cpp \
	MainWindow.cpp \
	StatusDialog.cpp

HEADERS += \
	MainWindow.h \
	StatusDialog.h

FORMS += \
	mainwindow.ui \
	StatusDialog.ui
	
RESOURCES += demo.qrc


LIBS += -L$${ADS_OUT_ROOT}/lib


INCLUDEPATH += ../src
DEPENDPATH += ../src

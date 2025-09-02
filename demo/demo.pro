ADS_OUT_ROOT = $${OUT_PWD}/..

TARGET = AdvancedDockingSystemDemo
DESTDIR = $${ADS_OUT_ROOT}/lib
QT += core gui widgets quick quickwidgets

include(../ads.pri)

lessThan(QT_MAJOR_VERSION, 6) {
    win32 {
        QT += axcontainer
    }
}

CONFIG += c++14
CONFIG += debug_and_release
DEFINES += QT_DEPRECATED_WARNINGS
RC_FILE += app.rc

adsBuildStatic {
    DEFINES += ADS_STATIC
}


HEADERS += \
	MainWindow.h \
	StatusDialog.h \
	ImageViewer.h \
	RenderWidget.h

SOURCES += \
	main.cpp \
	MainWindow.cpp \
	StatusDialog.cpp \
	ImageViewer.cpp \
	RenderWidget.cpp

FORMS += \
	mainwindow.ui \
	StatusDialog.ui
	
RESOURCES += demo.qrc


LIBS += -L$${ADS_OUT_ROOT}/lib


INCLUDEPATH += ../src
DEPENDPATH += ../src

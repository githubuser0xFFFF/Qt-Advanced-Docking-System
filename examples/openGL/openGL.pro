ADS_OUT_ROOT = $${OUT_PWD}/../..

QT += core gui widgets charts opengl quick quickwidgets

TARGET = OpenGLExample
DESTDIR = $${ADS_OUT_ROOT}/lib
TEMPLATE = app
CONFIG += c++14
CONFIG += debug
CONFIG += qmltypes
QML_IMPORT_NAME = fbitem
QML_IMPORT_MAJOR_VERSION = 1

adsBuildStatic {
    DEFINES += ADS_STATIC
}

DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_DEBUG_PLUGINS

SOURCES += \
        glwidget.cpp \
        main.cpp \
        mainwindow.cpp \
        logo.cpp \
        glwindow.cpp \
        fbitem.cpp

HEADERS += \
        mainwindow.h \
        glwidget.h \
        logo.h \
        glwindow.h \
        fbitem.h

RESOURCES += openGL.qrc

OTHER_FILES += test.qml

LIBS += -L$${ADS_OUT_ROOT}/lib
include(../../ads.pri)
INCLUDEPATH += ../../src
DEPENDPATH += ../../src    


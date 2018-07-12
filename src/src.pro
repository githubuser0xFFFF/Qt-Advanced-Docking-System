ADS_ROOT = $${PWD}/..
ADS_OUT_ROOT = $${OUT_PWD}/..

TARGET = $$qtLibraryTarget(AdvancedDockingSystem)
TEMPLATE = lib
DESTDIR = $${ADS_OUT_ROOT}/lib
QT += core gui widgets

CONFIG += adsBuildShared


adsBuildShared {
	CONFIG += shared
        DEFINES += SHARED_EXPORT
}
!adsBuildShared {
	CONFIG += staticlib
}

windows {
	# MinGW
	*-g++* {
		QMAKE_CXXFLAGS += -std=c++11
		QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
	}
	# MSVC
	*-msvc* {
	}
}

RESOURCES += ads.qrc

HEADERS += \
    ads_globals.h \
    DockAreaWidget.h \
    DockContainerWidget.h \
    DockManager.h \
    DockWidget.h \
    DockWidgetTitleBar.h \
    FloatingDockContainer.h \
    DockOverlay.h \
    DockSplitter.h \
    shared_export.h
    
    
    
SOURCES += \
    ads_globals.cpp \
    DockAreaWidget.cpp \
    DockContainerWidget.cpp \
    DockManager.cpp \
    DockWidget.cpp \
    DockWidgetTitleBar.cpp \
    FloatingDockContainer.cpp \
    DockOverlay.cpp \
    DockSplitter.cpp

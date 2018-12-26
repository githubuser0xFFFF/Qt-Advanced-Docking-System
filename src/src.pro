ADS_ROOT = $${PWD}/..
ADS_OUT_ROOT = $${OUT_PWD}/..

TARGET = $$qtLibraryTarget(qtadvanceddocking)
TEMPLATE = lib
DESTDIR = $${ADS_OUT_ROOT}/lib
QT += core gui widgets

CONFIG += adsBuildShared


adsBuildShared {
	CONFIG += shared
    DEFINES += ADS_SHARED_EXPORT
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
    DockAreaTabBar.h \
    DockContainerWidget.h \
    DockManager.h \
    DockWidget.h \
    DockWidgetTab.h \
    FloatingDockContainer.h \
    DockOverlay.h \
    DockSplitter.h \
    DockAreaTitleBar.h \
    ElidingLabel.h
    
    
    
SOURCES += \
    ads_globals.cpp \
    DockAreaWidget.cpp \
    DockAreaTabBar.cpp \
    DockContainerWidget.cpp \
    DockManager.cpp \
    DockWidget.cpp \
    DockWidgetTab.cpp \
    FloatingDockContainer.cpp \
    DockOverlay.cpp \
    DockSplitter.cpp \
    DockAreaTitleBar.cpp \
    ElidingLabel.cpp

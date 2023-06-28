ADS_OUT_ROOT = $${OUT_PWD}/..
CONFIG += c++14
CONFIG += debug_and_release
TARGET = $$qtLibraryTarget(qtadvanceddocking)
DEFINES += QT_DEPRECATED_WARNINGS
TEMPLATE = lib
DESTDIR = $${ADS_OUT_ROOT}/lib
QT += core gui widgets

!adsBuildStatic {
	CONFIG += shared
    DEFINES += ADS_SHARED_EXPORT
}
adsBuildStatic {
	CONFIG += staticlib
    DEFINES += ADS_STATIC
}

windows {
	# MinGW
	*-g++* {
		QMAKE_CXXFLAGS += -Wall -Wextra -pedantic
	}
	# MSVC
	*-msvc* {
                QMAKE_CXXFLAGS += /utf-8
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
    DockingStateReader.h \
    FloatingDockContainer.h \
    FloatingDragPreview.h \
    DockOverlay.h \
    DockSplitter.h \
    DockAreaTitleBar_p.h \
    DockAreaTitleBar.h \
    ElidingLabel.h \
    IconProvider.h \
    DockComponentsFactory.h  \
    DockFocusController.h \
    AutoHideDockContainer.h \
    AutoHideSideBar.h \
    AutoHideTab.h \
    PushButton.h \
    ResizeHandle.h


SOURCES += \
    ads_globals.cpp \
    DockAreaWidget.cpp \
    DockAreaTabBar.cpp \
    DockContainerWidget.cpp \
    DockManager.cpp \
    DockWidget.cpp \
    DockingStateReader.cpp \
    DockWidgetTab.cpp \
    FloatingDockContainer.cpp \
    FloatingDragPreview.cpp \
    DockOverlay.cpp \
    DockSplitter.cpp \
    DockAreaTitleBar.cpp \
    ElidingLabel.cpp \
    IconProvider.cpp \
    DockComponentsFactory.cpp \
    DockFocusController.cpp \
    AutoHideDockContainer.cpp \
    AutoHideSideBar.cpp \
    AutoHideTab.cpp \
    PushButton.cpp \
    ResizeHandle.cpp


unix:!macx {
HEADERS += linux/FloatingWidgetTitleBar.h
SOURCES += linux/FloatingWidgetTitleBar.cpp
LIBS += -lxcb
QT += gui-private
}

isEmpty(PREFIX){
	PREFIX=../installed
	warning("Install Prefix not set")
}
headers.path=$$PREFIX/include
headers.files=$$HEADERS
target.path=$$PREFIX/lib
INSTALLS += headers target

DISTFILES +=

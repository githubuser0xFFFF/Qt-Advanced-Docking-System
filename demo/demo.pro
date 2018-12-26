ADS_ROOT = $${PWD}/..
ADS_OUT_ROOT = $${OUT_PWD}/..

TARGET = AdvancedDockingSystemDemo
DESTDIR = $${ADS_OUT_ROOT}/lib
QT += core gui widgets
CONFIG *= c++14


SOURCES += \
	main.cpp \
	MainWindow.cpp


HEADERS += \
	MainWindow.h

FORMS += \
	mainwindow.ui
	
RESOURCES += main.qrc

LIBS += -L$${ADS_OUT_ROOT}/lib

# Dependency: AdvancedDockingSystem (shared)
win32:CONFIG(release, debug|release): LIBS += -lqtadvanceddocking
else:win32:CONFIG(debug, debug|release): LIBS += -lqtadvanceddockingd
else:unix: LIBS += -lqtadvanceddocking

INCLUDEPATH += ../src
DEPENDPATH += ../src

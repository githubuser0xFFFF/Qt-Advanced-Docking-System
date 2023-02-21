
CONFIG(debug, debug|release){
    win32-g++ {
    	versionAtLeast(QT_VERSION, 5.15.0) {
    		LIBS += -lqtadvanceddocking
    	}
    	else {
    		LIBS += -lqtadvanceddockingd
    	}
    }
    else:msvc {
        LIBS += -lqtadvanceddockingd
    }
    else:mac {
        LIBS += -lqtadvanceddocking_debug
    }
    else {
        LIBS += -lqtadvanceddocking
    }
}
else{
    LIBS += -lqtadvanceddocking
}


unix:!macx {
    LIBS += -lxcb
}

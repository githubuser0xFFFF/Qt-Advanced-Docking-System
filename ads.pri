
lessThan(QT_MAJOR_VERSION, 6) {
    CONFIG(debug, debug|release){
        win32 {
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
}
else {
    # qt$$qtLibraryTarget(qtadvanceddocking) does not produce an advanceddockingd.dll file on Windows
    # for Qt6 - I don't know if this is a bug and I have to investigate
    LIBS += -lqtadvanceddocking
}


unix:!macx {
    LIBS += -lxcb
    QT += x11extras
}

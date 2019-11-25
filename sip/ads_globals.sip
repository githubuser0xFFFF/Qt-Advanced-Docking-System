%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{
    %TypeHeaderCode
    #include <ads_globals.h>
    %End

    enum DockWidgetArea
    {
        NoDockWidgetArea,
        LeftDockWidgetArea,
        RightDockWidgetArea,
        TopDockWidgetArea,
        BottomDockWidgetArea,
        CenterDockWidgetArea,

        InvalidDockWidgetArea,
        OuterDockAreas,
        AllDockAreas
    };
    typedef QFlags<ads::DockWidgetArea> DockWidgetAreas;


    enum TitleBarButton
    {
        TitleBarButtonTabsMenu,
        TitleBarButtonUndock,
        TitleBarButtonClose
    };

    enum eDragState
    {
        DraggingInactive,
        DraggingMousePressed,
        DraggingTab,
        DraggingFloatingWidget
    };

};

%End
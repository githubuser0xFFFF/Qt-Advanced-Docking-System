%Module(name=PyQtAds.QtAds.ads, call_super_init=True, keyword_arguments="Optional", use_limited_api=True)
%Import QtCore/QtCoremod.sip
%DefaultSupertype sip.simplewrapper
%Platforms {Linux macOS Windows}

%Include ads_globals.sip
%Include DockWidget.sip
%Include DockAreaTabBar.sip
%Include DockAreaTitleBar.sip
%Include DockAreaWidget.sip
%Include DockContainerWidget.sip
%Include DockManager.sip
%Include DockOverlay.sip
%Include DockSplitter.sip
%Include DockWidgetTab.sip
%Include ElidingLabel.sip
%Include FloatingDockContainer.sip
%If (Linux)
    %Include linux/FloatingWidgetTitleBar.sip
%End
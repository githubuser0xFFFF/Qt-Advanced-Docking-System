%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CAutoHideTab : CPushButton
{
    
    %TypeHeaderCode
    #include <AutoHideTab.h>
    %End

protected:
	void setSideBar(ads::CAutoHideSideBar *SideTabBar);
	void removeFromSideBar();
    virtual bool event(QEvent* event);

public:
	CAutoHideTab(QWidget* parent /TransferThis/ = 0);
	virtual ~CAutoHideTab();
	void updateStyle();
	ads::SideBarLocation sideBarLocation() const;
	void setOrientation(Qt::Orientation Orientation);
	Qt::Orientation orientation() const;
	bool isActiveTab() const;
	ads::CDockWidget* dockWidget() const;
	void setDockWidget(ads::CDockWidget* DockWidget);
	bool iconOnly() const;
	ads::CAutoHideSideBar* sideBar() const;
};

};

%End

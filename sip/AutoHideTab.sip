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
    virtual void contextMenuEvent(QContextMenuEvent* ev);
	virtual void mousePressEvent(QMouseEvent* ev);
	virtual void mouseReleaseEvent(QMouseEvent* ev);
	virtual void mouseMoveEvent(QMouseEvent* ev);

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
    int tabIndex() const;
    void setDockWidgetFloating();
    void unpinDockWidget();
    void requestCloseDockWidget();
};

};

%End

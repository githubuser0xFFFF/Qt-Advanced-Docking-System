%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CAutoHideDockContainer : QFrame
{

    %TypeHeaderCode
    #include <AutoHideDockContainer.h>
    %End

protected:
	virtual bool eventFilter(QObject* watched, QEvent* event);
	virtual void resizeEvent(QResizeEvent* event);
	virtual void leaveEvent(QEvent *event);
	virtual bool event(QEvent* event);
	void updateSize();
	void saveState(QXmlStreamWriter& Stream);

public:
	CAutoHideDockContainer(ads::CDockWidget* DockWidget /Transfer/, ads::SideBarLocation area,
		ads::CDockContainerWidget* parent /TransferThis/);
	virtual ~CAutoHideDockContainer();
	ads::CAutoHideSideBar* autoHideSideBar() const;
	ads::CAutoHideTab* autoHideTab() const;
	ads::CDockWidget* dockWidget() const;
    int tabIndex() const;
	void addDockWidget(ads::CDockWidget* DockWidget /Transfer/);
	ads::SideBarLocation sideBarLocation() const;
	void setSideBarLocation(ads::SideBarLocation SideBarLocation);
	ads::CDockAreaWidget* dockAreaWidget() const;
	ads::CDockContainerWidget* dockContainer() const;
    void moveContentsToParent();
	void cleanupAndDelete();
	void toggleView(bool Enable);
	void collapseView(bool Enable);
	void toggleCollapseState();
	void setSize(int Size);
    void resetToInitialDockWidgetSize();
    Qt::Orientation orientation() const;
    void moveToNewSideBarLocation(ads::SideBarLocation);
};

};

%End
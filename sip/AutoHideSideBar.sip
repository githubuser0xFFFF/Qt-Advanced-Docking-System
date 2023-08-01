%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{


class CAutoHideSideBar : QScrollArea
{

    %TypeHeaderCode
    #include <AutoHideSideBar.h>
    %End

protected:
	virtual bool eventFilter(QObject *watched, QEvent *event);
	void saveState(QXmlStreamWriter& Stream) const;
	void insertTab(int Index, ads::CAutoHideTab* SideTab /Transfer/);

public:
    CAutoHideSideBar(ads::CDockContainerWidget* parent /TransferThis/, SideBarLocation area);
	virtual ~CAutoHideSideBar();
	void removeTab(ads::CAutoHideTab* SideTab) /TransferBack/;
	ads::CAutoHideDockContainer* insertDockWidget(int Index, ads::CDockWidget* DockWidget /Transfer/);
	void removeAutoHideWidget(ads::CAutoHideDockContainer* AutoHideWidget) /TransferBack/;
	void addAutoHideWidget(ads::CAutoHideDockContainer* AutoHideWidget);
	Qt::Orientation orientation() const;
	ads::CAutoHideTab* tabAt(int index) const;
	int tabCount() const;
    int visibleTabCount() const;
    bool hasVisibleTabs() const;
	ads::SideBarLocation sideBarLocation() const;
	virtual QSize minimumSizeHint() const;
	virtual QSize sizeHint() const;
	int spacing() const;
	void setSpacing(int Spacing);
	CDockContainerWidget* dockContainer() const;
};

};

%End
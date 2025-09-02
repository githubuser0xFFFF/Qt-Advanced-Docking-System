%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

/**
 * Container that manages a number of dock areas with single dock widgets
 * or tabyfied dock widgets in each area.
 * Each window that support docking has a DockContainerWidget. That means
 * the main application window and all floating windows contain a
 * DockContainerWidget.
 */
class CDockContainerWidget : QFrame
{
    %TypeHeaderCode
    #include <DockContainerWidget.h>
    %End

protected:
	virtual bool event(QEvent *e);
	ads::CDockSplitter* rootSplitter() const;
    ads::CAutoHideDockContainer* createAndSetupAutoHideContainer(ads::SideBarLocation area, ads::CDockWidget* DockWidget /Transfer/, int TabIndex = -1);
	void createRootSplitter();
	void dropFloatingWidget(ads::CFloatingDockContainer* FloatingWidget, const QPoint& TargetPos);
    void dropWidget(QWidget* Widget, DockWidgetArea DropArea, CDockAreaWidget* TargetAreaWidget, int TabIndex = -1);
	void addDockArea(ads::CDockAreaWidget* DockAreaWidget /Transfer/, ads::DockWidgetArea area = ads::CenterDockWidgetArea);
	void removeDockArea(ads::CDockAreaWidget* area /TransferBack/);
    /*QList<QPointer<ads::CDockAreaWidget>> removeAllDockAreas();*/
	void saveState(QXmlStreamWriter& Stream) const;
	bool restoreState(CDockingStateReader& Stream, bool Testing);
	ads::CDockAreaWidget* lastAddedDockAreaWidget(ads::DockWidgetArea area) const;
	ads::CDockWidget* topLevelDockWidget() const;
	ads::CDockAreaWidget* topLevelDockArea() const;
    QList<ads::CDockWidget*> dockWidgets() const;
    void updateSplitterHandles(QSplitter* splitter);
    void registerAutoHideWidget(ads::CAutoHideDockContainer* AutoHideWidget /Transfer/);
    void removeAutoHideWidget(ads::CAutoHideDockContainer* AutoHideWidget /TransferBack/);
    void handleAutoHideWidgetEvent(QEvent* e, QWidget* w);
    
public:
	CDockContainerWidget(ads::CDockManager* DockManager /TransferThis/, QWidget* parent /TransferThis/ = 0);
	virtual ~CDockContainerWidget();
	ads::CDockAreaWidget* addDockWidget(ads::DockWidgetArea area, ads::CDockWidget* Dockwidget /Transfer/,
    ads::CDockAreaWidget* DockAreaWidget /Transfer/ = 0,
		int Index = -1);
	void removeDockWidget(ads::CDockWidget* Dockwidget) /TransferBack/;
	virtual unsigned int zOrderIndex() const;
	bool isInFrontOf(ads::CDockContainerWidget* Other) const;
	ads::CDockAreaWidget* dockAreaAt(const QPoint& GlobalPos) const;
	ads::CDockAreaWidget* dockArea(int Index) const;
	QList<ads::CDockAreaWidget*> openedDockAreas() const;
    QList<ads::CDockWidget*> openedDockWidgets() const;
	bool hasTopLevelDockWidget() const;
	int dockAreaCount() const;
	int visibleDockAreaCount() const;
	bool isFloating() const;
	void dumpLayout();
	ads::CDockWidget::DockWidgetFeatures features() const;
	ads::CFloatingDockContainer* floatingWidget() const;
	void closeOtherAreas(ads::CDockAreaWidget* KeepOpenArea);
    ads::CAutoHideSideBar* autoHideSideBar(SideBarLocation area) const;
	QList<ads::CAutoHideDockContainer*> autoHideWidgets() const;
	QRect contentRect() const;
	QRect contentRectGlobal() const;
	ads::CDockManager* dockManager() const;

signals:
	void dockAreasAdded();
    void autoHideWidgetCreated(ads::CAutoHideDockContainer* AutoHideWidget);
	void dockAreasRemoved();
	void dockAreaViewToggled(ads::CDockAreaWidget* DockArea, bool Open);
};

};

%End

%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CDockAreaWidget : QFrame
{

    %TypeHeaderCode
    #include <DockAreaWidget.h>
    %End
    
protected:
	void insertDockWidget(int index, ads::CDockWidget* DockWidget /Transfer/, bool Activate = true);
	void addDockWidget(ads::CDockWidget* DockWidget /Transfer/);
	void removeDockWidget(ads::CDockWidget* DockWidget) /TransferBack/;
	void toggleDockWidgetView(ads::CDockWidget* DockWidget, bool Open);
	CDockWidget* nextOpenDockWidget(ads::CDockWidget* DockWidget) const;
	int index(ads::CDockWidget* DockWidget);
	void hideAreaWithNoVisibleContent();
	void updateTitleBarVisibility();
	void internalSetCurrentDockWidget(ads::CDockWidget* DockWidget /Transfer/);
	void markTitleBarMenuOutdated();
    void updateTitleBarButtonVisibility(bool IsTopLevel) const;

protected slots:
	void toggleView(bool Open);
    
public:
    enum eDockAreaFlag
	{
		HideSingleWidgetTitleBar,
		DefaultFlags
	};
    typedef QFlags<ads::CDockAreaWidget::eDockAreaFlag> DockAreaFlags;
    
	CDockAreaWidget(ads::CDockManager* DockManager /TransferThis/, ads::CDockContainerWidget* parent /TransferThis/);
	virtual ~CDockAreaWidget();
	ads::CDockManager* dockManager() const;
	ads::CDockContainerWidget* dockContainer() const;
    ads::CAutoHideDockContainer* autoHideDockContainer() const;
    ads::CDockSplitter* parentSplitter() const;
    bool isAutoHide() const;
    void setAutoHideDockContainer(CAutoHideDockContainer*);
	virtual QSize minimumSizeHint() const;
	QRect titleBarGeometry() const;
	QRect contentAreaGeometry() const;
	int dockWidgetsCount() const;
	QList<ads::CDockWidget*> dockWidgets() const;
	int openDockWidgetsCount() const;
	QList<ads::CDockWidget*> openedDockWidgets() const;
	ads::CDockWidget* dockWidget(int Index) const;
	int currentIndex() const;
	int indexOfFirstOpenDockWidget() const;
	ads::CDockWidget* currentDockWidget() const;
	void setCurrentDockWidget(ads::CDockWidget* DockWidget);
	void saveState(QXmlStreamWriter& Stream) const;
    static bool restoreState(ads::CDockingStateReader& Stream, ads::CDockAreaWidget*& CreatedWidget,
		bool Testing, ads::CDockContainerWidget* ParentContainer);
 	ads::CDockWidget::DockWidgetFeatures features(ads::eBitwiseOperator Mode = ads::BitwiseAnd) const;
	QAbstractButton* titleBarButton(ads::TitleBarButton which) const;
	virtual void setVisible(bool Visible);

	void setAllowedAreas(DockWidgetAreas areas);
	DockWidgetAreas allowedAreas() const;
	CDockAreaTitleBar* titleBar() const;
    
    DockAreaFlags dockAreaFlags() const;
    void setDockAreaFlags(DockAreaFlags Flags);
    void setDockAreaFlag(eDockAreaFlag Flag, bool On);
    
    bool isCentralWidgetArea() const;
    bool containsCentralWidget() const;
    bool isTopLevelArea() const;

public slots:
	void setCurrentIndex(int index);
	void closeArea();
    void setAutoHide(bool Enable, SideBarLocation Location = ads::SideBarNone, int TabIndex = -1);
    void toggleAutoHide(SideBarLocation Location = ads::SideBarNone);
	void closeOtherAreas();
    void setFloating();

signals:
	void tabBarClicked(int index);
    void currentChanging(int index);
	void currentChanged(int index);
	void viewToggled(bool Open);
};

};

%End

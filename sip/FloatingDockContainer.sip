%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CFloatingDockContainer : QWidget
{
    
    %TypeHeaderCode
    #include <FloatingDockContainer.h>
    %End
    
protected:
	void startFloating(const QPoint& DragStartMousePos, const QSize& Size,
        ads::eDragState DragState, QWidget* MouseEventHandler);
    void startDragging(const QPoint& DragStartMousePos, const QSize& Size,
        QWidget* MouseEventHandler);
	void finishDragging();
	void initFloatingGeometry(const QPoint& DragStartMousePos, const QSize& Size);
	void moveFloating();
	bool restoreState(QXmlStreamReader& Stream, bool Testing);
	void updateWindowTitle();


protected:
	virtual void changeEvent(QEvent *event);
	virtual void moveEvent(QMoveEvent *event);
	virtual bool event(QEvent *e);
	virtual void closeEvent(QCloseEvent *event);
	virtual void hideEvent(QHideEvent *event);
	virtual void showEvent(QShowEvent *event);
	virtual bool eventFilter(QObject *watched, QEvent *event);

public:
	CFloatingDockContainer(ads::CDockManager* DockManager /TransferThis/);
	CFloatingDockContainer(ads::CDockAreaWidget* DockArea /TransferThis/);
	CFloatingDockContainer(ads::CDockWidget* DockWidget /TransferThis/);
	virtual ~CFloatingDockContainer();
	ads::CDockContainerWidget* dockContainer() const;
    bool isClosable() const;
    bool hasTopLevelDockWidget() const;
    ads::CDockWidget* topLevelDockWidget() const;
    QList<ads::CDockWidget*> dockWidgets() const;
};

};

%End

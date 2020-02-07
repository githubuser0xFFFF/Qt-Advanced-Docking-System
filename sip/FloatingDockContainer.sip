%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class IFloatingWidget
{
    %TypeHeaderCode
    #include <FloatingDockContainer.h>
    %End

public:
	virtual void startFloating(const QPoint& DragStartMousePos, const QSize& Size,
        ads::eDragState DragState, QWidget* MouseEventHandler) = 0;

	virtual void moveFloating() = 0;
	virtual void finishDragging() = 0;
};


class CFloatingDockContainer : QWidget, ads::IFloatingWidget
{
    
    %TypeHeaderCode
    #include <FloatingDockContainer.h>
    %End
    
protected:
	virtual void startFloating(const QPoint& DragStartMousePos, const QSize& Size,
        ads::eDragState DragState, QWidget* MouseEventHandler);
    void startDragging(const QPoint& DragStartMousePos, const QSize& Size,
        QWidget* MouseEventHandler);
	virtual void finishDragging();
	void initFloatingGeometry(const QPoint& DragStartMousePos, const QSize& Size);
	void moveFloating();
	bool restoreState(ads::CDockingStateReader& Stream, bool Testing);
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

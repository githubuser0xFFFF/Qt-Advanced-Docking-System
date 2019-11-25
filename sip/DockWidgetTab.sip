%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CDockWidgetTab : QFrame
{
    %TypeHeaderCode
    #include <DockWidgetTab.h>
    %End
    
protected:
	virtual void mousePressEvent(QMouseEvent* ev);
	virtual void mouseReleaseEvent(QMouseEvent* ev);
	virtual void mouseMoveEvent(QMouseEvent* ev);
	virtual void contextMenuEvent(QContextMenuEvent* ev);
	virtual void mouseDoubleClickEvent(QMouseEvent *event);

public:
	CDockWidgetTab(ads::CDockWidget* DockWidget /TransferThis/, QWidget* parent /TransferThis/ = 0);
	virtual ~CDockWidgetTab();
	bool isActiveTab() const;
	void setActiveTab(bool active);
	ads::CDockWidget* dockWidget() const;
	void setDockAreaWidget(ads::CDockAreaWidget* DockArea /Transfer/);
	ads::CDockAreaWidget* dockAreaWidget() const;
	void setIcon(const QIcon& Icon);
	const QIcon& icon() const;
	QString text() const;
	void setText(const QString& title);
	bool isClosable() const;
	virtual bool event(QEvent *e);


public slots:
	virtual void setVisible(bool visible);

signals:
	void activeTabChanged();
	void clicked();
	void closeRequested();
	void closeOtherTabsRequested();
	void moved(const QPoint& GlobalPos);
}; // class DockWidgetTab
};
 // namespace ads

%End

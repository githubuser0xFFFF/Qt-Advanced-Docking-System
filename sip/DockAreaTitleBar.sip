%Import QtWidgets/QtWidgetsmod.sip

%If (Qt_5_0_0 -)

namespace ads
{

class CDockAreaTitleBar : QFrame
{
    %TypeHeaderCode
    #include <DockAreaTitleBar.h>
    %End

public slots:
	void markTabsMenuOutdated();


public:
	CDockAreaTitleBar(ads::CDockAreaWidget* parent /TransferThis/);
	virtual ~CDockAreaTitleBar();
	ads::CDockAreaTabBar* tabBar() const;
	QAbstractButton* button(ads::TitleBarButton which) const;
	virtual void setVisible(bool Visible);


signals:
	void tabBarClicked(int index);
};

};

%End

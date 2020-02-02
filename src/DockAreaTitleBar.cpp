/*******************************************************************************
** Qt Advanced Docking System
** Copyright (C) 2017 Uwe Kindler
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


//============================================================================
/// \file   DockAreaTitleBar.cpp
/// \author Uwe Kindler
/// \date   12.10.2018
/// \brief  Implementation of CDockAreaTitleBar class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockAreaTitleBar.h"

#include <QPushButton>
#include <QToolButton>
#include <QBoxLayout>
#include <QStyle>
#include <QMenu>
#include <QScrollArea>
#include <QMouseEvent>
#include <QDebug>
#include <QPointer>

#include "ads_globals.h"
#include "FloatingDockContainer.h"
#include "DockAreaWidget.h"
#include "DockOverlay.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "DockWidgetTab.h"
#include "DockAreaTabBar.h"
#include "IconProvider.h"

#include <iostream>

namespace ads
{
using tTitleBarButton = QToolButton;

/**
 * Some kind of dummy button that is used if certain buttons are hidden
 * by dock manager config flags (i.e CDockManager::DockAreaHasCloseButton is
 * disabled)
 */
class CInvisibleButton : public tTitleBarButton
{
public:
	CInvisibleButton(QWidget* parent = nullptr)
		: tTitleBarButton(parent)
	{
		this->hide();
	}


    virtual void setVisible(bool visible) override
    {
    	Q_UNUSED(visible);
        tTitleBarButton::setVisible(false);
    }
};


/**
 * Private data class of CDockAreaTitleBar class (pimpl)
 */
struct DockAreaTitleBarPrivate
{
	CDockAreaTitleBar* _this;
	QPointer<tTitleBarButton> TabsMenuButton;
	QPointer<tTitleBarButton> UndockButton;
	QPointer<tTitleBarButton> CloseButton;
  QPointer<tTitleBarButton> WidgetActionsMenuButton;
	QBoxLayout* TopLayout;
	CDockAreaWidget* DockArea;
	CDockAreaTabBar* TabBar;
	bool MenuOutdated = true;

	/**
	 * Private data constructor
	 */
	DockAreaTitleBarPrivate(CDockAreaTitleBar* _public);

	/**
	 * Creates the title bar close and menu buttons
	 */
	void createButtons();

	/**
	 * Creates the internal TabBar
	 */
	void createTabBar();

	/**
	 * Convenience function for DockManager access
	 */
	CDockManager* dockManager() const
	{
		return DockArea->dockManager();
	}

	/**
	 * Returns true if the given config flag is set
	 */
	bool testConfigFlag(CDockManager::eConfigFlag Flag) const
	{
		return CDockManager::configFlags().testFlag(Flag);
	}

    /**
     * Helper function to set title bar button icons depending on operating
     * system and to avoid duplicated code. On windows the standard icons
     * are blurry since Qt 5.11 so we need to do some additional steps.
     * If the global IconPovider of the dockmanager provides a custom
     * Icon for the given CustomIconId, the this icon will be used.
     */
    void setTitleBarButtonIcon(tTitleBarButton* Button, QStyle::StandardPixmap StandarPixmap,
    	ads::eIcon CustomIconId)
    {
    	// First we try to use custom icons if available
    	QIcon Icon = CDockManager::iconProvider().customIcon(CustomIconId);
    	if (!Icon.isNull())
    	{
    		Button->setIcon(Icon);
    		return;
    	}

    #ifdef Q_OS_LINUX
        Button->setIcon(_this->style()->standardIcon(StandarPixmap));
    #else
		QPixmap normalPixmap = _this->style()->standardPixmap(StandardPixmap, 0, Button);
        Icon.addPixmap(internal::createTransparentPixmap(normalPixmap, 0.25), QIcon::Disabled);
        Icon.addPixmap(normalPixmap, QIcon::Normal);
        Button->setIcon(Icon);
    #endif
	}

	void createWidgetActionsButton();

};// struct DockAreaTitleBarPrivate



//============================================================================
DockAreaTitleBarPrivate::DockAreaTitleBarPrivate(CDockAreaTitleBar* _public) :
	_this(_public)
{

}


//============================================================================
void DockAreaTitleBarPrivate::createButtons()
{
	QSizePolicy ButtonSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

    if (testConfigFlag(CDockManager::DockAreaHasTabsMenuButton))
	{
		// Tabs menu button
		TabsMenuButton = new tTitleBarButton();
		TabsMenuButton->setObjectName("tabsMenuButton");
		TabsMenuButton->setAutoRaise(true);
		TabsMenuButton->setPopupMode(QToolButton::InstantPopup);
	    setTitleBarButtonIcon(TabsMenuButton, QStyle::SP_TitleBarUnshadeButton, ads::DockAreaMenuIcon);
		QMenu* TabsMenu = new QMenu(TabsMenuButton);
#ifndef QT_NO_TOOLTIP
		TabsMenu->setToolTipsVisible(true);
#endif
		_this->connect(TabsMenu, SIGNAL(aboutToShow()), SLOT(onTabsMenuAboutToShow()));
		TabsMenuButton->setMenu(TabsMenu);
#ifndef QT_NO_TOOLTIP
		TabsMenuButton->setToolTip(QObject::tr("List all tabs"));
#endif
		TabsMenuButton->setSizePolicy(ButtonSizePolicy);
		TopLayout->addWidget(TabsMenuButton, 0);
		_this->connect(TabsMenuButton->menu(), SIGNAL(triggered(QAction*)),
			SLOT(onTabsMenuActionTriggered(QAction*)));
	}
	else
	{
		TabsMenuButton = new CInvisibleButton();
	}

	if (testConfigFlag(CDockManager::DockAreaHasWidgetActionsMenuButton))
	{
		createWidgetActionsButton();
	}

	if (testConfigFlag(CDockManager::DockAreaHasUndockButton))
	{
		// Undock button
		UndockButton = new tTitleBarButton();
		UndockButton->setObjectName("undockButton");
		UndockButton->setAutoRaise(true);
#ifndef QT_NO_TOOLTIP
		UndockButton->setToolTip(QObject::tr("Detach Group"));
#endif
		setTitleBarButtonIcon(UndockButton, QStyle::SP_TitleBarNormalButton, ads::DockAreaUndockIcon);
		UndockButton->setSizePolicy(ButtonSizePolicy);
		TopLayout->addWidget(UndockButton, 0);
		_this->connect(UndockButton, SIGNAL(clicked()), SLOT(onUndockButtonClicked()));
	}
	else
	{
		UndockButton = new CInvisibleButton();
	}

	if (testConfigFlag(CDockManager::DockAreaHasCloseButton))
	{
		// Close button
		CloseButton = new tTitleBarButton();
		CloseButton->setObjectName("closeButton");
		CloseButton->setAutoRaise(true);
		setTitleBarButtonIcon(CloseButton, QStyle::SP_TitleBarCloseButton, ads::DockAreaCloseIcon);
#ifndef QT_NO_TOOLTIP
		if (testConfigFlag(CDockManager::DockAreaCloseButtonClosesTab))
		{
			CloseButton->setToolTip(QObject::tr("Close Active Tab"));
		}
		else
		{
			CloseButton->setToolTip(QObject::tr("Close Group"));
		}
#endif
		CloseButton->setSizePolicy(ButtonSizePolicy);
		CloseButton->setIconSize(QSize(16, 16));
		TopLayout->addWidget(CloseButton, 0);
		_this->connect(CloseButton, SIGNAL(clicked()), SLOT(onCloseButtonClicked()));
	}
	else
	{
		CloseButton = new CInvisibleButton();
	}
}

void DockAreaTitleBarPrivate::createWidgetActionsButton()
{
	QSizePolicy ButtonSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);

	WidgetActionsMenuButton = new tTileBarButton();
	WidgetActionsMenuButton->setObjectName("widgetActionsMenuButton");
	WidgetActionsMenuButton->setAutoRaise(true);
	WidgetActionsMenuButton->setPopupMode(QToolButton::InstantPopup);

	// First we try to use custom icons if available
	QIcon Icon = CDockManager::iconProvider().customIcon(ads::eIcon::DockAreaWidgetActionsMenuIcon);
	if (Icon.isNull())
	{
		// use default menu icon
		QPixmap normalPixmap(":/ads/images/menu.svg");
		Icon.addPixmap(internal::createTransparentPixmap(normalPixmap, 0.25), QIcon::Disabled);
		Icon.addPixmap(normalPixmap, QIcon::Normal);
	}
	WidgetActionsMenuButton->setIcon(Icon);

	QMenu* WidgetActionsMenu = new QMenu(WidgetActionsMenuButton);
#ifndef QT_NO_TOOLTIP
	WidgetActionsMenu->setToolTipsVisible(true);
#endif
	_this->connect(WidgetActionsMenu, &QMenu::aboutToShow, _this, &CDockAreaTitleBar::onWidgetActionsMenuAboutToShow);
	WidgetActionsMenuButton->setMenu(WidgetActionsMenu);
#ifndef QT_NO_TOOLTIP
	WidgetActionsMenuButton->setToolTip(QObject::tr("Options"));
#endif
	WidgetActionsMenuButton->setSizePolicy(ButtonSizePolicy);
	TopLayout->addWidget(WidgetActionsMenuButton, 0);
}


//============================================================================
void DockAreaTitleBarPrivate::createTabBar()
{
	TabBar = new CDockAreaTabBar(DockArea);
	TopLayout->addWidget(TabBar);
	_this->connect(TabBar, SIGNAL(tabClosed(int)), SLOT(markTabsMenuOutdated()));
	_this->connect(TabBar, SIGNAL(tabOpened(int)), SLOT(markTabsMenuOutdated()));
	_this->connect(TabBar, SIGNAL(tabInserted(int)), SLOT(markTabsMenuOutdated()));
	_this->connect(TabBar, SIGNAL(removingTab(int)), SLOT(markTabsMenuOutdated()));
	_this->connect(TabBar, SIGNAL(tabMoved(int, int)), SLOT(markTabsMenuOutdated()));
	_this->connect(TabBar, SIGNAL(currentChanged(int)), SLOT(onCurrentTabChanged(int)));
	_this->connect(TabBar, SIGNAL(tabBarClicked(int)), SIGNAL(tabBarClicked(int)));

	TabBar->setContextMenuPolicy(Qt::CustomContextMenu);
	_this->connect(TabBar, SIGNAL(customContextMenuRequested(const QPoint&)),
		SLOT(showContextMenu(const QPoint&)));
}


//============================================================================
CDockAreaTitleBar::CDockAreaTitleBar(CDockAreaWidget* parent) :
	QFrame(parent),
	d(new DockAreaTitleBarPrivate(this))
{
	d->DockArea = parent;

	setObjectName("dockAreaTitleBar");
	d->TopLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	d->TopLayout->setContentsMargins(0, 0, 0, 0);
	d->TopLayout->setSpacing(0);
	setLayout(d->TopLayout);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

	d->createTabBar();
	d->createButtons();

}


//============================================================================
CDockAreaTitleBar::~CDockAreaTitleBar()
{
	if (!d->CloseButton.isNull())
	{
		delete d->CloseButton;
	}

	if (!d->TabsMenuButton.isNull())
	{
		delete d->TabsMenuButton;
	}

	if (!d->UndockButton.isNull())
	{
		delete d->UndockButton;
	}
	delete d;
}


//============================================================================
CDockAreaTabBar* CDockAreaTitleBar::tabBar() const
{
	return d->TabBar;
}


//============================================================================
void CDockAreaTitleBar::markTabsMenuOutdated()
{
	d->MenuOutdated = true;
}


//============================================================================
void CDockAreaTitleBar::onTabsMenuAboutToShow()
{
	if (!d->MenuOutdated)
	{
		return;
	}

	QMenu* menu = d->TabsMenuButton->menu();
	menu->clear();
	for (int i = 0; i < d->TabBar->count(); ++i)
	{
		if (!d->TabBar->isTabOpen(i))
		{
			continue;
		}
		auto Tab = d->TabBar->tab(i);
		QAction* Action = menu->addAction(Tab->icon(), Tab->text());
		#ifndef QT_NO_TOOLTIP
		Action->setToolTip(Tab->toolTip());
		#endif
		Action->setData(i);
	}

	d->MenuOutdated = false;
}

//============================================================================
void CDockAreaTitleBar::onWidgetActionsMenuAboutToShow()
{
	QMenu* menu = d->WidgetActionsMenuButton->menu();
	menu->clear();

	CDockWidgetTab* currentTab = d->TabBar->currentTab();
	if (!currentTab)
	{
		return;
	}

	CDockWidget* currentDockWidget = currentTab->dockWidget();
	if (!currentDockWidget)
	{
		return;
	}

	QWidget* currentContentWidget = currentDockWidget->widget();
	if (!currentContentWidget)
	{
		return;
	}

	menu->addActions(currentContentWidget->actions());

	if (!menu->actions().isEmpty()) {
		menu->addSeparator();
	}

	menu->addActions(currentTab->createContextMenuActions(menu));
}


//============================================================================
void CDockAreaTitleBar::onCloseButtonClicked()
{
    ADS_PRINT("CDockAreaTitleBar::onCloseButtonClicked");
	if (d->testConfigFlag(CDockManager::DockAreaCloseButtonClosesTab))
	{
		d->TabBar->closeTab(d->TabBar->currentIndex());
	}
	else
	{
		d->DockArea->closeArea();
	}
}


//============================================================================
void CDockAreaTitleBar::onUndockButtonClicked()
{
	if (d->DockArea->features().testFlag(CDockWidget::DockWidgetFloatable))
	{
		d->TabBar->makeAreaFloating(mapFromGlobal(QCursor::pos()), DraggingInactive);
	}
}


//============================================================================
void CDockAreaTitleBar::onTabsMenuActionTriggered(QAction* Action)
{
	int Index = Action->data().toInt();
	d->TabBar->setCurrentIndex(Index);
	emit tabBarClicked(Index);
}


//============================================================================
void CDockAreaTitleBar::onCurrentTabChanged(int Index)
{
	if (Index < 0)
	{
		return;
	}

	if (d->testConfigFlag(CDockManager::DockAreaCloseButtonClosesTab))
	{
		CDockWidget* DockWidget = d->TabBar->tab(Index)->dockWidget();
		d->CloseButton->setEnabled(DockWidget->features().testFlag(CDockWidget::DockWidgetClosable));
	}
}


//============================================================================
QAbstractButton* CDockAreaTitleBar::button(TitleBarButton which) const
{
	switch (which)
	{
	case TitleBarButtonTabsMenu: return d->TabsMenuButton;
	case TitleBarButtonUndock: return d->UndockButton;
	case TitleBarButtonClose: return d->CloseButton;
	default:
		return nullptr;
	}
}


//============================================================================
void CDockAreaTitleBar::setVisible(bool Visible)
{
	Super::setVisible(Visible);
	markTabsMenuOutdated();
}


//============================================================================
void CDockAreaTitleBar::showContextMenu(const QPoint& pos)
{
	if (d->TabBar->dragState() == DraggingFloatingWidget)
	{
		return;
	}

	QMenu Menu(this);
	auto Action = Menu.addAction(tr("Detach Area"), this, SLOT(onUndockButtonClicked()));
	Action->setEnabled(d->DockArea->features().testFlag(CDockWidget::DockWidgetFloatable));
	Menu.addSeparator();
	Action = Menu.addAction(tr("Close Area"), this, SLOT(onCloseButtonClicked()));
	Action->setEnabled(d->DockArea->features().testFlag(CDockWidget::DockWidgetClosable));
	Menu.addAction(tr("Close Other Areas"), d->DockArea, SLOT(closeOtherAreas()));
	Menu.exec(mapToGlobal(pos));
}


} // namespace ads

//---------------------------------------------------------------------------
// EOF DockAreaTitleBar.cpp

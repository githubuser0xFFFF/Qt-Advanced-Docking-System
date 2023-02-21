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
/// \file   AutoHideTab.cpp
/// \author Syarif Fakhri
/// \date   05.09.2022
/// \brief  Implementation of CAutoHideTab class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "AutoHideTab.h"

#include <QBoxLayout>
#include <QApplication>
#include <QElapsedTimer>

#include "AutoHideDockContainer.h"
#include "AutoHideSideBar.h"
#include "DockAreaWidget.h"
#include "DockManager.h"
#include "DockWidget.h"

namespace ads
{
/**
 * Private data class of CDockWidgetTab class (pimpl)
 */
struct AutoHideTabPrivate
{
    CAutoHideTab* _this;
    CDockWidget* DockWidget = nullptr;
    CAutoHideSideBar* SideBar = nullptr;
	Qt::Orientation Orientation{Qt::Vertical};
	QElapsedTimer TimeSinceHoverMousePress;

	/**
	 * Private data constructor
	 */
	AutoHideTabPrivate(CAutoHideTab* _public);

	/**
	 * Update the orientation, visibility and spacing based on the area of
	 * the side bar
	 */
	void updateOrientation();

	/**
	 * Convenience function to ease dock container access
	 */
	CDockContainerWidget* dockContainer() const
	{
		return DockWidget ? DockWidget->dockContainer() : nullptr;
	}

	/**
	 * Forward this event to the dock container
	 */
	void forwardEventToDockContainer(QEvent* event)
	{
		auto DockContainer = dockContainer();
		if (DockContainer)
		{
			DockContainer->handleAutoHideWidgetEvent(event, _this);
		}
	}
}; // struct DockWidgetTabPrivate


//============================================================================
AutoHideTabPrivate::AutoHideTabPrivate(CAutoHideTab* _public) :
	_this(_public)
{

}


//============================================================================
void AutoHideTabPrivate::updateOrientation()
{
	bool IconOnly = CDockManager::testAutoHideConfigFlag(CDockManager::AutoHideSideBarsIconOnly);
	if (IconOnly && !_this->icon().isNull())
	{
		_this->setText("");
		_this->setOrientation(Qt::Horizontal);
	}
	else
	{
		auto area = SideBar->sideBarLocation();
		_this->setOrientation((area == SideBarBottom || area == SideBarTop) ? Qt::Horizontal : Qt::Vertical);
	}
}


//============================================================================
void CAutoHideTab::setSideBar(CAutoHideSideBar* SideTabBar)
{
	d->SideBar = SideTabBar;
	if (d->SideBar)
	{
		d->updateOrientation();
	}
}


//============================================================================
CAutoHideSideBar* CAutoHideTab::sideBar() const
{
	return d->SideBar;
}


//============================================================================
void CAutoHideTab::removeFromSideBar()
{
	if (d->SideBar == nullptr)
	{
		return;
	}
	d->SideBar->removeTab(this);
    setSideBar(nullptr);
}

//============================================================================
CAutoHideTab::CAutoHideTab(QWidget* parent) :
	CPushButton(parent),
	d(new AutoHideTabPrivate(this))
{
	setAttribute(Qt::WA_NoMousePropagation);
	setFocusPolicy(Qt::NoFocus);
}


//============================================================================
CAutoHideTab::~CAutoHideTab()
{
	ADS_PRINT("~CDockWidgetSideTab()");
	delete d;
}


//============================================================================
void CAutoHideTab::updateStyle()
{
    internal::repolishStyle(this, internal::RepolishDirectChildren);
	update();
}


//============================================================================
SideBarLocation CAutoHideTab::sideBarLocation() const
{
    if (d->SideBar)
	{
        return d->SideBar->sideBarLocation();
	}

	return SideBarLeft;
}


//============================================================================
void CAutoHideTab::setOrientation(Qt::Orientation Orientation)
{
	d->Orientation = Orientation;
	if (orientation() == Qt::Horizontal)
	{
		setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
	}
	else
	{
		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
	}
	CPushButton::setButtonOrientation((Qt::Horizontal == Orientation)
		? CPushButton::Horizontal : CPushButton::VerticalTopToBottom);
	updateStyle();
}


//============================================================================
Qt::Orientation CAutoHideTab::orientation() const
{
	return d->Orientation;
}


//============================================================================
bool CAutoHideTab::isActiveTab() const
{
	if (d->DockWidget && d->DockWidget->autoHideDockContainer())
	{
		return d->DockWidget->autoHideDockContainer()->isVisible();
	}

	return false;
}


//============================================================================
CDockWidget* CAutoHideTab::dockWidget() const
{
	return d->DockWidget;
}


//============================================================================
void CAutoHideTab::setDockWidget(CDockWidget* DockWidget)
{
	if (!DockWidget)
	{
		return;
	}
	d->DockWidget = DockWidget;
	setText(DockWidget->windowTitle());
	setIcon(d->DockWidget->icon());
	setToolTip(DockWidget->windowTitle());
}


//============================================================================
bool CAutoHideTab::event(QEvent* event)
{
	if (!CDockManager::testAutoHideConfigFlag(CDockManager::AutoHideShowOnMouseOver))
	{
		return Super::event(event);
	}

	switch (event->type())
	{
	case QEvent::Enter:
	case QEvent::Leave:
		 d->forwardEventToDockContainer(event);
		 break;

	case QEvent::MouseButtonPress:
		 // If AutoHideShowOnMouseOver is active, then the showing is triggered
		 // by a MousePressEvent sent to this tab. To prevent accidental hiding
		 // of the tab by a mouse click, we wait at least 500 ms before we accept
		 // the mouse click
		 if (!event->spontaneous())
		 {
			 d->TimeSinceHoverMousePress.restart();
			 d->forwardEventToDockContainer(event);
		 }
		 else if (d->TimeSinceHoverMousePress.hasExpired(500))
		 {
			 d->forwardEventToDockContainer(event);
		 }
		 break;

	default:
		break;
	}
	return Super::event(event);
}

//============================================================================
bool CAutoHideTab::iconOnly() const
{
	return CDockManager::testAutoHideConfigFlag(CDockManager::AutoHideSideBarsIconOnly) && !icon().isNull();
}

}

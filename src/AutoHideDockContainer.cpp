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
/// \file   AutoHideDockContainer.cpp
/// \author Syarif Fakhri
/// \date   05.09.2022
/// \brief  Implementation of CAutoHideDockContainer class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "AutoHideDockContainer.h"

#include <QXmlStreamWriter>
#include <QBoxLayout>
#include <QPainter>
#include <QSplitter>
#include <QPointer>
#include <QApplication>

#include "DockManager.h"
#include "DockWidgetTab.h"
#include "DockAreaWidget.h"
#include "DockingStateReader.h"
#include "ResizeHandle.h"
#include "DockComponentsFactory.h"
#include "AutoHideSideBar.h"
#include "AutoHideTab.h"


#include <iostream>

namespace ads
{
static const int ResizeMargin = 30;

//============================================================================
bool static isHorizontalArea(SideBarLocation Area)
{
	switch (Area)
	{
	case SideBarLocation::Top:
	case SideBarLocation::Bottom: return true;
	case SideBarLocation::Left:
	case SideBarLocation::Right: return false;
	}

	return true;
}


//============================================================================
Qt::Edge static edgeFromSideTabBarArea(SideBarLocation Area)
{
	switch (Area)
	{
	case SideBarLocation::Top: return Qt::BottomEdge;
	case SideBarLocation::Bottom: return Qt::TopEdge;
	case SideBarLocation::Left: return Qt::RightEdge;
	case SideBarLocation::Right: return Qt::LeftEdge;
	}

	return Qt::LeftEdge;
}


//============================================================================
int resizeHandleLayoutPosition(SideBarLocation Area)
{
	switch (Area)
	{
	case SideBarLocation::Bottom:
	case SideBarLocation::Right: return 0;

	case SideBarLocation::Top:
	case SideBarLocation::Left: return 1;
	}

	return 0;
}


/**
 * Private data of CAutoHideDockContainer - pimpl
 */
struct AutoHideDockContainerPrivate
{
    CAutoHideDockContainer* _this;
	CDockAreaWidget* DockArea{nullptr};
	CDockWidget* DockWidget{nullptr};
	SideBarLocation SideTabBarArea;
	QBoxLayout* Layout;
	CResizeHandle* ResizeHandle = nullptr;
	QSize Size; // creates invalid size
	QPointer<CAutoHideTab> SideTab;

	/**
	 * Private data constructor
	 */
	AutoHideDockContainerPrivate(CAutoHideDockContainer *_public);

	/**
	 * Convenience function to get a dock widget area
	 */
	DockWidgetArea getDockWidgetArea(SideBarLocation area)
	{
        switch (area)
        {
            case SideBarLocation::Left: return LeftDockWidgetArea;
            case SideBarLocation::Right: return RightDockWidgetArea;
            case SideBarLocation::Bottom: return BottomDockWidgetArea;
            case SideBarLocation::Top: return TopDockWidgetArea;
        }

		return LeftDockWidgetArea;
	}

	/**
	 * Update the resize limit of the resize handle
	 */
	void updateResizeHandleSizeLimitMax()
	{
		auto Rect = _this->parentContainer()->contentRect();
		const auto maxResizeHandleSize = ResizeHandle->orientation() == Qt::Horizontal
			? Rect.width() : Rect.height();
		ResizeHandle->setMaxResizeSize(maxResizeHandleSize - ResizeMargin);
	}

	/**
	 * Convenience function to check, if this is an horizontal area
	 */
	bool isHorizontal() const
	{
		return isHorizontalArea(SideTabBarArea);
	}

}; // struct AutoHideDockContainerPrivate


//============================================================================
AutoHideDockContainerPrivate::AutoHideDockContainerPrivate(
    CAutoHideDockContainer *_public) :
	_this(_public)
{

}


//============================================================================
CDockContainerWidget* CAutoHideDockContainer::parentContainer() const
{
	if (d->DockArea)
	{
		return d->DockArea->dockContainer();
	}
	else
	{
		return internal::findParent<CDockContainerWidget*>(this);
	}
}


//============================================================================
CAutoHideDockContainer::CAutoHideDockContainer(CDockManager* DockManager, SideBarLocation area, CDockContainerWidget* parent) :
    Super(parent),
    d(new AutoHideDockContainerPrivate(this))
{
	hide(); // auto hide dock container is initially always hidden
	d->SideTabBarArea = area;
	d->SideTab = componentsFactory()->createDockWidgetSideTab(nullptr);
	connect(d->SideTab, &CAutoHideTab::pressed, this, &CAutoHideDockContainer::toggleCollapseState);
	d->DockArea = new CDockAreaWidget(DockManager, parent);
	d->DockArea->setObjectName("autoHideDockArea");
	d->DockArea->setAutoHideDockContainer(this);
	d->DockArea->updateAutoHideButtonCheckState();
	d->DockArea->updateTitleBarButtonToolTip();

	setObjectName("autoHideDockContainer");

	d->Layout = new QBoxLayout(isHorizontalArea(area) ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
	d->Layout->setContentsMargins(0, 0, 0, 0);
	d->Layout->setSpacing(0);
	setLayout(d->Layout);
	d->Layout->addWidget(d->DockArea);
	d->ResizeHandle = new CResizeHandle(edgeFromSideTabBarArea(area), this);
	d->ResizeHandle->setMinResizeSize(64);
	bool OpaqueResize = CDockManager::testConfigFlag(CDockManager::OpaqueSplitterResize);
	d->ResizeHandle->setOpaqueResize(OpaqueResize);
	d->Layout->insertWidget(resizeHandleLayoutPosition(area), d->ResizeHandle);
	d->Size = d->DockArea->size();

	updateSize();
	parent->registerAutoHideWidget(this);
}


//============================================================================
CAutoHideDockContainer::CAutoHideDockContainer(CDockWidget* DockWidget, SideBarLocation area, CDockContainerWidget* parent) :
	CAutoHideDockContainer(DockWidget->dockManager(), area, parent)
{
	addDockWidget(DockWidget);
	hide();
}


//============================================================================
void CAutoHideDockContainer::updateSize()
{
	auto dockContainerParent = parentContainer();
	auto rect = dockContainerParent->contentRect();

	switch (sideBarLocation())
	{
	case SideBarLocation::Top:
		 resize(rect.width(), qMin(rect.height(), d->Size.height() - ResizeMargin));
		 move(rect.topLeft());
		 break;

	case SideBarLocation::Left:
		 resize(qMin(d->Size.width(), rect.width() - ResizeMargin), rect.height());
		 move(rect.topLeft());
		 break;

	case SideBarLocation::Right:
		 {
			 resize(qMin(d->Size.width(), rect.width() - ResizeMargin), rect.height());
			 QPoint p = rect.topRight();
			 p.rx() -= (width() - 1);
			 move(p);
		 }
		 break;

	case SideBarLocation::Bottom:
		 {
			 resize(rect.width(), qMin(rect.height(), d->Size.height() - ResizeMargin));
			 QPoint p = rect.bottomLeft();
			 p.ry() -= (height() - 1);
			 move(p);
		 }
		 break;
	}
}

//============================================================================
CAutoHideDockContainer::~CAutoHideDockContainer()
{
	ADS_PRINT("~CAutoHideDockContainer");

	// Remove event filter in case there are any queued messages
	qApp->removeEventFilter(this);
	if (parentContainer())
	{
		parentContainer()->removeAutoHideWidget(this);
	}

	if (d->SideTab)
	{
		delete d->SideTab;
	}

	delete d;
}

//============================================================================
CAutoHideSideBar* CAutoHideDockContainer::sideBar() const
{
	return parentContainer()->sideTabBar(d->SideTabBarArea);
}


//============================================================================
CAutoHideTab* CAutoHideDockContainer::autoHideTab() const
{
	return d->SideTab;
}


//============================================================================
CDockWidget* CAutoHideDockContainer::dockWidget() const
{
	return d->DockWidget;
}

//============================================================================
void CAutoHideDockContainer::addDockWidget(CDockWidget* DockWidget)
{
	if (d->DockWidget)
	{
		// Remove the old dock widget at this area
        d->DockArea->removeDockWidget(d->DockWidget);
	}

	d->DockWidget = DockWidget;
	d->SideTab->setDockWidget(DockWidget);
    CDockAreaWidget* OldDockArea = DockWidget->dockAreaWidget();
    if (OldDockArea)
    {
        OldDockArea->removeDockWidget(DockWidget);
    }
	d->DockArea->addDockWidget(DockWidget);

	// Prevent overriding of d->Size parameter when this function is called during
	// state restoring
	if (!DockWidget->dockManager()->isRestoringState() && OldDockArea)
	{
		// The initial size should be a little bit bigger than the original dock
		// area size to prevent that the resize handle of this auto hid dock area
		// is near of the splitter of the old dock area.
		d->Size = OldDockArea->size() + QSize(16, 16);
	}

	updateSize();
}


//============================================================================
SideBarLocation CAutoHideDockContainer::sideBarLocation() const
{
	return d->SideTabBarArea;
}

//============================================================================
CDockAreaWidget* CAutoHideDockContainer::dockAreaWidget() const
{
	return d->DockArea;
}

//============================================================================
void CAutoHideDockContainer::moveContentsToParent()
{
	cleanupAndDelete();
	// If we unpin the auto hide dock widget, then we insert it into the same
	// location like it had as a auto hide widget.  This brings the least surprise
	// to the user and he does not have to search where the widget was inserted.
	d->DockWidget->setDockArea(nullptr);
	parentContainer()->addDockWidget(d->getDockWidgetArea(d->SideTabBarArea), d->DockWidget);
}


//============================================================================
void CAutoHideDockContainer::cleanupAndDelete()
{
	const auto dockWidget = d->DockWidget;
	if (dockWidget)
	{

		auto SideTab = d->SideTab;
        SideTab->removeFromSideBar();
        SideTab->setParent(nullptr);
        SideTab->hide();
	}

	hide();
	deleteLater();
}


//============================================================================
void CAutoHideDockContainer::saveState(QXmlStreamWriter& s)
{
	s.writeStartElement("Widget");
	s.writeAttribute("Name", d->DockWidget->objectName());
	s.writeAttribute("Closed", QString::number(d->DockWidget->isClosed() ? 1 : 0));
    s.writeAttribute("Size", QString::number(d->isHorizontal() ? d->Size.height() : d->Size.width()));
	s.writeEndElement();
}


//============================================================================
void CAutoHideDockContainer::toggleView(bool Enable)
{
	if (Enable)
	{
        if (d->SideTab)
        {
            d->SideTab->show();
        }
	}
	else
	{
        if (d->SideTab)
        {
            d->SideTab->hide();
        }
        hide();
        qApp->removeEventFilter(this);
	}
}


//============================================================================
void CAutoHideDockContainer::collapseView(bool Enable)
{
	if (Enable)
	{
		hide();
		qApp->removeEventFilter(this);
	}
	else
	{
		updateSize();
		d->updateResizeHandleSizeLimitMax();
		raise();
		show();
		d->DockWidget->dockManager()->setDockWidgetFocused(d->DockWidget);
		qApp->installEventFilter(this);
	}

	ADS_PRINT("CAutoHideDockContainer::collapseView " << Enable);
    d->SideTab->updateStyle();
}


//============================================================================
void CAutoHideDockContainer::toggleCollapseState()
{
	collapseView(isVisible());
}


//============================================================================
void CAutoHideDockContainer::setSize(int Size)
{
	if (d->isHorizontal())
	{
		d->Size.setHeight(Size);
	}
	else
	{
		d->Size.setWidth(Size);
	}

	updateSize();
}


//============================================================================
bool CAutoHideDockContainer::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::Resize)
	{
		if (!d->ResizeHandle->isResizing())
		{
			updateSize();
		}
	}
	else if (event->type() == QEvent::MouseButtonPress)
	{
		auto Container = parentContainer();
		// First we check, if the mouse button press is inside the container
		// widget. If it is not, i.e. if someone resizes the main window or
		// clicks into the application menu or toolbar, then we ignore the
		// event
		auto widget = qobject_cast<QWidget*>(watched);
		bool IsContainer = false;
		while (widget)
		{
			if (widget == Container)
			{
				IsContainer = true;
			}
			widget = widget->parentWidget();
		}

		if (!IsContainer)
		{
			return Super::eventFilter(watched, event);
		}

		// Now we check, if the user clicked inside of this auto hide container.
		// If the click is inside of this auto hide container, then we can also
		// ignore the event, because the auto hide overlay should not get collapsed if
		// user works in it
		QMouseEvent* me = static_cast<QMouseEvent*>(event);
		auto pos = mapFromGlobal(me->globalPos());
		if (rect().contains(pos))
		{
			return Super::eventFilter(watched, event);
		}

		// Now check, if the user clicked into the side tab and ignore this event,
		// because the side tab click handler will call collapseView(). If we
		// do not ignore this here, then we will collapse the container and the side tab
		// click handler will uncollapse it
		auto SideTab = d->SideTab;
		pos = SideTab->mapFromGlobal(me->globalPos());
		if (SideTab->rect().contains(pos))
		{
			return Super::eventFilter(watched, event);
		}

		// If the mouse button down event is in the dock manager but outside
		// of the open auto hide container, then the auto hide dock widget
		// should get collapsed
		collapseView(true);
	}

	return Super::eventFilter(watched, event);
}


//============================================================================
void CAutoHideDockContainer::resizeEvent(QResizeEvent* event)
{
    Super::resizeEvent(event);
	if (d->ResizeHandle->isResizing())
	{
        d->Size = this->size();
		d->updateResizeHandleSizeLimitMax();
	}
}

}


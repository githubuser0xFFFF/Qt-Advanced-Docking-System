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
/// \file   AutoHideSideBar.cpp
/// \author Syarif Fakhri
/// \date   05.09.2022
/// \brief  Implementation of CAutoHideSideBar class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "AutoHideSideBar.h"

#include <QBoxLayout>
#include <QStyleOption>
#include <QPainter>
#include <QXmlStreamWriter>

#include "DockContainerWidget.h"
#include "DockWidgetTab.h"
#include "DockFocusController.h"
#include "AutoHideDockContainer.h"
#include "DockAreaWidget.h"
#include "DockingStateReader.h"
#include "AutoHideTab.h"

namespace ads
{
/**
 * Private data class of CSideTabBar class (pimpl)
 */
struct AutoHideSideBarPrivate
{
	/**
	 * Private data constructor
	 */
	AutoHideSideBarPrivate(CAutoHideSideBar* _public);

    CAutoHideSideBar* _this;
    CDockContainerWidget* ContainerWidget;
    QBoxLayout* TabsLayout;
    Qt::Orientation Orientation;
    SideBarLocation SideTabArea = SideBarLocation::Left;

    /**
     * Convenience function to check if this is a horizontal side bar
     */
    bool isHorizontal() const
    {
    	return Qt::Horizontal == Orientation;
    }
}; // struct AutoHideSideBarPrivate

//============================================================================
AutoHideSideBarPrivate::AutoHideSideBarPrivate(CAutoHideSideBar* _public) :
    _this(_public)
{
}


//============================================================================
CAutoHideSideBar::CAutoHideSideBar(CDockContainerWidget* parent, SideBarLocation area) :
    Super(parent),
    d(new AutoHideSideBarPrivate(this))
{
	d->SideTabArea = area;
    d->ContainerWidget = parent;
    d->Orientation = (area == SideBarLocation::Bottom || area == SideBarLocation::Top)
    	? Qt::Horizontal : Qt::Vertical;

    auto mainLayout = new QBoxLayout(d->Orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);

    d->TabsLayout = new QBoxLayout(d->Orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
    d->TabsLayout->setContentsMargins(0, 0, 0, 0);
    d->TabsLayout->setSpacing(0);
    mainLayout->addLayout(d->TabsLayout);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addStretch(1);
    setLayout(mainLayout);

    setFocusPolicy(Qt::NoFocus);
	if (d->isHorizontal())
	{
		setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
	}
	else
	{
		setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
	}

	hide();
}


//============================================================================
CAutoHideSideBar::~CAutoHideSideBar() 
{
	qDebug() << "~CSideTabBar() ";
	// The SideTabeBar is not the owner of the tabs and to prevent deletion
	// we set the parent here to nullptr to remove it from the children
	auto Tabs = findChildren<CAutoHideTab*>(QString(), Qt::FindDirectChildrenOnly);
	for (auto Tab : Tabs)
	{
		Tab->setParent(nullptr);
	}
	delete d;
}


//============================================================================
void CAutoHideSideBar::insertTab(int Index, CAutoHideTab* SideTab)
{
	SideTab->installEventFilter(this);
    SideTab->setSideBar(this);
    d->TabsLayout->insertWidget(Index, SideTab);
    show();
}


//============================================================================
CAutoHideDockContainer* CAutoHideSideBar::insertDockWidget(int Index, CDockWidget* DockWidget)
{
	auto AutoHideContainer = new CAutoHideDockContainer(DockWidget, d->SideTabArea, d->ContainerWidget);
	DockWidget->dockManager()->dockFocusController()->clearDockWidgetFocus(DockWidget);
	auto Tab = AutoHideContainer->autoHideTab();
	insertTab(Index, Tab);
	return AutoHideContainer;
}


//============================================================================
void CAutoHideSideBar::removeTab(CAutoHideTab* SideTab)
{
	SideTab->removeEventFilter(this);
    d->TabsLayout->removeWidget(SideTab);
    if (d->TabsLayout->isEmpty())
    {
    	hide();
    }
}


//============================================================================
bool CAutoHideSideBar::event(QEvent* e)
{
	switch (e->type())
	{
	case QEvent::ChildRemoved:
		if (d->TabsLayout->isEmpty())
		{
			hide();
		}
		break;

	case QEvent::Resize:
		if (d->TabsLayout->count())
		{
			auto ev = static_cast<QResizeEvent*>(e);
			auto Tab = tabAt(0);
			int Size = d->isHorizontal() ? ev->size().height() : ev->size().width();
			int TabSize = d->isHorizontal() ? Tab->size().height() : Tab->size().width();
			// If the size of the side bar is less than the size of the first tab
			// then there are no visible tabs in this side bar. This check will
			// fail if someone will force a very big border via CSS!!
			if (Size < TabSize)
			{
				hide();
			}
		}
		else
		{
			hide();
		}
		break;

	default:
		break;
	}
	return Super::event(e);
}


//============================================================================
bool CAutoHideSideBar::eventFilter(QObject *watched, QEvent *event)
{
	if (event->type() != QEvent::ShowToParent)
	{
		return false;
	}

	// As soon as on tab is shhown, we need to show the side tab bar
	auto Tab = qobject_cast<CAutoHideTab*>(watched);
	if (Tab)
	{
		show();
	}
	return false;
}

//============================================================================
Qt::Orientation CAutoHideSideBar::orientation() const
{
    return d->Orientation;
}


//============================================================================
CAutoHideTab* CAutoHideSideBar::tabAt(int index) const
{
    return qobject_cast<CAutoHideTab*>(d->TabsLayout->itemAt(index)->widget());
}


//============================================================================
int CAutoHideSideBar::tabCount() const
{
    return d->TabsLayout->count();
}


//============================================================================
SideBarLocation CAutoHideSideBar::sideBarLocation() const
{
	return d->SideTabArea;
}


//============================================================================
void CAutoHideSideBar::saveState(QXmlStreamWriter& s) const
{
	if (!tabCount())
	{
		return;
	}

	s.writeStartElement("SideBar");
	s.writeAttribute("Area", QString::number(sideBarLocation()));
	s.writeAttribute("Tabs", QString::number(tabCount()));

	for (auto i = 0; i < tabCount(); ++i)
	{
		auto Tab = tabAt(i);
		if (!Tab)
		{
			continue;
		}

		Tab->dockWidget()->autoHideDockContainer()->saveState(s);
	}

	s.writeEndElement();
}


} // namespace ads

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
/// \file   DockAreaTabBar.cpp
/// \author Uwe Kindler
/// \date   24.08.2018
/// \brief  Implementation of CDockAreaTabBar class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockAreaTabBar.h"

#include <QMouseEvent>
#include <QScrollBar>
#include <QDebug>
#include <QBoxLayout>
#include <QApplication>

#include "FloatingDockContainer.h"
#include "DockAreaWidget.h"
#include "DockOverlay.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "DockWidgetTab.h"

#include <iostream>


namespace ads
{
/**
 * Private data class of CDockAreaTabBar class (pimpl)
 */
struct DockAreaTabBarPrivate
{
	CDockAreaTabBar* _this;
	QPoint DragStartMousePos;
	CDockAreaWidget* DockArea;
	CFloatingDockContainer* FloatingWidget = nullptr;
	QWidget* TabsContainerWidget;
	QBoxLayout* TabsLayout;
	int CurrentIndex = -1;

	/**
	 * Private data constructor
	 */
	DockAreaTabBarPrivate(CDockAreaTabBar* _public);

	/**
	 * Update tabs after current index changed or when tabs are removed.
	 * The function reassigns the stylesheet to update the tabs
	 */
	void updateTabs();
};
// struct DockAreaTabBarPrivate

//============================================================================
DockAreaTabBarPrivate::DockAreaTabBarPrivate(CDockAreaTabBar* _public) :
	_this(_public)
{

}


//============================================================================
void DockAreaTabBarPrivate::updateTabs()
{
	// Set active TAB and update all other tabs to be inactive
	for (int i = 0; i < _this->count(); ++i)
	{
		auto TabWidget = _this->tab(i);
		if (!TabWidget)
		{
			continue;
		}

		if (i == CurrentIndex)
		{
			TabWidget->show();
			TabWidget->setActiveTab(true);
			_this->ensureWidgetVisible(TabWidget);
		}
		else
		{
			TabWidget->setActiveTab(false);
		}
	}
}


//============================================================================
CDockAreaTabBar::CDockAreaTabBar(CDockAreaWidget* parent) :
	QScrollArea(parent),
	d(new DockAreaTabBarPrivate(this))
{
	d->DockArea = parent;
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
	setFrameStyle(QFrame::NoFrame);
	setWidgetResizable(true);
	setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	d->TabsContainerWidget = new QWidget();
	d->TabsContainerWidget->setObjectName("tabsContainerWidget");
	setWidget(d->TabsContainerWidget);

	d->TabsLayout = new QBoxLayout(QBoxLayout::LeftToRight);
	d->TabsLayout->setContentsMargins(0, 0, 0, 0);
	d->TabsLayout->setSpacing(0);
	d->TabsLayout->addStretch(1);
	d->TabsContainerWidget->setLayout(d->TabsLayout);
}

//============================================================================
CDockAreaTabBar::~CDockAreaTabBar()
{
	delete d;
}


//============================================================================
void CDockAreaTabBar::wheelEvent(QWheelEvent* Event)
{
	Event->accept();
	const int direction = Event->angleDelta().y();
	if (direction < 0)
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() + 20);
	}
	else
	{
		horizontalScrollBar()->setValue(horizontalScrollBar()->value() - 20);
	}
}


//============================================================================
void CDockAreaTabBar::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		ev->accept();
		d->DragStartMousePos = ev->pos();
		return;
	}
	QScrollArea::mousePressEvent(ev);
}


//============================================================================
void CDockAreaTabBar::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
        ADS_PRINT("CTabsScrollArea::mouseReleaseEvent");
		ev->accept();
		d->FloatingWidget = nullptr;
		d->DragStartMousePos = QPoint();
		return;
	}
	QScrollArea::mouseReleaseEvent(ev);
}


//============================================================================
void CDockAreaTabBar::mouseMoveEvent(QMouseEvent* ev)
{
	QScrollArea::mouseMoveEvent(ev);
	if (ev->buttons() != Qt::LeftButton)
	{
		return;
	}

	if (d->FloatingWidget)
	{
		d->FloatingWidget->moveFloating();
		return;
	}

	// If this is the last dock area in a dock container it does not make
	// sense to move it to a new floating widget and leave this one
	// empty
	if (d->DockArea->dockContainer()->isFloating()
	 && d->DockArea->dockContainer()->visibleDockAreaCount() == 1)
	{
		return;
	}

	// If one single dock widget in this area is not floatable then the whole
	// area is not floatable
	if (!d->DockArea->features().testFlag(CDockWidget::DockWidgetFloatable))
	{
		return;
	}

	int DragDistance = (d->DragStartMousePos - ev->pos()).manhattanLength();
	if (DragDistance >= CDockManager::startDragDistance())
	{
        ADS_PRINT("CTabsScrollArea::startFloating");
		startFloating(d->DragStartMousePos);
		auto Overlay = d->DockArea->dockManager()->containerOverlay();
		Overlay->setAllowedAreas(OuterDockAreas);
	}

	return;
}


//============================================================================
void CDockAreaTabBar::mouseDoubleClickEvent(QMouseEvent *event)
{
	// If this is the last dock area in a dock container it does not make
	// sense to move it to a new floating widget and leave this one
	// empty
	if (d->DockArea->dockContainer()->isFloating() && d->DockArea->dockContainer()->dockAreaCount() == 1)
	{
		return;
	}

	if (!d->DockArea->features().testFlag(CDockWidget::DockWidgetFloatable))
	{
		return;
	}
	makeAreaFloating(event->pos(), DraggingInactive);
}


//============================================================================
CFloatingDockContainer* CDockAreaTabBar::makeAreaFloating(const QPoint& Offset,
	eDragState DragState)
{
	QSize Size = d->DockArea->size();
	CFloatingDockContainer* FloatingWidget = new CFloatingDockContainer(d->DockArea);
    FloatingWidget->startFloating(Offset, Size, DragState, nullptr);
	auto TopLevelDockWidget = FloatingWidget->topLevelDockWidget();
	if (TopLevelDockWidget)
	{
		TopLevelDockWidget->emitTopLevelChanged(true);
	}

	return FloatingWidget;
}


//============================================================================
void CDockAreaTabBar::startFloating(const QPoint& Offset)
{
	d->FloatingWidget = makeAreaFloating(Offset, DraggingFloatingWidget);
}


//============================================================================
void CDockAreaTabBar::setCurrentIndex(int index)
{
	if (index == d->CurrentIndex)
	{
		return;
	}

	if (index < -1 || index > (count() - 1))
	{
		qWarning() << Q_FUNC_INFO << "Invalid index" << index;
		return;
    }

    emit currentChanging(index);
	d->CurrentIndex = index;
	d->updateTabs();
	emit currentChanged(index);
}


//============================================================================
int CDockAreaTabBar::count() const
{
	// The tab bar contains a stretch item as last item
	return d->TabsLayout->count() - 1;
}


//===========================================================================
void CDockAreaTabBar::insertTab(int Index, CDockWidgetTab* Tab)
{
	d->TabsLayout->insertWidget(Index, Tab);
	connect(Tab, SIGNAL(clicked()), this, SLOT(onTabClicked()));
	connect(Tab, SIGNAL(closeRequested()), this, SLOT(onTabCloseRequested()));
	connect(Tab, SIGNAL(closeOtherTabsRequested()), this, SLOT(onCloseOtherTabsRequested()));
	connect(Tab, SIGNAL(moved(const QPoint&)), this, SLOT(onTabWidgetMoved(const QPoint&)));
	Tab->installEventFilter(this);
	emit tabInserted(Index);
	if (Index <= d->CurrentIndex)
	{
		setCurrentIndex(d->CurrentIndex + 1);
	}
}


//===========================================================================
void CDockAreaTabBar::removeTab(CDockWidgetTab* Tab)
{
	if (!count())
	{
		return;
	}
    ADS_PRINT("CDockAreaTabBar::removeTab ");
	int NewCurrentIndex = currentIndex();
	int RemoveIndex = d->TabsLayout->indexOf(Tab);
	if (count() == 1)
	{
		NewCurrentIndex = -1;
	}
	if (NewCurrentIndex > RemoveIndex)
	{
		NewCurrentIndex--;
	}
	else if (NewCurrentIndex == RemoveIndex)
	{
		NewCurrentIndex = -1;
		// First we walk to the right to search for the next visible tab
		for (int i = (RemoveIndex + 1); i < count(); ++i)
		{
			if (tab(i)->isVisibleTo(this))
			{
				NewCurrentIndex = i - 1;
				break;
			}
		}

		// If there is no visible tab right to this tab then we walk to
		// the left to find a visible tab
		if (NewCurrentIndex < 0)
		{
			for (int i = (RemoveIndex - 1); i >= 0; --i)
			{
				if (tab(i)->isVisibleTo(this))
				{
					NewCurrentIndex = i;
					break;
				}
			}
		}
	}

	emit removingTab(RemoveIndex);
	d->TabsLayout->removeWidget(Tab);
	Tab->disconnect(this);
	Tab->removeEventFilter(this);
    ADS_PRINT("NewCurrentIndex " << NewCurrentIndex);
	if (NewCurrentIndex != d->CurrentIndex)
	{
		setCurrentIndex(NewCurrentIndex);
	}
	else
	{
		d->updateTabs();
	}
}


//===========================================================================
int CDockAreaTabBar::currentIndex() const
{
	return d->CurrentIndex;
}


//===========================================================================
CDockWidgetTab* CDockAreaTabBar::currentTab() const
{
	if (d->CurrentIndex < 0)
	{
		return nullptr;
	}
	else
	{
		return qobject_cast<CDockWidgetTab*>(d->TabsLayout->itemAt(d->CurrentIndex)->widget());
	}
}


//===========================================================================
void CDockAreaTabBar::onTabClicked()
{
	CDockWidgetTab* Tab = qobject_cast<CDockWidgetTab*>(sender());
	if (!Tab)
	{
		return;
	}

	int index = d->TabsLayout->indexOf(Tab);
	if (index < 0)
	{
		return;
	}
	setCurrentIndex(index);
 	emit tabBarClicked(index);
}


//===========================================================================
void CDockAreaTabBar::onTabCloseRequested()
{
	CDockWidgetTab* Tab = qobject_cast<CDockWidgetTab*>(sender());
	int Index = d->TabsLayout->indexOf(Tab);
	closeTab(Index);
}


//===========================================================================
void CDockAreaTabBar::onCloseOtherTabsRequested()
{
	auto Sender = qobject_cast<CDockWidgetTab*>(sender());
	for (int i = 0; i < count(); ++i)
	{
		auto Tab = tab(i);
		if (Tab->isClosable() && !Tab->isHidden() && Tab != Sender)
		{
			closeTab(i);
		}
	}
}


//===========================================================================
CDockWidgetTab* CDockAreaTabBar::tab(int Index) const
{
	if (Index >= count() || Index < 0)
	{
		return nullptr;
	}
	return qobject_cast<CDockWidgetTab*>(d->TabsLayout->itemAt(Index)->widget());
}


//===========================================================================
void CDockAreaTabBar::onTabWidgetMoved(const QPoint& GlobalPos)
{
	CDockWidgetTab* MovingTab = qobject_cast<CDockWidgetTab*>(sender());
	if (!MovingTab)
	{
		return;
	}

	int fromIndex = d->TabsLayout->indexOf(MovingTab);
	auto MousePos = mapFromGlobal(GlobalPos);
	int toIndex = -1;
	// Find tab under mouse
	for (int i = 0; i < count(); ++i)
	{
		CDockWidgetTab* DropTab = tab(i);
		if (DropTab == MovingTab || !DropTab->isVisibleTo(this)
		    || !DropTab->geometry().contains(MousePos))
		{
			continue;
		}

		toIndex = d->TabsLayout->indexOf(DropTab);
		if (toIndex == fromIndex)
		{
			toIndex = -1;
			continue;
		}

		if (toIndex < 0)
		{
			toIndex = 0;
		}
		break;
	}

	// Now check if the mouse is behind the last tab
	if (toIndex < 0)
	{
		if (MousePos.x() > tab(count() - 1)->geometry().right())
		{
            ADS_PRINT("after all tabs");
			toIndex = count() - 1;
		}
		else
		{
			toIndex = fromIndex;
		}
	}

	d->TabsLayout->removeWidget(MovingTab);
	d->TabsLayout->insertWidget(toIndex, MovingTab);
	if (toIndex >= 0)
	{
        ADS_PRINT("tabMoved from " << fromIndex << " to " << toIndex);
		emit tabMoved(fromIndex, toIndex);
		setCurrentIndex(toIndex);
	}
}


//===========================================================================
void CDockAreaTabBar::closeTab(int Index)
{
	if (Index < 0 || Index >= count())
	{
		return;
	}

	auto Tab = tab(Index);
	if (Tab->isHidden())
	{
		return;
	}
	emit tabCloseRequested(Index);
	Tab->hide();
}


//===========================================================================
bool CDockAreaTabBar::eventFilter(QObject *watched, QEvent *event)
{
	bool Result = Super::eventFilter(watched, event);
	CDockWidgetTab* Tab = qobject_cast<CDockWidgetTab*>(watched);
	if (!Tab)
	{
		return Result;
	}

	switch (event->type())
	{
	case QEvent::Hide:
		 emit tabClosed(d->TabsLayout->indexOf(Tab)); break;
	case QEvent::Show:
		 emit tabOpened(d->TabsLayout->indexOf(Tab)); break;
	default:
		break;
	}

	return Result;
}


//===========================================================================
bool CDockAreaTabBar::isTabOpen(int Index) const
{
	if (Index < 0 || Index >= count())
	{
		return false;
	}

	return !tab(Index)->isHidden();
}


//===========================================================================
QSize CDockAreaTabBar::minimumSizeHint() const
{
	QSize Size = sizeHint();
	Size.setWidth(Super::minimumSizeHint().width());// this defines the minimum width of a dock area
	return Size;
}

//===========================================================================
QSize CDockAreaTabBar::sizeHint() const
{
	QSize Size = Super::sizeHint();
	Size.setHeight(d->TabsContainerWidget->sizeHint().height());
	return Size;
}

} // namespace ads

//---------------------------------------------------------------------------
// EOF DockAreaTabBar.cpp

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
/// \file   FloatingDockContainer.cpp
/// \author Uwe Kindler
/// \date   01.03.2017
/// \brief  Implementation of CFloatingDockContainer class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "FloatingDockContainer.h"

#include <QBoxLayout>
#include <QApplication>
#include <QMouseEvent>
#include <QPointer>
#include <QAction>
#include <QDebug>
#include <QAbstractButton>
#include <QElapsedTimer>

#include "DockContainerWidget.h"
#include "DockAreaWidget.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "DockOverlay.h"

#include <iostream>

namespace ads
{
static unsigned int zOrderCounter = 0;
/**
 * Private data class of CFloatingDockContainer class (pimpl)
 */
struct FloatingDockContainerPrivate
{
	CFloatingDockContainer* _this;
	CDockContainerWidget* DockContainer;
	unsigned int zOrderIndex = ++zOrderCounter;
	QPointer<CDockManager> DockManager;
	eDragState DraggingState = DraggingInactive;
	QPoint DragStartMousePosition;
	CDockContainerWidget* DropContainer = nullptr;
	CDockAreaWidget* SingleDockArea = nullptr;

	/**
	 * Private data constructor
	 */
	FloatingDockContainerPrivate(CFloatingDockContainer* _public);

	void titleMouseReleaseEvent();
	void updateDropOverlays(const QPoint& GlobalPos);

	/**
	 * Tests is a certain state is active
	 */
	bool isState(eDragState StateId) const
	{
		return StateId == DraggingState;
	}

	void setState(eDragState StateId)
	{
		DraggingState = StateId;
	}
};
// struct FloatingDockContainerPrivate



//============================================================================
FloatingDockContainerPrivate::FloatingDockContainerPrivate(CFloatingDockContainer* _public) :
	_this(_public)
{

}



//============================================================================
void FloatingDockContainerPrivate::titleMouseReleaseEvent()
{
	setState(DraggingInactive);
	if (!DropContainer)
	{
		return;
	}

	if (DockManager->dockAreaOverlay()->dropAreaUnderCursor() != InvalidDockWidgetArea
	 || DockManager->containerOverlay()->dropAreaUnderCursor() != InvalidDockWidgetArea)
	{
		// Resize the floating widget to the size of the highlighted drop area
		// rectangle
		CDockOverlay* Overlay = DockManager->containerOverlay();
		if (!Overlay->dropOverlayRect().isValid())
		{
			Overlay = DockManager->dockAreaOverlay();
		}

		QRect Rect = Overlay->dropOverlayRect();
		int FrameWidth = (_this->frameSize().width() - _this->rect().width()) / 2;
		int TitleBarHeight = _this->frameSize().height() - _this->rect().height() - FrameWidth;
		if (Rect.isValid())
		{
			QPoint TopLeft = Overlay->mapToGlobal(Rect.topLeft());
			TopLeft.ry() += TitleBarHeight;
			_this->setGeometry(QRect(TopLeft, QSize(Rect.width(), Rect.height() - TitleBarHeight)));
			QApplication::processEvents();
		}
		DropContainer->dropFloatingWidget(_this, QCursor::pos());
	}

	DockManager->containerOverlay()->hideOverlay();
	DockManager->dockAreaOverlay()->hideOverlay();
}



//============================================================================
void FloatingDockContainerPrivate::updateDropOverlays(const QPoint& GlobalPos)
{
	if (!_this->isVisible() || !DockManager)
	{
		return;
	}

    auto Containers = DockManager->dockContainers();
    CDockContainerWidget* TopContainer = nullptr;
    for (auto ContainerWidget : Containers)
    {
    	if (!ContainerWidget->isVisible())
    	{
    		continue;
    	}

    	if (DockContainer == ContainerWidget)
    	{
    		continue;
    	}

    	QPoint MappedPos = ContainerWidget->mapFromGlobal(GlobalPos);
    	if (ContainerWidget->rect().contains(MappedPos))
    	{
    		if (!TopContainer || ContainerWidget->isInFrontOf(TopContainer))
    		{
    			TopContainer = ContainerWidget;
    		}
    	}
    }

    DropContainer = TopContainer;
    auto ContainerOverlay = DockManager->containerOverlay();
    auto DockAreaOverlay = DockManager->dockAreaOverlay();

    if (!TopContainer)
    {
    	ContainerOverlay->hideOverlay();
    	DockAreaOverlay->hideOverlay();
    	return;
    }

    int VisibleDockAreas = TopContainer->visibleDockAreaCount();
    ContainerOverlay->setAllowedAreas(VisibleDockAreas > 1 ?
    	OuterDockAreas : AllDockAreas);
	DockWidgetArea ContainerArea = ContainerOverlay->showOverlay(TopContainer);
	ContainerOverlay->enableDropPreview(ContainerArea != InvalidDockWidgetArea);
    auto DockArea = TopContainer->dockAreaAt(GlobalPos);
    if (DockArea && DockArea->isVisible() && VisibleDockAreas > 0)
    {
    	DockAreaOverlay->enableDropPreview(true);
    	DockAreaOverlay->setAllowedAreas((VisibleDockAreas == 1) ?
    		NoDockWidgetArea : AllDockAreas);
        DockWidgetArea Area = DockAreaOverlay->showOverlay(DockArea);

        // A CenterDockWidgetArea for the dockAreaOverlay() indicates that
        // the mouse is in the title bar. If the ContainerArea is valid
        // then we ignore the dock area of the dockAreaOverlay() and disable
        // the drop preview
        if ((Area == CenterDockWidgetArea) && (ContainerArea != InvalidDockWidgetArea))
        {
        	DockAreaOverlay->enableDropPreview(false);
        	ContainerOverlay->enableDropPreview(true);
        }
        else
        {
            ContainerOverlay->enableDropPreview(InvalidDockWidgetArea == Area);
        }
    }
    else
    {
    	DockAreaOverlay->hideOverlay();
    }
}


//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockManager* DockManager) :
	QWidget(DockManager, Qt::Window),
	d(new FloatingDockContainerPrivate(this))
{
	d->DockManager = DockManager;
    QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
    l->setContentsMargins(0, 0, 0, 0);
    l->setSpacing(0);
    setLayout(l);

    d->DockContainer = new CDockContainerWidget(DockManager, this);
    connect(d->DockContainer, SIGNAL(dockAreasAdded()), this, SLOT(onDockAreasAddedOrRemoved()));
    connect(d->DockContainer, SIGNAL(dockAreasRemoved()), this, SLOT(onDockAreasAddedOrRemoved()));
	l->addWidget(d->DockContainer);
	DockManager->registerFloatingWidget(this);

	// We install an event filter to detect mouse release events because we
	// do not receive mouse release event if the floating widget is behind
	// the drop overlay cross
	qApp->installEventFilter(this);
}


//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockAreaWidget* DockArea) :
	CFloatingDockContainer(DockArea->dockManager())
{
	d->DockContainer->addDockArea(DockArea);
}


//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockWidget* DockWidget) :
	CFloatingDockContainer(DockWidget->dockManager())
{
	d->DockContainer->addDockWidget(CenterDockWidgetArea, DockWidget);
}

//============================================================================
CFloatingDockContainer::~CFloatingDockContainer()
{
	qDebug() << "~CFloatingDockContainer";
	if (d->DockManager)
	{
		d->DockManager->removeFloatingWidget(this);
	}
	delete d;
}


//============================================================================
CDockContainerWidget* CFloatingDockContainer::dockContainer() const
{
	return d->DockContainer;
}


//============================================================================
void CFloatingDockContainer::changeEvent(QEvent *event)
{
	QWidget::changeEvent(event);
	if ((event->type() == QEvent::ActivationChange) && isActiveWindow())
    {
		qDebug() << "FloatingWidget::changeEvent QEvent::ActivationChange ";
		d->zOrderIndex = ++zOrderCounter;
        return;
    }
}


//============================================================================
void CFloatingDockContainer::moveEvent(QMoveEvent *event)
{
	QWidget::moveEvent(event);
	switch (d->DraggingState)
	{
	case DraggingMousePressed:
		 d->setState(DraggingFloatingWidget);
		 d->updateDropOverlays(QCursor::pos());
		 break;

	case DraggingFloatingWidget:
		 d->updateDropOverlays(QCursor::pos());
		 break;
	default:
		break;
	}
}


//============================================================================
void CFloatingDockContainer::closeEvent(QCloseEvent *event)
{
    qDebug() << "CFloatingDockContainer closeEvent";
	d->setState(DraggingInactive);

    if (isClosable())
    {
    	QWidget::closeEvent(event);
    }
    else
    {
        event->ignore();
    }
}


//============================================================================
void CFloatingDockContainer::hideEvent(QHideEvent *event)
{
	Super::hideEvent(event);
	for (auto DockArea : d->DockContainer->openedDockAreas())
	{
		for (auto DockWidget : DockArea->openedDockWidgets())
		{
			DockWidget->toggleView(false);
		}
	}
}


//============================================================================
void CFloatingDockContainer::showEvent(QShowEvent *event)
{
	Super::showEvent(event);
	/*for (auto DockArea : d->DockContainer->openedDockAreas())
	{
		for (auto DockWidget : DockArea->openedDockWidgets())
		{
			DockWidget->setToggleViewActionChecked(true);
		}
	}*/
}


//============================================================================
bool CFloatingDockContainer::event(QEvent *e)
{
	switch (d->DraggingState)
	{
	case DraggingInactive:
		if (e->type() == QEvent::NonClientAreaMouseButtonPress && QGuiApplication::mouseButtons() == Qt::LeftButton)
		{
			qDebug() << "FloatingWidget::event Event::NonClientAreaMouseButtonPress" << e->type();
			d->setState(DraggingMousePressed);
		}
	break;

	case DraggingMousePressed:
		switch (e->type())
		{
		case QEvent::NonClientAreaMouseButtonDblClick:
			 qDebug() << "FloatingWidget::event QEvent::NonClientAreaMouseButtonDblClick";
			 d->setState(DraggingInactive);
			 break;

		case QEvent::Resize:
			 // If the first event after the mouse press is a resize event, then
			 // the user resizes the window instead of dragging it around.
			 // But there is one exception. If the window is maximized,
		     // then dragging the window via title bar will cause the widget to
		     // leave the maximized state. This in turn will trigger a resize event.
		     // To know, if the resize event was triggered by user via moving a
		     // corner of the window frame or if it was caused by a windows state
		     // change, we check, if we are not in maximized state.
			 if (!isMaximized())
			 {
				 d->setState(DraggingInactive);
			 }
			 break;

		default:
			break;
		}
	break;

	case DraggingFloatingWidget:
		if (e->type() == QEvent::NonClientAreaMouseButtonRelease)
		{
			qDebug() << "FloatingWidget::event QEvent::NonClientAreaMouseButtonRelease";
			d->titleMouseReleaseEvent();
		}
	break;

	default:
		break;
	}

#if (ADS_DEBUG_LEVEL > 0)
	qDebug() << "CFloatingDockContainer::event " << e->type();
#endif
	return QWidget::event(e);
}


//============================================================================
bool CFloatingDockContainer::eventFilter(QObject *watched, QEvent *event)
{
	Q_UNUSED(watched);
	if (event->type() == QEvent::MouseButtonRelease && d->isState(DraggingFloatingWidget))
	{
		qDebug() << "FloatingWidget::eventFilter QEvent::MouseButtonRelease";
		d->titleMouseReleaseEvent();
	}

	return false;
}


//============================================================================
void CFloatingDockContainer::startFloating(const QPoint& DragStartMousePos, const QSize& Size,
	eDragState DragState)
{
	resize(Size);
	d->setState(DragState);
	d->DragStartMousePosition = DragStartMousePos;
	moveFloating();
    show();

}


//============================================================================
void CFloatingDockContainer::moveFloating()
{
	int BorderSize = (frameSize().width() - size().width()) / 2;
	const QPoint moveToPos = QCursor::pos() - d->DragStartMousePosition - QPoint(BorderSize, 0);
	move(moveToPos);
}


//============================================================================
bool CFloatingDockContainer::isClosable() const
{
    return d->DockContainer->features().testFlag(CDockWidget::DockWidgetClosable);
}


//============================================================================
void CFloatingDockContainer::onDockAreasAddedOrRemoved()
{
	qDebug() << "CFloatingDockContainer::onDockAreasAddedOrRemoved()";
	auto TopLevelDockArea = d->DockContainer->topLevelDockArea();
	if (TopLevelDockArea)
	{
		d->SingleDockArea = TopLevelDockArea;
		this->setWindowTitle(d->SingleDockArea->currentDockWidget()->windowTitle());
		connect(d->SingleDockArea, SIGNAL(currentChanged(int)), this,
			SLOT(onDockAreaCurrentChanged(int)));
	}
	else
	{
		if (d->SingleDockArea)
		{
			disconnect(d->SingleDockArea, SIGNAL(currentChanged(int)), this,
				SLOT(onDockAreaCurrentChanged(int)));
			d->SingleDockArea = nullptr;
		}
		this->setWindowTitle(qApp->applicationDisplayName());
	}
}


//============================================================================
void CFloatingDockContainer::updateWindowTitle()
{
	auto TopLevelDockArea = d->DockContainer->topLevelDockArea();
	if (TopLevelDockArea)
	{
		this->setWindowTitle(TopLevelDockArea->currentDockWidget()->windowTitle());
	}
	else
	{
		this->setWindowTitle(qApp->applicationDisplayName());
	}
}


//============================================================================
void CFloatingDockContainer::onDockAreaCurrentChanged(int Index)
{
	Q_UNUSED(Index);
	this->setWindowTitle(d->SingleDockArea->currentDockWidget()->windowTitle());
}


//============================================================================
bool CFloatingDockContainer::restoreState(QXmlStreamReader& Stream, bool Testing)
{
	if (!d->DockContainer->restoreState(Stream, Testing))
	{
		return false;
	}

	onDockAreasAddedOrRemoved();
	return true;
}


//============================================================================
bool CFloatingDockContainer::hasTopLevelDockWidget() const
{
	return d->DockContainer->hasTopLevelDockWidget();
}


//============================================================================
CDockWidget* CFloatingDockContainer::topLevelDockWidget() const
{
	return d->DockContainer->topLevelDockWidget();
}


//============================================================================
QList<CDockWidget*> CFloatingDockContainer::dockWidgets() const
{
	return d->DockContainer->dockWidgets();
}


} // namespace ads

//---------------------------------------------------------------------------
// EOF FloatingDockContainer.cpp

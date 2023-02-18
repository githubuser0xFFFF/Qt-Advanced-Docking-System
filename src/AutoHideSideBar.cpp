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
class CTabsWidget;

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
    CTabsWidget* TabsContainerWidget;
    QBoxLayout* TabsLayout;
    Qt::Orientation Orientation;
    SideBarLocation SideTabArea = SideBarLocation::SideBarLeft;
    CAutoHideTab* PlaceholderTab;

    /**
     * Convenience function to check if this is a horizontal side bar
     */
    bool isHorizontal() const
    {
      return Qt::Horizontal == Orientation;
    }

    /**
     * Called from viewport to forward event handling to this
     */
    void handleViewportEvent(QEvent* e);

    /**
    * Convenience function to access first tab
    */
    CAutoHideTab* firstTab() const {return _this->tab(0);}

    /**
    * Convenience function to access last tab
    */
    CAutoHideTab* lastTab() const {return _this->tab(_this->tabCount() - 1);}
}; // struct AutoHideSideBarPrivate


/**
 * This widget stores the tab buttons
 */
class CTabsWidget : public QWidget
{
public:
  using QWidget::QWidget;
  using Super = QWidget;
  AutoHideSideBarPrivate* EventHandler;

  /**
   * Returns the size hint as minimum size hint
   */
  virtual QSize minimumSizeHint() const override
  {
    return Super::sizeHint();
  }

  /**
   * Forward event handling to EventHandler
   */
  virtual bool event(QEvent* e) override
  {
    EventHandler->handleViewportEvent(e);
    return Super::event(e);
  }
};


//============================================================================
AutoHideSideBarPrivate::AutoHideSideBarPrivate(CAutoHideSideBar* _public) :
    _this(_public),
    PlaceholderTab(new CAutoHideTab(_this))
{
    PlaceholderTab->hide();
}


//============================================================================
void AutoHideSideBarPrivate::handleViewportEvent(QEvent* e)
{
  switch (e->type())
  {
  case QEvent::ChildRemoved:
    if (TabsLayout->isEmpty())
    {
      _this->hide();
    }
    break;

  case QEvent::Resize:
    if (_this->tabCount())
    {
      auto ev = static_cast<QResizeEvent*>(e);
      auto Tab = _this->tabAt(0);
      int Size = isHorizontal() ? ev->size().height() : ev->size().width();
      int TabSize = isHorizontal() ? Tab->size().height() : Tab->size().width();
      // If the size of the side bar is less than the size of the first tab
      // then there are no visible tabs in this side bar. This check will
      // fail if someone will force a very big border via CSS!!
      if (Size < TabSize)
      {
        _this->hide();
      }
    }
    else
    {
      _this->hide();
    }
    break;

  default:
    break;
  }
}


//============================================================================
CAutoHideSideBar::CAutoHideSideBar(CDockContainerWidget* parent, SideBarLocation area) :
    Super(parent),
    d(new AutoHideSideBarPrivate(this))
{
  d->SideTabArea = area;
  d->ContainerWidget = parent;
  d->Orientation = (area == SideBarLocation::SideBarBottom || area == SideBarLocation::SideBarTop)
  ? Qt::Horizontal : Qt::Vertical;

  setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  setFrameStyle(QFrame::NoFrame);
  setWidgetResizable(true);
  setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

  d->TabsContainerWidget = new CTabsWidget();
  d->TabsContainerWidget->EventHandler = d;
  d->TabsContainerWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  d->TabsContainerWidget->setObjectName("sideTabsContainerWidget");


  d->TabsLayout = new QBoxLayout(d->Orientation == Qt::Vertical ? QBoxLayout::TopToBottom : QBoxLayout::LeftToRight);
  d->TabsLayout->setContentsMargins(0, 0, 0, 0);
  d->TabsLayout->setSpacing(12);
  d->TabsLayout->addStretch(1);
  d->TabsContainerWidget->setLayout(d->TabsLayout);
  setWidget(d->TabsContainerWidget);

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
  ADS_PRINT("~CSideTabBar()");
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
    SideTab->setSideBar(this);
    SideTab->installEventFilter(this);
    connect(SideTab, SIGNAL(moved(QPoint)), this, SLOT(onAutoHideTabMoved(QPoint)));
    connect(SideTab, SIGNAL(moving(QPoint)), this, SLOT(onAutoHideTabMoving(QPoint)));
    if (Index < 0)
    {
      d->TabsLayout->insertWidget(d->TabsLayout->count() - 1, SideTab);
    }
    else
    {
      d->TabsLayout->insertWidget(Index, SideTab);
    }
    show();
}


//============================================================================
CAutoHideDockContainer* CAutoHideSideBar::insertDockWidget(int Index, CDockWidget* DockWidget)
{
  auto AutoHideContainer = new CAutoHideDockContainer(DockWidget, d->SideTabArea, d->ContainerWidget);
  DockWidget->dockManager()->dockFocusController()->clearDockWidgetFocus(DockWidget);
  auto Tab = AutoHideContainer->autoHideTab();
  DockWidget->setSideTabWidget(Tab);
  insertTab(Index, Tab);
  return AutoHideContainer;
}


//============================================================================
void CAutoHideSideBar::removeAutoHideWidget(CAutoHideDockContainer* AutoHideWidget)
{
  AutoHideWidget->autoHideTab()->removeFromSideBar();
  auto DockContainer = AutoHideWidget->dockContainer();
  if (DockContainer)
  {
    DockContainer->removeAutoHideWidget(AutoHideWidget);
  }
  AutoHideWidget->setParent(nullptr);
}

//============================================================================
void CAutoHideSideBar::addAutoHideWidget(CAutoHideDockContainer* AutoHideWidget)
{
  auto SideBar = AutoHideWidget->autoHideTab()->sideBar();
  if (SideBar == this)
  {
    return;
  }

  if (SideBar)
  {
    SideBar->removeAutoHideWidget(AutoHideWidget);
  }
  AutoHideWidget->setParent(d->ContainerWidget);
  AutoHideWidget->setSideBarLocation(d->SideTabArea);
  d->ContainerWidget->registerAutoHideWidget(AutoHideWidget);
  insertTab(-1, AutoHideWidget->autoHideTab());
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
bool CAutoHideSideBar::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() != QEvent::ShowToParent)
    {
        return false;
    }

    // As soon as on tab is shown, we need to show the side tab bar
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
    return d->TabsLayout->count() - 1;
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

//===========================================================================
QSize CAutoHideSideBar::minimumSizeHint() const
{
  QSize Size = sizeHint();
  Size.setWidth(10);
  return Size;
}


//===========================================================================
QSize CAutoHideSideBar::sizeHint() const
{
  return d->TabsContainerWidget->sizeHint();
}


//===========================================================================
int CAutoHideSideBar::spacing() const
{
  return d->TabsLayout->spacing();
}


//===========================================================================
void CAutoHideSideBar::setSpacing(int Spacing)
{
  d->TabsLayout->setSpacing(Spacing);
}


//===========================================================================
CDockContainerWidget* CAutoHideSideBar::dockContainer() const
{
  return d->ContainerWidget;
}


//===========================================================================
void CAutoHideSideBar::onAutoHideTabMoved(const QPoint& GlobalPos)
{
    auto MovingTab = qobject_cast<CAutoHideTab*>(sender());
    if (!MovingTab)
    {
        return;
    }

    QSizePolicy sp_retain = d->PlaceholderTab->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(false);
    d->PlaceholderTab->setSizePolicy(sp_retain);

    // Swap the placeholder and the moved tab
    const auto index = d->TabsLayout->indexOf(d->PlaceholderTab);
    d->TabsLayout->removeWidget(d->PlaceholderTab);
    d->TabsLayout->insertWidget(index, MovingTab);
}


//===========================================================================
void CAutoHideSideBar::onAutoHideTabMoving(const QPoint& GlobalPos)
{
    auto MovingTab = qobject_cast<CAutoHideTab*>(sender());
    if (!MovingTab)
    {
        return;
    }

    d->PlaceholderTab->setText(MovingTab->text());
    d->PlaceholderTab->setOrientation(MovingTab->orientation());
    QSizePolicy sp_retain = d->PlaceholderTab->sizePolicy();
    sp_retain.setRetainSizeWhenHidden(true);
    d->PlaceholderTab->setSizePolicy(sp_retain);
    d->PlaceholderTab->setGeometry(MovingTab->geometry());

    if (const auto index = d->TabsLayout->indexOf(MovingTab); index > -1)
    {
        // First time moving, set the placeholder tab into the moving tab position
        d->TabsLayout->removeWidget(MovingTab);
        d->TabsLayout->insertWidget(index, d->PlaceholderTab);
    }

    int fromIndex = d->TabsLayout->indexOf(d->PlaceholderTab);

    auto MousePos = mapFromGlobal(GlobalPos);
    MousePos.rx() = qMax(d->firstTab()->geometry().left(), MousePos.x());
    MousePos.rx() = qMin(d->lastTab()->geometry().right(), MousePos.x());
    MousePos.ry() = qMax(d->firstTab()->geometry().top(), MousePos.y());
    MousePos.ry() = qMin(d->lastTab()->geometry().bottom(), MousePos.y());

    int toIndex = -1;
    // Find tab under mouse
    for (int i = 0; i < tabCount(); ++i)
    {
        CAutoHideTab* DropTab = tab(i);
        if (DropTab == d->PlaceholderTab || !DropTab->isVisibleTo(this)
            || !DropTab->geometry().contains(MousePos)
            )
        {
            continue;
        }

        toIndex = d->TabsLayout->indexOf(DropTab);
        if (toIndex == fromIndex)
        {
            toIndex = -1;
        }
        break;
    }

    if (toIndex > -1)
    {
        d->TabsLayout->removeWidget(d->PlaceholderTab);
        d->TabsLayout->insertWidget(toIndex, d->PlaceholderTab);
    }
    else
    {
        // Ensure that the moved tab is reset to its start position
        d->TabsLayout->update();
    }
}


//===========================================================================
CAutoHideTab* CAutoHideSideBar::tab(int Index) const
{
	if (Index >= tabCount() || Index < 0)
	{
		return nullptr;
	}
	return qobject_cast<CAutoHideTab*>(d->TabsLayout->itemAt(Index)->widget());
}
} // namespace ads


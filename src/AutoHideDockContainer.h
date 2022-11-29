#ifndef AutoHideDockContainerH
#define AutoHideDockContainerH
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
/// \file   AutoHideDockContainer.h
/// \author Syarif Fakhri
/// \date   05.09.2022
/// \brief  Declaration of CAutoHideDockContainer class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "ads_globals.h"

#include <QSplitter>
#include "AutoHideTab.h"

class QXmlStreamWriter;

namespace ads
{
struct AutoHideDockContainerPrivate;
class CDockManager;
class CDockWidget;
class CDockContainerWidget;
class CAutoHideSideBar;
class CDockAreaWidget;
class CDockingStateReader;
struct SideTabBarPrivate;

/**
 * Auto hide container for hosting an auto hide dock widget
 */
class ADS_EXPORT CAutoHideDockContainer : public QFrame
{
	Q_OBJECT
    Q_PROPERTY(int sideBarLocation READ sideBarLocation)
private:
	AutoHideDockContainerPrivate* d; ///< private data (pimpl)
	friend struct AutoHideDockContainerPrivate;
	friend CAutoHideSideBar;
	friend SideTabBarPrivate;

protected:
	virtual bool eventFilter(QObject* watched, QEvent* event) override;
	virtual void resizeEvent(QResizeEvent* event) override;
	virtual void leaveEvent(QEvent *event) override;
	virtual bool event(QEvent* event) override;

	/**
	 * Updates the size considering the size limits and the resize margins
	 */
	void updateSize();

	/*
	 * Saves the state and size
	 */
	void saveState(QXmlStreamWriter& Stream);

public:
	using Super = QFrame;

    /**
	 * Create Auto Hide widget with the given dock widget
	 */
	CAutoHideDockContainer(CDockWidget* DockWidget, SideBarLocation area,
		CDockContainerWidget* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~CAutoHideDockContainer();

	/**
	 * Get's the side tab bar
	 */
	CAutoHideSideBar* sideBar() const;

	/**
	 * Returns the side tab
	 */
	CAutoHideTab* autoHideTab() const;

	/**
	 * Get's the dock widget in this dock container
	 */
	CDockWidget* dockWidget() const;

	/**
	 * Adds a dock widget and removes the previous dock widget
	 */
	void addDockWidget(CDockWidget* DockWidget);

    /**
	 * Returns the side tab bar area of this Auto Hide dock container
	 */
	SideBarLocation sideBarLocation() const;

	/**
	 * Sets a new SideBarLocation.
	 * If a new side bar location is set, the auto hide dock container needs
	 * to update its resize handle position
	 */
	void setSideBarLocation(SideBarLocation SideBarLocation);

	/**
	 * Returns the dock area widget of this Auto Hide dock container
	 */
	CDockAreaWidget* dockAreaWidget() const;

	/**
	 * Returns the parent container that hosts this auto hide container
	 */
	CDockContainerWidget* dockContainer() const;

	/**
	 * Moves the contents to the parent container widget
	 * Used before removing this Auto Hide dock container 
	 */
    void moveContentsToParent();

	/**
	 * Cleanups up the side tab widget and then deletes itself
	 */
	void cleanupAndDelete();

	/*
	 * Toggles the auto Hide dock container widget
	 * This will also hide the side tab widget
	 */
	void toggleView(bool Enable);

	/*
	 * Collapses the auto hide dock container widget
	 * Does not hide the side tab widget
	 */
	void collapseView(bool Enable);

	/**
	 * Toggles the current collapse state
	 */
	void toggleCollapseState();

	/**
	 * Use this instead of resize.
	 * Depending on the sidebar location this will set the width or heigth
	 * of this auto hide container.
	 */
	void setSize(int Size);
};
} // namespace ads

//-----------------------------------------------------------------------------
#endif

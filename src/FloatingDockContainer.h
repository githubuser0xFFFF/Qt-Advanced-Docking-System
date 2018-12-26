#ifndef FloatingDockContainerH
#define FloatingDockContainerH
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
/// \file   FloatingDockContainer.h
/// \author Uwe Kindler
/// \date   01.03.2017
/// \brief  Declaration of CFloatingDockContainer class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QWidget>

#include "ads_globals.h"

class QXmlStreamReader;

namespace ads
{
struct FloatingDockContainerPrivate;
class CDockManager;
struct DockManagerPrivate;
class CDockAreaWidget;
class CDockContainerWidget;
class CDockWidget;
class CDockManager;
class CDockAreaTabBar;
class CDockWidgetTab;
struct DockWidgetTabPrivate;
class CDockAreaTitleBar;
struct DockAreaTitleBarPrivate;

/**
 * This implements a floating widget that is a dock container that accepts
 * docking of dock widgets like the main window and that can be docked into
 * another dock container
 */
class ADS_EXPORT CFloatingDockContainer : public QWidget
{
	Q_OBJECT
private:
	FloatingDockContainerPrivate* d; ///< private data (pimpl)
	friend struct FloatingDockContainerPrivate;
	friend class CDockManager;
	friend struct DockManagerPrivate;
	friend class CDockAreaTabBar;
	friend struct DockWidgetTabPrivate;
	friend class CDockWidgetTab;
	friend class CDockAreaTitleBar;
	friend struct DockAreaTitleBarPrivate;
	friend class CDockWidget;
	friend class CDockAreaWidget;

private slots:
	void onDockAreasAddedOrRemoved();
	void onDockAreaCurrentChanged(int Index);

protected:
	/**
	 * Starts floating at the given global position.
	 * Use moveToGlobalPos() to move the widget to a new position
	 * depending on the start position given in Pos parameter
	 */
	void startFloating(const QPoint& DragStartMousePos, const QSize& Size,
		eDragState DragState);

	/**
	 * Call this function to start dragging the floating widget
	 */
	void startDragging(const QPoint& DragStartMousePos, const QSize& Size)
	{
		startFloating(DragStartMousePos, Size, DraggingFloatingWidget);
	}

	/**
	 * Call this function if you just want to initialize the position
	 * and size of the floating widget
	 */
	void initFloatingGeometry(const QPoint& DragStartMousePos, const QSize& Size)
	{
		startFloating(DragStartMousePos, Size, DraggingInactive);
	}

	/**
	 * Moves the widget to a new position relative to the position given when
	 * startFloating() was called
	 */
	void moveFloating();

	/**
	 * Restores the state from given stream.
	 * If Testing is true, the function only parses the data from the given
	 * stream but does not restore anything. You can use this check for
	 * faulty files before you start restoring the state
	 */
	bool restoreState(QXmlStreamReader& Stream, bool Testing);

	/**
	 * Call this function to update the window title
	 */
	void updateWindowTitle();


protected: // reimplements QWidget
	virtual void changeEvent(QEvent *event) override;
	virtual void moveEvent(QMoveEvent *event) override;
	virtual bool event(QEvent *e) override;
	virtual void closeEvent(QCloseEvent *event) override;
	virtual void hideEvent(QHideEvent *event) override;
	virtual void showEvent(QShowEvent *event) override;
	virtual bool eventFilter(QObject *watched, QEvent *event) override;

public:
	using Super = QWidget;

	/**
	 * Create empty flatingb widget - required for restore state
	 */
	CFloatingDockContainer(CDockManager* DockManager);

	/**
	 * Create floating widget with the given dock area
	 */
	CFloatingDockContainer(CDockAreaWidget* DockArea);

	/**
	 * Create floating widget with the given dock widget
	 */
	CFloatingDockContainer(CDockWidget* DockWidget);

	/**
	 * Virtual Destructor
	 */
	virtual ~CFloatingDockContainer();

	/**
	 * Access function for the internal dock container
	 */
	CDockContainerWidget* dockContainer() const;

	/**
	 * This function returns true, if it can be closed.
	 * It can be closed, if all dock widgets in all dock areas can be closed
	 */
    bool isClosable() const;

    /**
     * This function returns true, if this floating widget has only one single
     * visible dock widget in a single visible dock area.
     * The single dock widget is a real top level floating widget because no
     * other widgets are docked.
     */
    bool hasTopLevelDockWidget() const;

    /**
     * This function returns the first dock widget in the first dock area.
     * If the function hasSingleDockWidget() returns true, then this function
     * returns this single dock widget.
     */
    CDockWidget* topLevelDockWidget() const;

    /**
     * This function returns a list of all dock widget in this floating widget.
     * This is a simple convenience function that simply calls the dockWidgets()
     * function of the internal container widget.
     */
    QList<CDockWidget*> dockWidgets() const;
}; // class FloatingDockContainer
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // FloatingDockContainerH

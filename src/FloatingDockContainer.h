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

#include "shared_export.h"

class QXmlStreamReader;

namespace ads
{
struct FloatingDockContainerPrivate;
class CDockAreaWidget;
class CDockContainerWidget;
class CDockWidget;
class CDockManager;

/**
 * This implements a floating widget that is a dock container that accepts
 * docking of dock widgets like the main window and that can be docked into
 * another dock container
 */
class SHARED_EXPORT CFloatingDockContainer : public QWidget
{
	Q_OBJECT
private:
	FloatingDockContainerPrivate* d; ///< private data (pimpl)
	friend struct FloatingDockContainerPrivate;

private slots:
	void onDockAreasAddedOrRemoved();
	void onDockAreaCurrentChanged(int Index);


protected: // reimplements QWidget
	virtual void changeEvent(QEvent *event) override;
	virtual void moveEvent(QMoveEvent *event) override;
	virtual bool event(QEvent *e) override;
	virtual void closeEvent(QCloseEvent *event) override;
	virtual void hideEvent(QHideEvent *event) override;
	virtual void showEvent(QShowEvent *event) override;
	virtual bool eventFilter(QObject *watched, QEvent *event) override;

public:
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
	 * Starts floating at the given global position.
	 * Use moveToGlobalPos() to move the widget to a new position
	 * depending on the start position given in Pos parameter
	 */
	void startFloating(const QPoint& Pos, const QSize& Size = QSize());

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
}; // class FloatingDockContainer
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // FloatingDockContainerH

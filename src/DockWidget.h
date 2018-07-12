#ifndef DockWidgetH
#define DockWidgetH
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
/// \file   DockWidget.h
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Declaration of CDockWidget class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

#include "shared_export.h"

class QXmlStreamWriter;

namespace ads
{
struct DockWidgetPrivate;
class CDockWidgetTitleBar;
class CDockManager;
class CDockContainerWidget;
class CDockAreaWidget;
struct DockContainerWidgetPrivate;

/**
 * The QDockWidget class provides a widget that can be docked inside a
 * CDockManager or floated as a top-level window on the desktop.
 */
class SHARED_EXPORT CDockWidget : public QFrame
{
	Q_OBJECT
private:
	DockWidgetPrivate* d; ///< private data (pimpl)
	friend struct DockWidgetPrivate;

protected:
	friend class CDockContainerWidget;
	friend class CDockAreaWidget;
	friend class CFloatingDockContainer;
	friend class CDockManager;
	friend struct DockContainerWidgetPrivate;

	/**
	 * Assigns the dock manager that manages this dock widget
	 */
	void setDockManager(CDockManager* DockManager);

	/**
	 * If this dock widget is inserted into a dock area, the dock area will
	 * be registered on this widget via this function. If a dock widget is
	 * removed from a dock area, this function will be called with nullptr
	 * value.
	 */
	void setDockArea(CDockAreaWidget* DockArea);

	/**
	 * This function changes the toggle view action without emitting any
	 * signal
	 */
	void setToggleViewActionChecked(bool Checked);

	/**
	 * Saves the state into the given stream
	 */
	void saveState(QXmlStreamWriter& Stream) const;

	/**
	 * This is a helper function for the dock manager to flag this widget
	 * as unassigned.
	 * When calling the restore function, it may happen, that the saved state
	 * contains less dock widgets then currently available. All widgets whose
	 * data is not contained in the saved state, are flagged as unassigned
	 * after the restore process. If the user shows an unassigned dock widget,
	 * a floating widget will be created to take up the dock widget.
	 */
	void flagAsUnassigned();

public:
	enum DockWidgetFeature
	{
		DockWidgetClosable = 0x01,
		DockWidgetMovable = 0x02,
		DockWidgetFloatable = 0x04,
		AllDockWidgetFeatures = DockWidgetClosable | DockWidgetMovable | DockWidgetFloatable,
		NoDockWidgetFeatures = 0x00
	};
	Q_DECLARE_FLAGS(DockWidgetFeatures, DockWidgetFeature)


	enum eState
	{
		StateHidden,
		StateDocked,
		StateFloating
	};

	/**
	 * This mode configures the behavior of the toggle view action.
	 * If the mode if ActionModeToggle, then the toggle view action is
	 * a checkable action to show / hide the dock widget. If the mode
	 * is ActionModeShow, then the action is not checkable an it will
	 * always show the dock widget if clicked. If the mode is ActionModeShow,
	 * the user can only close the DockWidget with the close button.
	 */
	enum eToggleViewActionMode
	{
		ActionModeToggle,//!< ActionModeToggle
		ActionModeShow   //!< ActionModeShow
	};

	/**
	 * Default Constructor
	 */
	CDockWidget(const QString &title, QWidget* parent = 0);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockWidget();

	/**
	 * Sets the widget for the dock widget to widget.
	 */
	void setWidget(QWidget* widget);

	/**
	 * Returns the widget for the dock widget. This function returns zero if
	 * the widget has not been set.
	 */
	QWidget* widget() const;

	/**
	 * Returns the title bar widget of this dock widget
	 */
	CDockWidgetTitleBar* titleBar() const;

	/**
	 * Sets, whether the dock widget is movable, closable, and floatable.
	 */
	void setFeatures(DockWidgetFeatures features);

	/**
	 * This property holds whether the dock widget is movable, closable, and
	 * floatable.
	 * By default, this property is set to a combination of DockWidgetClosable,
	 * DockWidgetMovable and DockWidgetFloatable.
	 */
	DockWidgetFeatures features() const;

	/**
	 * Returns the dock manager that manages the dock widget or 0 if the widget
	 * has not been assigned to any dock manager yet
	 */
	CDockManager* dockManager() const;

	/**
	 * Returns the dock container widget this dock area widget belongs to or 0
	 * if this dock widget has nt been docked yet
	 */
	CDockContainerWidget* dockContainer() const;

	/**
	 * Returns the dock area widget this dock widget belongs to or 0
	 * if this dock widget has not been docked yet
	 */
	CDockAreaWidget* dockAreaWidget() const;

	/**
	 * This property holds whether the dock widget is floating.
	 */
	bool isFloating() const;

	/**
	 * Returns true, if this dock widget is closed.
	 */
	bool isClosed() const;

	/**
	 * Returns a checkable action that can be used to show or close this dock widget.
	 * The action's text is set to the dock widget's window title.
	 */
	QAction* toggleViewAction() const;

	/**
	 * Configures the behavior of the toggle view action.
	 * \see eToggleViewActionMode for a detailed description
	 */
	void setToggleViewActionMode(eToggleViewActionMode Mode);

	/**
	 * Emits titleChanged signal if title change event occurs
	 */
	virtual bool event(QEvent *e) override;

	/**
	 * Sets the dock widget icon that is shown in tabs and in toggle view
	 * actions
	 */
	void setIcon(const QIcon& Icon);

public slots:
	/**
	 * This property controls whether the dock widget is open or closed.
	 * The toogleViewAction triggers this slot
	 */
	void toggleView(bool Open = true);

signals:
	/**
	 * This signal is emitted if the dock widget is opened or closed
	 */
	void viewToggled(bool Open);

	/**
	 * This signal is emitted if the dock widget is closed
	 */
	void closed();

	/**
	 * This signal is emitted if the window title of this dock widget
	 * changed
	 */
	void titleChanged(const QString& Title);
}; // class DockWidget
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockWidgetH

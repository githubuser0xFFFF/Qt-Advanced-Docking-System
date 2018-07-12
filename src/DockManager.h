#ifndef DockManagerH
#define DockManagerH
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
/// \file   DockManager.h
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Declaration of CDockManager class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include "DockContainerWidget.h"

#include "shared_export.h"

class QSettings;

namespace ads
{
struct DockManagerPrivate;
class CFloatingDockContainer;
class CDockContainerWidget;
class CDockOverlay;

/**
 * The central dock manager that maintains the complete docking system
 **/
class SHARED_EXPORT CDockManager : public CDockContainerWidget
{
	Q_OBJECT
private:
	DockManagerPrivate* d; ///< private data (pimpl)
	friend struct DockManagerPrivate;

protected:


public:
	/**
	 * Default Constructor.
	 * If the given parent is a QMainWindow, the dock manager sets itself as the
	 * central widget
	 */
	CDockManager(QWidget* parent = 0);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockManager();

	/**
	 * Registers the given floating widget in the internal list of
	 * floating widgets
	 */
	void registerFloatingWidget(CFloatingDockContainer* FloatingWidget);

	/**
	 * Remove the given floating widget from the list of registered floating
	 * widgets
	 */
	void removeFloatingWidget(CFloatingDockContainer* FloatingWidget);

	/**
	 * Registers the given dock container widget
	 */
	void registerDockContainer(CDockContainerWidget* DockContainer);

	/**
	 * Remove dock container from the internal list of registered dock
	 * containers
	 */
	void removeDockContainer(CDockContainerWidget* DockContainer);

	/**
	 * Overlay for containers
	 */
	CDockOverlay* containerOverlay() const;

	/**
	 * Overlay for dock areas
	 */
	CDockOverlay* dockAreaOverlay() const;

	/**
	 * Adds dockwidget into the given area.
	 * If DockAreaWidget is not null, then the area parameter indicates the area
	 * into the DockAreaWidget. If DockAreaWidget is null, the Dockwidget will
	 * be dropped into the container.
	 * \return Returns the dock area widget that contains the new DockWidget
	 */
	CDockAreaWidget* addDockWidget(DockWidgetArea area, CDockWidget* Dockwidget,
		CDockAreaWidget* DockAreaWidget = nullptr);

	/**
	 * Searches for a registered doc widget with the given ObjectName
	 * \return Return the found dock widget or nullptr if a dock widget with the
	 * given name is not registered
	 */
	CDockWidget* findDockWidget(const QString& ObjectName);

	/**
	 * Returns the list of all active and visible dock containers
	 * Dock containers are the main dock manager and all floating widgets
	 */
	const QList<CDockContainerWidget*> dockContainers() const;

	/**
	 * Returns the list of all floating widgets
	 */
	const QList<CFloatingDockContainer*> floatingWidgets() const;

	/**
	 * This function always return 0 because the main window is always behind
	 * any floating widget
	 */
	virtual unsigned int zOrderIndex() const;

	/**
	 * Saves the current state of the dockmanger and all its dock widgets
	 * into the returned QByteArray
	 */
	QByteArray saveState(int version = 0) const;

	/**
	 * Restores the state of this dockmanagers dockwidgets.
	 * The version number is compared with that stored in state. If they do
	 * not match, the dockmanager's state is left unchanged, and this function
	 * returns false; otherwise, the state is restored, and this function
	 * returns true.
	 */
	bool restoreState(const QByteArray &state, int version = 0);

	/**
	 * Saves the current perspective to the internal list of perspectives.
	 * A perspective is the current state of the dock manager assigned
	 * with a certain name. This makes it possible for the user,
	 * to switch between different perspectives quickly.
	 * If a perspective with the given name already exists, then
	 * it will be overwritten with the new state.
	 */
	void addPerspective(const QString& UniquePrespectiveName);

	/**
	 * Returns the names of all available perspectives
	 */
	QStringList perspectiveNames() const;

	/**
	 * Saves the perspectives to the given settings file.
	 */
	void savePerspectives(QSettings& Settings) const;

	/**
	 * Loads the perspectives from the given settings file
	 */
	void loadPerspectives(QSettings& Settings);

public slots:
	/**
	 * Opens the perspective with the given name.
	 */
	void openPerspective(const QString& PerspectiveName);

signals:
	/**
	 * This signal is emitted if the list of perspectives changed
	 */
	void perspectiveListChanged();
}; // class DockManager
} // namespace ads
//-----------------------------------------------------------------------------
#endif // DockManagerH

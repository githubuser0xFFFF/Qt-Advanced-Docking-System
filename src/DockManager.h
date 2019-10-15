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
#include <QIcon>

#include "ads_globals.h"

class QSettings;
class QMenu;

namespace ads
{
struct DockManagerPrivate;
class CFloatingDockContainer;
struct FloatingDockContainerPrivate;
class CDockContainerWidget;
class CDockOverlay;
class CDockAreaTabBar;
class CDockWidgetTab;
struct DockWidgetTabPrivate;
struct DockAreaWidgetPrivate;

/**
 * The central dock manager that maintains the complete docking system.
 * With the configuration flags you can globally control the functionality
 * of the docking system. The dock manager uses an internal stylesheet to
 * style its components like splitters, tabs and buttons. If you want to
 * disable this stylesheet because your application uses its own,
 * just call the function for settings the stylesheet with an empty
 * string.
 * \code
 * DockManager->setStyleSheet("");
 * \endcode
 **/
class ADS_EXPORT CDockManager : public CDockContainerWidget
{
	Q_OBJECT
private:
	DockManagerPrivate* d; ///< private data (pimpl)
	friend struct DockManagerPrivate;
	friend class CFloatingDockContainer;
	friend struct FloatingDockContainerPrivate;
	friend class CDockContainerWidget;
	friend class CDockAreaTabBar;
	friend class CDockWidgetTab;
	friend struct DockAreaWidgetPrivate;
	friend struct DockWidgetTabPrivate;

protected:
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

public:
	enum eViewMenuInsertionOrder
	{
		MenuSortedByInsertion,
		MenuAlphabeticallySorted
	};

	/**
	 * These global configuration flags configure some global dock manager
	 * settings.
	 */
	enum eConfigFlag
	{
		ActiveTabHasCloseButton = 0x0001,    //!< If this flag is set, the active tab in a tab area has a close button
		DockAreaHasCloseButton = 0x0002,     //!< If the flag is set each dock area has a close button
		DockAreaCloseButtonClosesTab = 0x0004,//!< If the flag is set, the dock area close button closes the active tab, if not set, it closes the complete cock area
		OpaqueSplitterResize = 0x0008, //!< See QSplitter::setOpaqueResize() documentation
		XmlAutoFormattingEnabled = 0x0010,//!< If enabled, the XML writer automatically adds line-breaks and indentation to empty sections between elements (ignorable whitespace).
		XmlCompressionEnabled = 0x0020,//!< If enabled, the XML output will be compressed and is not human readable anymore
		TabCloseButtonIsToolButton = 0x0040,//! If enabled the tab close buttons will be QToolButtons instead of QPushButtons - disabled by default
		AllTabsHaveCloseButton = 0x0080, //!< if this flag is set, then all tabs that are closable show a close button
		RetainTabSizeWhenCloseButtonHidden = 0x0100, //!< if this flag is set, the space for the close button is reserved even if the close button is not visible
		DefaultConfig = ActiveTabHasCloseButton | DockAreaHasCloseButton | OpaqueSplitterResize | XmlCompressionEnabled, ///< the default configuration
	};
	Q_DECLARE_FLAGS(ConfigFlags, eConfigFlag)

	/**
	 * Default Constructor.
	 * If the given parent is a QMainWindow, the dock manager sets itself as the
	 * central widget.
	 * Before you create any dock widgets, you should properly setup the
	 * configuration flags via setConfigFlags().
	 */
	CDockManager(QWidget* parent = 0);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockManager();

	/**
	 * This function returns the global configuration flags
	 */
	static ConfigFlags configFlags();

	/**
	 * Sets the global configuration flags for the whole docking system.
	 * Call this function before you create your first dock widget.
	 */
	static void setConfigFlags(const ConfigFlags Flags);

	/**
	 * Set a certain config flag
	 */
	static void setConfigFlag(eConfigFlag Flag, bool On = true);

	/**
	 * Adds dockwidget into the given area.
	 * If DockAreaWidget is not null, then the area parameter indicates the area
	 * into the DockAreaWidget. If DockAreaWidget is null, the Dockwidget will
	 * be dropped into the container. If you would like to add a dock widget
	 * tabified, then you need to add it to an existing dock area object
	 * into the CenterDockWidgetArea. The following code shows this:
	 * \code
	 * DockManager->addDockWidget(ads::CenterDockWidgetArea, NewDockWidget,
	 * 	   ExisitingDockArea);
	 * \endcode
	 * \return Returns the dock area widget that contains the new DockWidget
	 */
	CDockAreaWidget* addDockWidget(DockWidgetArea area, CDockWidget* Dockwidget,
		CDockAreaWidget* DockAreaWidget = nullptr);

	/**
	 * This function will add the given Dockwidget to the given dock area as
	 * a new tab.
	 * If no dock area widget exists for the given area identifier, a new
	 * dock area widget is created.
	 */
	CDockAreaWidget* addDockWidgetTab(DockWidgetArea area,
		CDockWidget* Dockwidget);

	/**
	 * This function will add the given Dockwidget to the given DockAreaWidget
	 * as a new tab.
	 */
	CDockAreaWidget* addDockWidgetTabToArea(CDockWidget* Dockwidget,
		CDockAreaWidget* DockAreaWidget);

	/**
	 * Searches for a registered doc widget with the given ObjectName
	 * \return Return the found dock widget or nullptr if a dock widget with the
	 * given name is not registered
	 */
	CDockWidget* findDockWidget(const QString& ObjectName) const;

	/**
	 * Remove the given Dock from the dock manager
	 */
	void removeDockWidget(CDockWidget* Dockwidget);

	/**
	 * This function returns a readable reference to the internal dock
	 * widgets map so that it is possible to iterate over all dock widgets
	 */
	QMap<QString, CDockWidget*> dockWidgetsMap() const;

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
	 * into the returned QByteArray.
	 * The XmlMode enables / disables the auto formatting for the XmlStreamWriter.
	 * If auto formatting is enabled, the output is intended and line wrapped.
	 * The XmlMode XmlAutoFormattingDisabled is better if you would like to have
	 * a more compact XML output - i.e. for storage in ini files.
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
	 * Removes the perspective with the given name from the list of perspectives
	 */
	void removePerspective(const QString& Name);

	/**
	 * Removes the given perspectives from the dock manager
	 */
	void removePerspectives(const QStringList& Names);

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

	/**
	 * Adds a toggle view action to the the internal view menu.
	 * You can either manage the insertion of the toggle view actions in your
	 * application or you can add the actions to the internal view menu and
	 * then simply insert the menu object into your.
	 * \param[in] ToggleViewAction The action to insert. If no group is provided
	 *            the action is directly inserted into the menu. If a group
	 *            is provided, the action is inserted into the group and the
	 *            group is inserted into the menu if it is not existing yet.
	 * \param[in] Group This is the text used for the group menu item
	 * \param[in] GroupIcon The icon used for grouping the workbenches in the
	 *            view menu. I.e. if there is a workbench for each device
	 *            like for spectrometer devices, it is good to group all these
	 *            workbenches under a menu item
	 * \return If Group is not empty, this function returns the GroupAction
	 *         for this group. If the group is empty, the function returns
	 *         the given ToggleViewAction.
	 */
	QAction* addToggleViewActionToMenu(QAction* ToggleViewAction,
		const QString& Group = QString(), const QIcon& GroupIcon = QIcon());

	/**
	 * This function returns the internal view menu.
	 * To fill the view menu, you can use the addToggleViewActionToMenu()
	 * function.
	 */
	QMenu* viewMenu() const;

	/**
	 * Define the insertion order for toggle view menu items.
	 * The order defines how the actions are added to the view menu.
	 * The default insertion order is MenuAlphabeticallySorted to make it
	 * easier for users to find the menu entry for a certain dock widget.
	 * You need to call this function befor you insert the first menu item
	 * into the view menu.
	 */
	void setViewMenuInsertionOrder(eViewMenuInsertionOrder Order);

	/**
	 * This function returns true between the restoringState() and
	 * stateRestored() signals.
	 */
	bool isRestoringState() const;

	/**
	 * The distance the user needs to move the mouse with the left button
	 * hold down before a dock widget start floating
	 */
	static int startDragDistance();

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

	/**
	 * This signal is emitted if perspectives have been removed
	 */
	void perspectivesRemoved();

	/**
	 * This signal is emitted, if the restore function is called, just before
	 * the dock manager starts restoring the state.
	 * If this function is called, nothing has changed yet
	 */
	void restoringState();

    /**
     * This signal is emitted if the state changed in restoreState.
     * The signal is emitted if the restoreState() function is called or
     * if the openPerspective() function is called
     */
    void stateRestored();

    /**
     * This signal is emitted, if the dock manager starts opening a
     * perspective.
     * Opening a perspective may take more than a second if there are
     * many complex widgets. The application may use this signal
     * to show some progress indicator or to change the mouse cursor
     * into a busy cursor.
     */
    void openingPerspective(const QString& PerspectiveName);

    /**
     * This signal is emitted if the dock manager finished opening a
     * perspective
     */
    void perspectiveOpened(const QString& PerspectiveName);
}; // class DockManager
} // namespace ads
//-----------------------------------------------------------------------------
#endif // DockManagerH

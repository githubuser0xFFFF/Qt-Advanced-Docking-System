#ifndef DockAreaTitleBarH
#define DockAreaTitleBarH
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
/// \file   DockAreaTitleBar.h
/// \author Uwe Kindler
/// \date   12.10.2018
/// \brief  Declaration of CDockAreaTitleBar class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QFrame>

#include "ads_globals.h"

class QAbstractButton;

namespace ads
{
class CDockAreaTabBar;
class CDockAreaWidget;
struct DockAreaTitleBarPrivate;

/**
 * Title bar of a dock area
 */
class CDockAreaTitleBar : public QFrame
{
	Q_OBJECT
private:
	DockAreaTitleBarPrivate* d; ///< private data (pimpl)
	friend struct DockAreaTitleBarPrivate;

private slots:
	void markTabsMenuOutdated();
	void onTabsMenuAboutToShow();
	void onCloseButtonClicked();
	void onUndockButtonClicked();
	void onTabsMenuActionTriggered(QAction* Action);
	void onCurrentTabChanged(int Index);
	void showContextMenu(const QPoint& pos);

public:
	using Super = QFrame;
	/**
	 * Default Constructor
	 */
	CDockAreaTitleBar(CDockAreaWidget* parent);

	/**
	 * Virtual Destructor
	 */
	virtual ~CDockAreaTitleBar();

	/**
	 * Returns the pointer to the tabBar()
	 */
	CDockAreaTabBar* tabBar() const;

	/**
	 * Returns the button corresponding to the given title bar button identifier
	 */
	QAbstractButton* button(TitleBarButton which) const;

	/**
	 * This function is here for debug reasons
	 */
	virtual void setVisible(bool Visible) override;

signals:
	/**
	 * This signal is emitted if a tab in the tab bar is clicked by the user
	 * or if the user clicks on a tab item in the title bar tab menu.
	 */
	void tabBarClicked(int index);
}; // class name
}
 // namespace ads
//-----------------------------------------------------------------------------
#endif // DockAreaTitleBarH

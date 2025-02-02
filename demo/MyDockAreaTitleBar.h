#ifndef QTADS_MYDOCKAREATITLEBAR_H
#define QTADS_MYDOCKAREATITLEBAR_H
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
//                                   INCLUDES
//============================================================================
#include <DockAreaTitleBar.h>


/**
 * Custom DockAreaTitleBar that adds a custom context menu
 */
class MyDockAreaTitleBar : public ads::CDockAreaTitleBar
{
public:
	explicit MyDockAreaTitleBar(ads::CDockAreaWidget *parent) :
		CDockAreaTitleBar(parent)
	{
	}

	QMenu* buildContextMenu(QMenu*) override
	{
		auto menu = ads::CDockAreaTitleBar::buildContextMenu(nullptr);
		menu->addSeparator();
		auto action = menu->addAction(tr("Format HardDrive"));

		connect(action, &QAction::triggered, this, [this]()
		{
			QMessageBox msgBox;
			msgBox.setText("No, just kidding");
			msgBox.setStandardButtons(QMessageBox::Abort);
			msgBox.setDefaultButton(QMessageBox::Abort);
			msgBox.exec();
		});

		return menu;
	}
};

#endif  // QTADS_MYDOCKAREATITLEBAR_H

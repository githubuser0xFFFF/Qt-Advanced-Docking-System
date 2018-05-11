#ifndef ads_globalsH
#define ads_globalsH
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
/// \file   ads_globals.h
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Declaration of
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QPair>

#include "shared_export.h"

class QSplitter;

namespace ads
{
SHARED_EXPORT enum DockWidgetArea
{
	NoDockWidgetArea = 0x00,
	LeftDockWidgetArea = 0x01,
	RightDockWidgetArea = 0x02,
	TopDockWidgetArea = 0x04,
	BottomDockWidgetArea = 0x08,
	CenterDockWidgetArea = 0x10,

	InvalidDockWidgetArea = NoDockWidgetArea,
	OuterDockAreas = TopDockWidgetArea | LeftDockWidgetArea | RightDockWidgetArea | BottomDockWidgetArea,
	AllDockAreas = OuterDockAreas | CenterDockWidgetArea
};
Q_DECLARE_FLAGS(DockWidgetAreas, DockWidgetArea)


namespace internal
{


/**
 * Helper function to create new splitter widgets
 */
QSplitter* newSplitter(Qt::Orientation orientation, QWidget* parent = 0);

/**
 * Replace the from widget in the given splitter with the To widget
 */
void replaceSplitterWidget(QSplitter* Splitter, QWidget* From, QWidget* To);

/**
 * Convenience class for QPair to provide better naming than first and
 * second
 */
class CDockInsertParam : public QPair<Qt::Orientation, bool>
{
public:
	using QPair::QPair;
	Qt::Orientation orientation() const {return this->first;}
	bool append() const {return this->second;}
	int insertOffset() const {return append() ? 1 : 0;}
};

/**
 * Returns the insertion parameters for the given dock area
 */
CDockInsertParam dockAreaInsertParameters(DockWidgetArea Area);

/**
 * Searches for the parent widget of the given type.
 * Returns the parent widget of the given widget or 0 if the widget is not
 * child of any widget of type T
 */
template <class T>
T findParent(const QWidget* w)
{
	QWidget* parentWidget = w->parentWidget();
	while (parentWidget)
	{
		T ParentImpl = dynamic_cast<T>(parentWidget);
		if (ParentImpl)
		{
			return ParentImpl;
		}
		parentWidget = parentWidget->parentWidget();
	}
	return 0;
}

} // namespace internal
} // namespace ads

//---------------------------------------------------------------------------
#endif // ads_globalsH

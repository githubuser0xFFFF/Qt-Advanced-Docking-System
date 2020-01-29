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
/// \file   ads_globals.cpp
/// \author Uwe Kindler
/// \date   24.02.2017
/// \brief  Implementation of
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <QVariant>
#include <QPainter>

#include "DockSplitter.h"
#include "ads_globals.h"


namespace ads
{

namespace internal
{
//============================================================================
void replaceSplitterWidget(QSplitter* Splitter, QWidget* From, QWidget* To)
{
	int index = Splitter->indexOf(From);
	From->setParent(nullptr);
	Splitter->insertWidget(index, To);
}

//============================================================================
CDockInsertParam dockAreaInsertParameters(DockWidgetArea Area)
{
	switch (Area)
    {
	case TopDockWidgetArea: return CDockInsertParam(Qt::Vertical, false);
	case RightDockWidgetArea: return CDockInsertParam(Qt::Horizontal, true);
	case CenterDockWidgetArea:
	case BottomDockWidgetArea: return CDockInsertParam(Qt::Vertical, true);
	case LeftDockWidgetArea: return CDockInsertParam(Qt::Horizontal, false);
	default: CDockInsertParam(Qt::Vertical, false);
    } // switch (Area)

	return CDockInsertParam(Qt::Vertical, false);
}


//============================================================================
QPixmap createTransparentPixmap(const QPixmap& Source, qreal Opacity)
{
	QPixmap TransparentPixmap(Source.size());
	TransparentPixmap.fill(Qt::transparent);
	QPainter p(&TransparentPixmap);
	p.setOpacity(Opacity);
	p.drawPixmap(0, 0, Source);
	return TransparentPixmap;
}


//============================================================================
void hideEmptyParentSplitters(CDockSplitter* Splitter)
{
	while (Splitter && Splitter->isVisible())
	{
		if (!Splitter->hasVisibleContent())
		{
			Splitter->hide();
		}
		Splitter = internal::findParent<CDockSplitter*>(Splitter);
	}
}

} // namespace internal
} // namespace ads



//---------------------------------------------------------------------------
// EOF ads_globals.cpp

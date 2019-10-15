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
#include <QtCore/QtGlobal>
#include <QPixmap>
#include <QWidget>

#ifndef ADS_STATIC
#ifdef ADS_SHARED_EXPORT
#define ADS_EXPORT Q_DECL_EXPORT
#else
#define ADS_EXPORT Q_DECL_IMPORT
#endif
#else
#define ADS_EXPORT
#endif

// Define ADS_DEBUG_PRINT to enable a lot of debug output
#ifdef ADS_DEBUG_PRINT
#define ADS_PRINT(s) qDebug() << s
#else
#define ADS_PRINT(s)
#endif

// Set ADS_DEBUG_LEVEL to enable additional debug output and to enable layout
// dumps to qDebug and std::cout after layout changes
#define ADS_DEBUG_LEVEL 0

class QSplitter;

namespace ads
{
class CDockSplitter;

enum DockWidgetArea
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


enum TitleBarButton
{
	TitleBarButtonTabsMenu,
	TitleBarButtonUndock,
	TitleBarButtonClose
};

/**
 * The different dragging states
 */
enum eDragState
{
	DraggingInactive,     //!< DraggingInactive
	DraggingMousePressed, //!< DraggingMousePressed
	DraggingTab,          //!< DraggingTab
	DraggingFloatingWidget//!< DraggingFloatingWidget
};

namespace internal
{
static const bool RestoreTesting = true;
static const bool Restore = false;
static const char* const ClosedProperty = "close";
static const char* const DirtyProperty = "dirty";

/**
 * Replace the from widget in the given splitter with the To widget
 */
void replaceSplitterWidget(QSplitter* Splitter, QWidget* From, QWidget* To);

/**
 * This function walks the splitter tree upwards to hides all splitters
 * that do not have visible content
 */
void hideEmptyParentSplitters(CDockSplitter* FirstParentSplitter);

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
 *
 * It is not safe to use this function in in CDockWidget because only
 * the current dock widget has a parent. All dock widgets that are not the
 * current dock widget in a dock area have no parent.
 */
template <class T>
T findParent(const QWidget* w)
{
	QWidget* parentWidget = w->parentWidget();
	while (parentWidget)
	{
		T ParentImpl = qobject_cast<T>(parentWidget);
		if (ParentImpl)
		{
			return ParentImpl;
		}
		parentWidget = parentWidget->parentWidget();
	}
	return 0;
}

/**
 * Creates a semi transparent pixmap from the given pixmap Source.
 * The Opacity parameter defines the opacity from completely transparent (0.0)
 * to completely opaque (1.0)
 */
QPixmap createTransparentPixmap(const QPixmap& Source, qreal Opacity);


/**
 * Helper function for settings flags in a QFlags instance.
 */
template <class T>
void setFlag(T& Flags, typename T::enum_type flag, bool on = true)
{
#if QT_VERSION >= 0x050700
	Flags.setFlag(flag, on);
#else
    if(on)
    {
        Flags |= flag;
    }
    else
    {
        Flags &= ~flag;
    }
#endif
}

} // namespace internal
} // namespace ads

//---------------------------------------------------------------------------
#endif // ads_globalsH

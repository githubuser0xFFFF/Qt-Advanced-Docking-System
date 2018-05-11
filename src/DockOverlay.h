#ifndef DockOverlayH
#define DockOverlayH
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
#include <QPointer>
#include <QHash>
#include <QRect>
#include <QFrame>
class QGridLayout;

#include "ads_globals.h"

#include "shared_export.h"

namespace ads
{
struct DockOverlayPrivate;
class CDockOverlayCross;

/*!
 * DockOverlay paints a translucent rectangle over another widget. The geometry
 * of the rectangle is based on the mouse location.
 */
class SHARED_EXPORT CDockOverlay : public QFrame
{
	Q_OBJECT
private:
	DockOverlayPrivate* d; //< private data class
	friend struct DockOverlayPrivate;
	friend class DockOverlayCross;

public:
	enum eMode
	{
		ModeDockAreaOverlay,
		ModeContainerOverlay
	};

	/**
	 * Creates a dock overlay
	 */
	CDockOverlay(QWidget* parent, eMode Mode = ModeDockAreaOverlay);

	/**
	 * Virtual destructor
	 */
	virtual ~CDockOverlay();

	/**
	 * Configures the areas that are allowed for docking
	 */
	void setAllowedAreas(DockWidgetAreas areas);

	/**
	 * Returns flags with all allowed drop areas
	 */
	DockWidgetAreas allowedAreas() const;

	/**
	 * Returns the drop area under the current cursor location
	 */
	DockWidgetArea dropAreaUnderCursor() const;

	/**
	 * Show the drop overly for the given target widget
	 */
	DockWidgetArea showOverlay(QWidget* target);

	/**
	 * Hides the overlay
	 */
	void hideOverlay();

	/**
	 * Enables / disables the semi transparent overlay rectangle that represents
	 * the future area of the dropped widget
	 */
	void enableDropPreview(bool Enable);

	/**
	 * The drop overlay rectangle for the target area
	 */
	QRect dropOverlayRect() const;

protected:
	virtual void paintEvent(QPaintEvent *e) override;
	virtual void showEvent(QShowEvent* e) override;
	virtual void hideEvent(QHideEvent* e) override;
};


struct DockOverlayCrossPrivate;
/*!
 * DockOverlayCross shows a cross with 5 different drop area possibilities.
 * I could have handled everything inside DockOverlay, but because of some
 * styling issues it's better to have a separate class for the cross.
 */
class CDockOverlayCross : public QWidget
{
	Q_OBJECT
private:
	DockOverlayCrossPrivate* d;
	friend struct DockOverlayCrossPrivate;
	friend class CDockOverlay;

public:
	/**
	 * Creates an overlay corss for the given overlay
	 */
	CDockOverlayCross(CDockOverlay* overlay);

	/**
	 * Virtual destructor
	 */
	virtual ~CDockOverlayCross();

	/**
	 * Returns the dock widget area depending on the current cursor location.
	 * The function checks, if the mouse cursor is inside of any drop indicator
	 * widget and returns the corresponding DockWidgetArea.
	 */
	DockWidgetArea cursorLocation() const;

	/**
	 * Sets up the overlay cross for the given overlay mode
	 */
	void setupOverlayCross(CDockOverlay::eMode Mode);

	/**
	 * Resets and updates the
	 */
	void reset();

	/**
	 * Updates the current position
	 */
	void updatePosition();

protected:
	virtual void showEvent(QShowEvent* e) override;
	void setAreaWidgets(const QHash<DockWidgetArea, QWidget*>& widgets);

private:

}; // CDockOverlayCross

} // namespace ads
#endif // DockOverlayH

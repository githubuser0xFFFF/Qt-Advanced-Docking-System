#ifndef FloatingOverlayH
#define FloatingOverlayH
//============================================================================
/// \file   FloatingOverlay.h
/// \author Uwe Kindler
/// \date   26.11.2019
/// \brief  Declaration of CFloatingOverlay
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <QWidget>
#include "FloatingDockContainer.h"

namespace ads
{
class CDockWidget;
class CDockAreaWidget;
struct FloatingOverlayPrivate;

/**
 * A floating overlay is a temporary floating widget that is just used to
 * indicate the floating widget movement
 */
class CFloatingOverlay : public QWidget, public IFloatingWidget
{
private:
	FloatingOverlayPrivate* d;
	friend class FloatingOverlayPrivate;

protected:
	virtual void moveEvent(QMoveEvent *event) override;
	virtual void paintEvent(QPaintEvent *e) override;

	/**
	 * The content is a DockArea or a DockWidget
	 */
	CFloatingOverlay(QWidget* Content, QWidget* parent);

public:
	using Super = QWidget;
	CFloatingOverlay(CDockWidget* Content);
	CFloatingOverlay(CDockAreaWidget* Content);

	/**
	 * Delete private data
	 */
	~CFloatingOverlay();

	virtual void startFloating(const QPoint& DragStartMousePos, const QSize& Size,
        eDragState DragState, QWidget* MouseEventHandler) override;

	/**
	 * Moves the widget to a new position relative to the position given when
	 * startFloating() was called
	 */
	virtual void moveFloating() override;

	/**
	 * Finishes dragging
	 */
	virtual void finishDragging() override;
};


} // namespace ads

//---------------------------------------------------------------------------
#endif // FloatingOverlayH


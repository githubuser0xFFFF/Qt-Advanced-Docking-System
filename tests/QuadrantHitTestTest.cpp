/*******************************************************************************
** [Wizard NLE fork] Unit test for CDockOverlay::quadrantAreaForCursor().
**
** Verifies the pure-function nearest-edge hit-test that backs the
** HalfPanelDropZones config flag. Constructed as a static QtTest target so it
** runs without a live overlay or DockManager.
******************************************************************************/

#include <QtTest/QtTest>

#include "DockOverlay.h"

using ads::CDockOverlay;
using ads::DockWidgetArea;
using ads::DockWidgetAreas;
using ads::LeftDockWidgetArea;
using ads::RightDockWidgetArea;
using ads::TopDockWidgetArea;
using ads::BottomDockWidgetArea;
using ads::CenterDockWidgetArea;
using ads::InvalidDockWidgetArea;
using ads::AllDockAreas;
using ads::OuterDockAreas;
using ads::NoDockWidgetArea;

namespace
{
constexpr DockWidgetAreas EdgesOnly = LeftDockWidgetArea
	| RightDockWidgetArea | TopDockWidgetArea | BottomDockWidgetArea;
}

class QuadrantHitTestTest : public QObject
{
	Q_OBJECT

private slots:
	// Cursor near each edge of a 100x100 square should snap to that edge.
	void cursorNearLeft_returnsLeft();
	void cursorNearRight_returnsRight();
	void cursorNearTop_returnsTop();
	void cursorNearBottom_returnsBottom();

	// Center of the rect: all four distances equal; tie-break order is
	// L > R > T > B per the Candidates iteration order.
	void cursorAtCenter_picksLeftByTieBreak();

	// Corners: two distances are zero. Tie-break still applies.
	void cursorAtTopLeftCorner_picksLeft();
	void cursorAtBottomRightCorner_picksRight();

	// Bounds checks.
	void cursorOutsideBounds_returnsInvalid();
	void invalidBounds_returnsInvalid();
	void zeroSizeBounds_returnsInvalid();

	// Allowed-areas mask.
	void allowedAreasExcludesNearestEdge_picksNextNearest();
	void allowedAreasIsEmpty_returnsInvalid();
	void allowedAreasOnlyCenter_returnsInvalid();
	void centerInAllowedAreas_isIgnored();

	// For a very wide rect with a vertically-centered cursor, the top/bottom
	// distances are smaller than the horizontal ones, so the cursor snaps to
	// Top by tie-break — a known consequence of pure nearest-edge math.
	void wideDockArea_verticallyCenteredCursor_picksTopByTieBreak();
};

void QuadrantHitTestTest::cursorNearLeft_returnsLeft()
{
	const QRect bounds(0, 0, 100, 100);
	const QPoint cursor(5, 50);
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, cursor, AllDockAreas),
		LeftDockWidgetArea);
}

void QuadrantHitTestTest::cursorNearRight_returnsRight()
{
	const QRect bounds(0, 0, 100, 100);
	const QPoint cursor(95, 50);
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, cursor, AllDockAreas),
		RightDockWidgetArea);
}

void QuadrantHitTestTest::cursorNearTop_returnsTop()
{
	const QRect bounds(0, 0, 100, 100);
	const QPoint cursor(50, 5);
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, cursor, AllDockAreas),
		TopDockWidgetArea);
}

void QuadrantHitTestTest::cursorNearBottom_returnsBottom()
{
	const QRect bounds(0, 0, 100, 100);
	const QPoint cursor(50, 95);
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, cursor, AllDockAreas),
		BottomDockWidgetArea);
}

void QuadrantHitTestTest::cursorAtCenter_picksLeftByTieBreak()
{
	const QRect bounds(0, 0, 100, 100);
	const QPoint cursor(50, 50);
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, cursor, AllDockAreas),
		LeftDockWidgetArea);
}

void QuadrantHitTestTest::cursorAtTopLeftCorner_picksLeft()
{
	const QRect bounds(0, 0, 100, 100);
	const QPoint cursor(0, 0);
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, cursor, AllDockAreas),
		LeftDockWidgetArea);
}

void QuadrantHitTestTest::cursorAtBottomRightCorner_picksRight()
{
	const QRect bounds(0, 0, 100, 100);
	const QPoint cursor(99, 99);
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, cursor, AllDockAreas),
		RightDockWidgetArea);
}

void QuadrantHitTestTest::cursorOutsideBounds_returnsInvalid()
{
	const QRect bounds(0, 0, 100, 100);
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, QPoint(-1, 50), AllDockAreas),
		InvalidDockWidgetArea);
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, QPoint(150, 50), AllDockAreas),
		InvalidDockWidgetArea);
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, QPoint(50, -1), AllDockAreas),
		InvalidDockWidgetArea);
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, QPoint(50, 150), AllDockAreas),
		InvalidDockWidgetArea);
}

void QuadrantHitTestTest::invalidBounds_returnsInvalid()
{
	const QRect bounds; // default-constructed: invalid
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, QPoint(0, 0), AllDockAreas),
		InvalidDockWidgetArea);
}

void QuadrantHitTestTest::zeroSizeBounds_returnsInvalid()
{
	const QRect bounds(10, 10, 0, 0); // QRect::isValid() is false
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, QPoint(10, 10), AllDockAreas),
		InvalidDockWidgetArea);
}

void QuadrantHitTestTest::allowedAreasExcludesNearestEdge_picksNextNearest()
{
	const QRect bounds(0, 0, 100, 100);
	const QPoint cursor(5, 50); // closest to left

	const DockWidgetAreas withoutLeft = EdgesOnly & ~DockWidgetAreas(LeftDockWidgetArea);
	// Without Left, next-best by distance for (5,50) in 100x100:
	//   dRight = 95, dTop = 50, dBottom = 50. Top wins by tie-break order.
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, cursor, withoutLeft),
		TopDockWidgetArea);
}

void QuadrantHitTestTest::allowedAreasIsEmpty_returnsInvalid()
{
	const QRect bounds(0, 0, 100, 100);
	const QPoint cursor(50, 50);
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, cursor, NoDockWidgetArea),
		InvalidDockWidgetArea);
}

void QuadrantHitTestTest::allowedAreasOnlyCenter_returnsInvalid()
{
	const QRect bounds(0, 0, 100, 100);
	const QPoint cursor(50, 50);
	// Quadrant logic never returns Center; if Center is the only allowed area,
	// the helper returns Invalid so the caller can fall back.
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, cursor,
			DockWidgetAreas(CenterDockWidgetArea)),
		InvalidDockWidgetArea);
}

void QuadrantHitTestTest::centerInAllowedAreas_isIgnored()
{
	const QRect bounds(0, 0, 100, 100);
	const QPoint cursor(5, 50);
	// Center is allowed but the helper still returns the nearest edge.
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, cursor, AllDockAreas),
		LeftDockWidgetArea);
}

void QuadrantHitTestTest::wideDockArea_verticallyCenteredCursor_picksTopByTieBreak()
{
	const QRect bounds(0, 0, 600, 200);
	const QPoint cursor(150, 100); // left third, vertical center
	// dLeft=150, dRight=450, dTop=100, dBottom=100. Top wins (smaller than
	// horizontal distances; tied with Bottom but earlier in iteration order).
	// This documents that for very wide rects, a vertically-centered cursor
	// snaps to Top/Bottom rather than Left/Right — a known consequence of
	// pure nearest-edge math. If this surprises users in practice we can add
	// an aspect-ratio-aware variant.
	QCOMPARE(CDockOverlay::quadrantAreaForCursor(bounds, cursor, AllDockAreas),
		TopDockWidgetArea);
}

QTEST_MAIN(QuadrantHitTestTest)
#include "QuadrantHitTestTest.moc"

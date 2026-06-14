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
/// \file   FloatingDockContainer.cpp
/// \author Uwe Kindler
/// \date   01.03.2017
/// \brief  Implementation of CFloatingDockContainer class
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include "FloatingDockContainer.h"

#include <iostream>

#include <QBoxLayout>
#include <QApplication>
#include <QMouseEvent>
#include <QPointer>
#include <QAction>
#include <QDebug>
#include <QAbstractButton>
#include <QElapsedTimer>
#include <QTime>
#include <QDrag>
#include <QMimeData>
#include <QDataStream>
#include <QWindow>

#include "DockContainerWidget.h"
#include "DockAreaWidget.h"
#include "DockManager.h"
#include "DockWidget.h"
#include "DockOverlay.h"
#include "FloatingDragPreview.h"

#ifdef Q_OS_WIN
#include <windows.h>
#ifdef _MSC_VER
#pragma comment(lib, "User32.lib")
#endif
#endif
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
#include "linux/FloatingWidgetTitleBar.h"
#include <xcb/xcb.h>
#endif

namespace ads
{
#ifdef Q_OS_WIN
#if 0 // set to 1 if you need this function for debugging
/**
 * Just for debugging to convert windows message identifiers to strings
 */
static const char* windowsMessageString(int MessageId)
{
	switch (MessageId)
	{
    case 0: return "WM_NULL";
    case 1: return "WM_CREATE";
    case 2: return "WM_DESTROY";
    case 3: return "WM_MOVE";
    case 5: return "WM_SIZE";
    case 6: return "WM_ACTIVATE";
    case 7: return "WM_SETFOCUS";
    case 8: return "WM_KILLFOCUS";
    case 10: return "WM_ENABLE";
    case 11: return "WM_SETREDRAW";
    case 12: return "WM_SETTEXT";
    case 13: return "WM_GETTEXT";
    case 14: return "WM_GETTEXTLENGTH";
    case 15: return "WM_PAINT";
    case 16: return "WM_CLOSE";
    case 17: return "WM_QUERYENDSESSION";
    case 18: return "WM_QUIT";
    case 19: return "WM_QUERYOPEN";
    case 20: return "WM_ERASEBKGND";
    case 21: return "WM_SYSCOLORCHANGE";
    case 22: return "WM_ENDSESSION";
    case 24: return "WM_SHOWWINDOW";
    case 25: return "WM_CTLCOLOR";
    case 26: return "WM_WININICHANGE";
    case 27: return "WM_DEVMODECHANGE";
    case 28: return "WM_ACTIVATEAPP";
    case 29: return "WM_FONTCHANGE";
    case 30: return "WM_TIMECHANGE";
    case 31: return "WM_CANCELMODE";
    case 32: return "WM_SETCURSOR";
    case 33: return "WM_MOUSEACTIVATE";
    case 34: return "WM_CHILDACTIVATE";
    case 35: return "WM_QUEUESYNC";
    case 36: return "WM_GETMINMAXINFO";
    case 38: return "WM_PAINTICON";
    case 39: return "WM_ICONERASEBKGND";
    case 40: return "WM_NEXTDLGCTL";
    case 42: return "WM_SPOOLERSTATUS";
    case 43: return "WM_DRAWITEM";
    case 44: return "WM_MEASUREITEM";
    case 45: return "WM_DELETEITEM";
    case 46: return "WM_VKEYTOITEM";
    case 47: return "WM_CHARTOITEM";
    case 48: return "WM_SETFONT";
    case 49: return "WM_GETFONT";
    case 50: return "WM_SETHOTKEY";
    case 51: return "WM_GETHOTKEY";
    case 55: return "WM_QUERYDRAGICON";
    case 57: return "WM_COMPAREITEM";
    case 61: return "WM_GETOBJECT";
    case 65: return "WM_COMPACTING";
    case 68: return "WM_COMMNOTIFY";
    case 70: return "WM_WINDOWPOSCHANGING";
    case 71: return "WM_WINDOWPOSCHANGED";
    case 72: return "WM_POWER";
    case 73: return "WM_COPYGLOBALDATA";
    case 74: return "WM_COPYDATA";
    case 75: return "WM_CANCELJOURNAL";
    case 78: return "WM_NOTIFY";
    case 80: return "WM_INPUTLANGCHANGEREQUEST";
    case 81: return "WM_INPUTLANGCHANGE";
    case 82: return "WM_TCARD";
    case 83: return "WM_HELP";
    case 84: return "WM_USERCHANGED";
    case 85: return "WM_NOTIFYFORMAT";
    case 123: return "WM_CONTEXTMENU";
    case 124: return "WM_STYLECHANGING";
    case 125: return "WM_STYLECHANGED";
    case 126: return "WM_DISPLAYCHANGE";
    case 127: return "WM_GETICON";
    case 128: return "WM_SETICON";
    case 129: return "WM_NCCREATE";
    case 130: return "WM_NCDESTROY";
    case 131: return "WM_NCCALCSIZE";
    case 132: return "WM_NCHITTEST";
    case 133: return "WM_NCPAINT";
    case 134: return "WM_NCACTIVATE";
    case 135: return "WM_GETDLGCODE";
    case 136: return "WM_SYNCPAINT";
    case 160: return "WM_NCMOUSEMOVE";
    case 161: return "WM_NCLBUTTONDOWN";
    case 162: return "WM_NCLBUTTONUP";
    case 163: return "WM_NCLBUTTONDBLCLK";
    case 164: return "WM_NCRBUTTONDOWN";
    case 165: return "WM_NCRBUTTONUP";
    case 166: return "WM_NCRBUTTONDBLCLK";
    case 167: return "WM_NCMBUTTONDOWN";
    case 168: return "WM_NCMBUTTONUP";
    case 169: return "WM_NCMBUTTONDBLCLK";
    case 171: return "WM_NCXBUTTONDOWN";
    case 172: return "WM_NCXBUTTONUP";
    case 173: return "WM_NCXBUTTONDBLCLK";
    case 176: return "EM_GETSEL";
    case 177: return "EM_SETSEL";
    case 178: return "EM_GETRECT";
    case 179: return "EM_SETRECT";
    case 180: return "EM_SETRECTNP";
    case 181: return "EM_SCROLL";
    case 182: return "EM_LINESCROLL";
    case 183: return "EM_SCROLLCARET";
    case 185: return "EM_GETMODIFY";
    case 187: return "EM_SETMODIFY";
    case 188: return "EM_GETLINECOUNT";
    case 189: return "EM_LINEINDEX";
    case 190: return "EM_SETHANDLE";
    case 191: return "EM_GETHANDLE";
    case 192: return "EM_GETTHUMB";
    case 193: return "EM_LINELENGTH";
    case 194: return "EM_REPLACESEL";
    case 195: return "EM_SETFONT";
    case 196: return "EM_GETLINE";
    case 197: return "EM_LIMITTEXT / EM_SETLIMITTEXT";
    case 198: return "EM_CANUNDO";
    case 199: return "EM_UNDO";
    case 200: return "EM_FMTLINES";
    case 201: return "EM_LINEFROMCHAR";
    case 202: return "EM_SETWORDBREAK";
    case 203: return "EM_SETTABSTOPS";
    case 204: return "EM_SETPASSWORDCHAR";
    case 205: return "EM_EMPTYUNDOBUFFER";
    case 206: return "EM_GETFIRSTVISIBLELINE";
    case 207: return "EM_SETREADONLY";
    case 209: return "EM_SETWORDBREAKPROC / EM_GETWORDBREAKPROC";
    case 210: return "EM_GETPASSWORDCHAR";
    case 211: return "EM_SETMARGINS";
    case 212: return "EM_GETMARGINS";
    case 213: return "EM_GETLIMITTEXT";
    case 214: return "EM_POSFROMCHAR";
    case 215: return "EM_CHARFROMPOS";
    case 216: return "EM_SETIMESTATUS";
    case 217: return "EM_GETIMESTATUS";
    case 224: return "SBM_SETPOS";
    case 225: return "SBM_GETPOS";
    case 226: return "SBM_SETRANGE";
    case 227: return "SBM_GETRANGE";
    case 228: return "SBM_ENABLE_ARROWS";
    case 230: return "SBM_SETRANGEREDRAW";
    case 233: return "SBM_SETSCROLLINFO";
    case 234: return "SBM_GETSCROLLINFO";
    case 235: return "SBM_GETSCROLLBARINFO";
    case 240: return "BM_GETCHECK";
    case 241: return "BM_SETCHECK";
    case 242: return "BM_GETSTATE";
    case 243: return "BM_SETSTATE";
    case 244: return "BM_SETSTYLE";
    case 245: return "BM_CLICK";
    case 246: return "BM_GETIMAGE";
    case 247: return "BM_SETIMAGE";
    case 248: return "BM_SETDONTCLICK";
    case 255: return "WM_INPUT";
    case 256: return "WM_KEYDOWN";
    case 257: return "WM_KEYUP";
    case 258: return "WM_CHAR";
    case 259: return "WM_DEADCHAR";
    case 260: return "WM_SYSKEYDOWN";
    case 261: return "WM_SYSKEYUP";
    case 262: return "WM_SYSCHAR";
    case 263: return "WM_SYSDEADCHAR";
    case 265: return "WM_UNICHAR / WM_WNT_CONVERTREQUESTEX";
    case 266: return "WM_CONVERTREQUEST";
    case 267: return "WM_CONVERTRESULT";
    case 268: return "WM_INTERIM";
    case 269: return "WM_IME_STARTCOMPOSITION";
    case 270: return "WM_IME_ENDCOMPOSITION";
    case 272: return "WM_INITDIALOG";
    case 273: return "WM_COMMAND";
    case 274: return "WM_SYSCOMMAND";
    case 275: return "WM_TIMER";
    case 276: return "WM_HSCROLL";
    case 277: return "WM_VSCROLL";
    case 278: return "WM_INITMENU";
    case 279: return "WM_INITMENUPOPUP";
    case 280: return "WM_SYSTIMER";
    case 287: return "WM_MENUSELECT";
    case 288: return "WM_MENUCHAR";
    case 289: return "WM_ENTERIDLE";
    case 290: return "WM_MENURBUTTONUP";
    case 291: return "WM_MENUDRAG";
    case 292: return "WM_MENUGETOBJECT";
    case 293: return "WM_UNINITMENUPOPUP";
    case 294: return "WM_MENUCOMMAND";
    case 295: return "WM_CHANGEUISTATE";
    case 296: return "WM_UPDATEUISTATE";
    case 297: return "WM_QUERYUISTATE";
    case 306: return "WM_CTLCOLORMSGBOX";
    case 307: return "WM_CTLCOLOREDIT";
    case 308: return "WM_CTLCOLORLISTBOX";
    case 309: return "WM_CTLCOLORBTN";
    case 310: return "WM_CTLCOLORDLG";
    case 311: return "WM_CTLCOLORSCROLLBAR";
    case 312: return "WM_CTLCOLORSTATIC";
    case 512: return "WM_MOUSEMOVE";
    case 513: return "WM_LBUTTONDOWN";
    case 514: return "WM_LBUTTONUP";
    case 515: return "WM_LBUTTONDBLCLK";
    case 516: return "WM_RBUTTONDOWN";
    case 517: return "WM_RBUTTONUP";
    case 518: return "WM_RBUTTONDBLCLK";
    case 519: return "WM_MBUTTONDOWN";
    case 520: return "WM_MBUTTONUP";
    case 521: return "WM_MBUTTONDBLCLK";
    case 522: return "WM_MOUSEWHEEL";
    case 523: return "WM_XBUTTONDOWN";
    case 524: return "WM_XBUTTONUP";
    case 525: return "WM_XBUTTONDBLCLK";
    case 528: return "WM_PARENTNOTIFY";
    case 529: return "WM_ENTERMENULOOP";
    case 530: return "WM_EXITMENULOOP";
    case 531: return "WM_NEXTMENU";
    case 532: return "WM_SIZING";
    case 533: return "WM_CAPTURECHANGED";
    case 534: return "WM_MOVING";
    case 536: return "WM_POWERBROADCAST";
    case 537: return "WM_DEVICECHANGE";
    case 544: return "WM_MDICREATE";
    case 545: return "WM_MDIDESTROY";
    case 546: return "WM_MDIACTIVATE";
    case 547: return "WM_MDIRESTORE";
    case 548: return "WM_MDINEXT";
    case 549: return "WM_MDIMAXIMIZE";
    case 550: return "WM_MDITILE";
    case 551: return "WM_MDICASCADE";
    case 552: return "WM_MDIICONARRANGE";
    case 553: return "WM_MDIGETACTIVE";
    case 560: return "WM_MDISETMENU";
    case 561: return "WM_ENTERSIZEMOVE";
    case 562: return "WM_EXITSIZEMOVE";
    case 563: return "WM_DROPFILES";
    case 564: return "WM_MDIREFRESHMENU";
    case 640: return "WM_IME_REPORT";
    case 641: return "WM_IME_SETCONTEXT";
    case 642: return "WM_IME_NOTIFY";
    case 643: return "WM_IME_CONTROL";
    case 644: return "WM_IME_COMPOSITIONFULL";
    case 645: return "WM_IME_SELECT";
    case 646: return "WM_IME_CHAR";
    case 648: return "WM_IME_REQUEST";
    case 656: return "WM_IME_KEYDOWN";
    case 657: return "WM_IME_KEYUP";
    case 672: return "WM_NCMOUSEHOVER";
    case 673: return "WM_MOUSEHOVER";
    case 674: return "WM_NCMOUSELEAVE";
    case 675: return "WM_MOUSELEAVE";
    case 768: return "WM_CUT";
    case 769: return "WM_COPY";
    case 770: return "WM_PASTE";
    case 771: return "WM_CLEAR";
    case 772: return "WM_UNDO";
    case 773: return "WM_RENDERFORMAT";
    case 774: return "WM_RENDERALLFORMATS";
    case 775: return "WM_DESTROYCLIPBOARD";
    case 776: return "WM_DRAWCLIPBOARD";
    case 777: return "WM_PAINTCLIPBOARD";
    case 778: return "WM_VSCROLLCLIPBOARD";
    case 779: return "WM_SIZECLIPBOARD";
    case 780: return "WM_ASKCBFORMATNAME";
    case 781: return "WM_CHANGECBCHAIN";
    case 782: return "WM_HSCROLLCLIPBOARD";
    case 783: return "WM_QUERYNEWPALETTE";
    case 784: return "WM_PALETTEISCHANGING";
    case 785: return "WM_PALETTECHANGED";
    case 786: return "WM_HOTKEY";
    case 791: return "WM_PRINT";
    case 792: return "WM_PRINTCLIENT";
    case 793: return "WM_APPCOMMAND";
    case 856: return "WM_HANDHELDFIRST";
    case 863: return "WM_HANDHELDLAST";
    case 864: return "WM_AFXFIRST";
    case 895: return "WM_AFXLAST";
    case 896: return "WM_PENWINFIRST";
    case 897: return "WM_RCRESULT";
    case 898: return "WM_HOOKRCRESULT";
    case 899: return "WM_GLOBALRCCHANGE / WM_PENMISCINFO";
    case 900: return "WM_SKB";
    case 901: return "WM_HEDITCTL / WM_PENCTL";
    case 902: return "WM_PENMISC";
    case 903: return "WM_CTLINIT";
    case 904: return "WM_PENEVENT";
    case 911: return "WM_PENWINLAST";
    default:
    	return "unknown WM_ message";
	}

	return "unknown WM_ message";
}
#endif
#endif


static unsigned int zOrderCounterFloating = 0;
/**
 * Private data class of CFloatingDockContainer class (pimpl)
 */
struct FloatingDockContainerPrivate
{
	CFloatingDockContainer *_this;
	CDockContainerWidget *DockContainer;
	unsigned int zOrderIndex = ++zOrderCounterFloating;
	QPointer<CDockManager> DockManager;
	eDragState DraggingState = DraggingInactive;
	QPoint DragStartMousePosition;
	CDockContainerWidget *DropContainer = nullptr;
	CDockAreaWidget *SingleDockArea = nullptr;
	QPoint DragStartPos;
	bool Hiding = false;
	bool AutoHideChildren = true;
	bool HideContentOnNextHide = false;
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    QWidget* MouseEventHandler = nullptr;
    CFloatingWidgetTitleBar* TitleBar = nullptr;
	bool IsResizing = false;
    bool MousePressed = false;
#endif

	/**
	 * Private data constructor
	 */
	FloatingDockContainerPrivate(CFloatingDockContainer *_public);

	void titleMouseReleaseEvent();
	void updateDropOverlays(const QPoint &GlobalPos);

	/**
	 * Returns true if the given config flag is set
	 */
	static bool testConfigFlag(CDockManager::eConfigFlag Flag)
	{
		return CDockManager::testConfigFlag(Flag);
	}

	/**
	 * Tests is a certain state is active
	 */
	bool isState(eDragState StateId) const
	{
		return StateId == DraggingState;
	}

	/**
	 * Sets the dragging state and posts a FloatingWidgetDragStartEvent
	 * if dragging starts
	 */
	void setState(eDragState StateId)
	{
		if (DraggingState == StateId)
		{
			return;
		}

		DraggingState = StateId;
        if (DraggingFloatingWidget == DraggingState)
        {
            qApp->postEvent(_this, new QEvent((QEvent::Type)internal::FloatingWidgetDragStartEvent));
        }
	}

	void setWindowTitle(const QString &Text)
	{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
		if (TitleBar)
		{
			TitleBar->setTitle(Text);
		}
#endif
		_this->setWindowTitle(Text);
	}

	/**
	 * Reflect the current dock widget title in the floating widget windowTitle()
	 * depending on the CDockManager::FloatingContainerHasWidgetTitle flag
	 */
	void reflectCurrentWidget(CDockWidget* CurrentWidget)
	{
		// reflect CurrentWidget's title if configured to do so, otherwise display application name as window title
		if (testConfigFlag(CDockManager::FloatingContainerHasWidgetTitle))
		{
			setWindowTitle(CurrentWidget->windowTitle());
		}
		else
		{
			setWindowTitle(floatingContainersTitle());
		}

		// reflect CurrentWidget's icon if configured to do so, otherwise display application icon as window icon
		QIcon CurrentWidgetIcon = CurrentWidget->icon();
		if (testConfigFlag(CDockManager::FloatingContainerHasWidgetIcon)
				&& !CurrentWidgetIcon.isNull())
		{
			_this->setWindowIcon(CurrentWidget->icon());
		}
		else
		{
			_this->setWindowIcon(CurrentWidget->windowIcon());
		}
	}

	/**
	 * Handles escape key press when dragging around the floating widget
	 */
	void handleEscapeKey();

	/**
	 * Returns the title used by all FloatingContainer that does not
	 * reflect the title of the current dock widget.
	 *
	 * If not title was set with CDockManager::setFloatingContainersTitle(),
	 * it returns QGuiApplication::applicationDisplayName().
	 */
	static QString floatingContainersTitle()
	{
		return CDockManager::floatingContainersTitle();
	}
};
// struct FloatingDockContainerPrivate

//============================================================================
FloatingDockContainerPrivate::FloatingDockContainerPrivate(
    CFloatingDockContainer *_public) :
	_this(_public)
{

}

//============================================================================
void FloatingDockContainerPrivate::titleMouseReleaseEvent()
{
	setState(DraggingInactive);
	if (!DropContainer)
	{
		return;
	}

	// DockManager will be unlinked from this within DropContainer->dropFloatingWidget
	const auto OriginalDockManager = this->DockManager.data();

	if (DockManager->dockAreaOverlay()->dropAreaUnderCursor() != InvalidDockWidgetArea
	 || DockManager->containerOverlay()->dropAreaUnderCursor() != InvalidDockWidgetArea)
	{
		CDockOverlay *Overlay = DockManager->containerOverlay();
		if (!Overlay->dropOverlayRect().isValid())
		{
			Overlay = DockManager->dockAreaOverlay();
		}

		// Do not resize if we drop into an autohide sidebar area to preserve
		// the dock area size for the initial size of the auto hide area
		if (!ads::internal::isSideBarArea(Overlay->dropAreaUnderCursor()))
		{
			// Resize the floating widget to the size of the highlighted drop area
			// rectangle
			QRect Rect = Overlay->dropOverlayRect();
			int FrameWidth = (_this->frameSize().width() - _this->rect().width())
				/ 2;
			int TitleBarHeight = _this->frameSize().height()
				- _this->rect().height() - FrameWidth;
			if (Rect.isValid())
			{
				QPoint TopLeft = Overlay->mapToGlobal(Rect.topLeft());
				TopLeft.ry() += TitleBarHeight;
				_this->setGeometry(
					QRect(TopLeft,
						QSize(Rect.width(), Rect.height() - TitleBarHeight)));
				QApplication::processEvents();
			}
		}
		DropContainer->dropFloatingWidget(_this, QCursor::pos());
	}

	OriginalDockManager->containerOverlay()->hideOverlay();
	OriginalDockManager->dockAreaOverlay()->hideOverlay();
}


//============================================================================
void FloatingDockContainerPrivate::updateDropOverlays(const QPoint &GlobalPos)
{
	if (!_this->isVisible() || !DockManager)
	{
		return;
	}

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
	// Prevent display of drop overlays and docking as long as a model dialog
	// is active
    if (qApp->activeModalWidget())
    {
        return;
    }
#endif

	auto Containers = DockManager->dockContainers();
	CDockContainerWidget *TopContainer = nullptr;
	for (auto ContainerWidget : Containers)
	{
		if (!ContainerWidget->isVisible())
		{
			continue;
		}

		if (DockContainer == ContainerWidget)
		{
			continue;
		}

		QPoint MappedPos = ContainerWidget->mapFromGlobal(GlobalPos);
		if (ContainerWidget->rect().contains(MappedPos))
		{
			if (!TopContainer || ContainerWidget->isInFrontOf(TopContainer))
			{
				TopContainer = ContainerWidget;
			}
		}
	}

	DropContainer = TopContainer;
	if (!TopContainer)
	{
		DockManager->containerOverlay()->hideOverlay();
		DockManager->dockAreaOverlay()->hideOverlay();
		return;
	}

	CDockContainerWidget::showDropOverlays(DockManager, TopContainer, GlobalPos,
		DockContainer->features().testFlag(CDockWidget::DockWidgetPinnable));
}


//============================================================================
void FloatingDockContainerPrivate::handleEscapeKey()
{
	ADS_PRINT("FloatingDockContainerPrivate::handleEscapeKey()");
	setState(DraggingInactive);
	DockManager->containerOverlay()->hideOverlay();
	DockManager->dockAreaOverlay()->hideOverlay();
}


//============================================================================
// Wayland: a floating widget that is a child of the dock manager makes Qt
// turn the dock manager and all of its child widgets into native windows
// when the floating widget is shown. Native child widgets are
// wl_subsurfaces that take part in the compositors drag and drop target
// picking and steal the drop focus from the dock container during a
// platform drag. The dock manager deletes the registered floating widgets
// in its destructor, so the floating widget does not need a parent for
// memory management.
CFloatingDockContainer::CFloatingDockContainer(CDockManager *DockManager) :
	tFloatingWidgetBase(internal::isWayland() ? nullptr : static_cast<QWidget*>(DockManager)),
	d(new FloatingDockContainerPrivate(this))
{
	d->DockManager = DockManager;
	d->DockContainer = new CDockContainerWidget(DockManager, this);
	connect(d->DockContainer, SIGNAL(dockAreasAdded()), this,
	    SLOT(onDockAreasAddedOrRemoved()));
	connect(d->DockContainer, SIGNAL(dockAreasRemoved()), this,
	    SLOT(onDockAreasAddedOrRemoved()));

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
	QDockWidget::setWidget(d->DockContainer);
	QDockWidget::setFeatures(QDockWidget::DockWidgetClosable
		| QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);

	bool native_window = true;

	// FloatingContainerForce*TitleBar is overwritten by the "ADS_UseNativeTitle" environment variable if set.
	auto env = qgetenv("ADS_UseNativeTitle").toUpper();
	if (env == "1")
	{
		native_window = true;
	}
	else if (env == "0")
	{
		native_window = false;
	}
	else if (DockManager->testConfigFlag(CDockManager::FloatingContainerForceNativeTitleBar))
	{
		native_window = true;
	}
	else if (DockManager->testConfigFlag(CDockManager::FloatingContainerForceQWidgetTitleBar))
	{
		native_window = false;
	}
	else
	{
		// KDE doesn't seem to fire MoveEvents while moving windows, so for now no native titlebar for everything using KWin.
		QString window_manager = internal::windowManager().toUpper().split(" ")[0];
                native_window = window_manager != "KWIN";
	}

	// Wayland does not allow clients to move windows, so the custom title
	// bar cannot work there - dragging it would not move the window. Always
	// use a native window so that the user can move the floating widget
	// via the window decoration provided by the compositor or by Qt.
	if (internal::isWayland())
	{
		native_window = true;
	}

	if (native_window)
	{
		setTitleBarWidget(new QWidget());
		setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
	}
	else
	{
		d->TitleBar = new CFloatingWidgetTitleBar(this);
		setTitleBarWidget(d->TitleBar);
		setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint);
		d->TitleBar->enableCloseButton(isClosable());
		connect(d->TitleBar, SIGNAL(closeRequested()), SLOT(close()));
		connect(d->TitleBar, &CFloatingWidgetTitleBar::maximizeRequested,
				this, &CFloatingDockContainer::onMaximizeRequest);
	}
#else
	setWindowFlags(
	    Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
	QBoxLayout *l = new QBoxLayout(QBoxLayout::TopToBottom);
	l->setContentsMargins(0, 0, 0, 0);
	l->setSpacing(0);
	setLayout(l);
	l->addWidget(d->DockContainer);
#endif

	if (CDockManager::testConfigFlag(CDockManager::UseNativeWindows))
	{
		winId();
	}

	// Wayland: the floating widget has no parent widget (see above), so it
	// does not inherit the dock manager's effective style sheet through the
	// widget hierarchy. Apply the style sheets along the dock manager parent
	// chain explicitly, so the floating widget looks like the docked content.
	// CDockManager::changeEvent() keeps this up to date when the style sheet
	// changes while the widget is already floating.
	if (internal::isWayland())
	{
		const QString StyleSheet = waylandInheritedStyleSheet(DockManager);
		if (!StyleSheet.isEmpty())
		{
			setStyleSheet(StyleSheet);
		}
	}

	DockManager->registerFloatingWidget(this);
}

//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockAreaWidget *DockArea) :
	CFloatingDockContainer(DockArea->dockManager())
{
	d->DockContainer->addDockArea(DockArea);

    auto TopLevelDockWidget = topLevelDockWidget();
    if (TopLevelDockWidget)
    {
    	TopLevelDockWidget->emitTopLevelChanged(true);
    }

    d->DockManager->notifyWidgetOrAreaRelocation(DockArea);
}

//============================================================================
CFloatingDockContainer::CFloatingDockContainer(CDockWidget *DockWidget) :
	CFloatingDockContainer(DockWidget->dockManager())
{
	d->DockContainer->addDockWidget(CenterDockWidgetArea, DockWidget);
    auto TopLevelDockWidget = topLevelDockWidget();
    if (TopLevelDockWidget)
    {
    	TopLevelDockWidget->emitTopLevelChanged(true);
    }

    d->DockManager->notifyWidgetOrAreaRelocation(DockWidget);
}


//============================================================================
CFloatingDockContainer::~CFloatingDockContainer()
{
	ADS_PRINT("~CFloatingDockContainer");
	if (d->DockManager)
	{
		d->DockManager->removeFloatingWidget(this);
	}
	delete d;
}


//============================================================================
void CFloatingDockContainer::deleteContent()
{
	std::vector<QPointer<ads::CDockAreaWidget>> areas;
	for (int i = 0; i != dockContainer()->dockAreaCount(); ++i)
	{
		areas.push_back( dockContainer()->dockArea(i) );
	}
	for (auto area : areas)
	{
		if (!area)
		{
			continue;
		}

		// QPointer delete safety - just in case some dock widget in destruction
		// deletes another related/twin or child dock widget.
		std::vector<QPointer<QWidget>> deleteWidgets;
		for (auto widget : area->dockWidgets())
		{
			deleteWidgets.push_back(widget);
		}
		for (auto ptrWdg : deleteWidgets)
		{
			delete ptrWdg;
		}
	}
}

//============================================================================
CDockContainerWidget* CFloatingDockContainer::dockContainer() const
{
	return d->DockContainer;
}

//============================================================================
bool CFloatingDockContainer::isDraggingActive() const
{
	return d->isState(DraggingFloatingWidget);
}

//============================================================================
void CFloatingDockContainer::changeEvent(QEvent *event)
{
	Super::changeEvent(event);
	switch (event->type())
	{
	case QEvent::ActivationChange:
		if (isActiveWindow())
		{
			ADS_PRINT("FloatingWidget::changeEvent QEvent::ActivationChange ");
			d->zOrderIndex = ++zOrderCounterFloating;

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
			if (d->DraggingState == DraggingFloatingWidget)
			{
				d->titleMouseReleaseEvent();
				d->DraggingState = DraggingInactive;
			}
#endif
		}
		break;

	case QEvent::WindowStateChange:
	    // If the DockManager window is restored from minimized on Windows
		// then the FloatingWidgets are not properly restored to maximized but
		// to normal state.
		// We simply check here, if the FloatingWidget was maximized before
		// and if the DockManager is just leaving the minimized state. In this
		// case, we restore the maximized state of this floating widget
		if (d->DockManager->isLeavingMinimizedState())
		{
			QWindowStateChangeEvent* ev = static_cast<QWindowStateChangeEvent*>(event);
			if (ev->oldState().testFlag(Qt::WindowMaximized))
			{
				this->showMaximized();
			}
		}
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
        if (d->TitleBar)
        {
            d->TitleBar->setVisible(!(isFloating() && isFullScreen()));
        }
#endif
		break;

	default:
		break; // do nothing
	}
}


#ifdef Q_OS_WIN
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
bool CFloatingDockContainer::nativeEvent(const QByteArray &eventType, void *message, long *result)
#else
bool CFloatingDockContainer::nativeEvent(const QByteArray &eventType, void *message, qintptr *result)
#endif
{
	QWidget::nativeEvent(eventType, message, result);
	MSG *msg = static_cast<MSG*>(message);
	switch (msg->message)
	{
		case WM_MOVING:
		{
			if (d->isState(DraggingFloatingWidget))
			{
				d->updateDropOverlays(QCursor::pos());
			}
		}
		break;

		case WM_NCLBUTTONDOWN:
			 if (msg->wParam == HTCAPTION && d->isState(DraggingInactive))
			 {
				ADS_PRINT("CFloatingDockContainer::nativeEvent WM_NCLBUTTONDOWN");
				d->DragStartPos = pos();
				d->setState(DraggingMousePressed);
			 }
			 break;

		case WM_NCLBUTTONDBLCLK:
			 d->setState(DraggingInactive);
			 break;

		case WM_ENTERSIZEMOVE:
			 if (d->isState(DraggingMousePressed))
			 {
				ADS_PRINT("CFloatingDockContainer::nativeEvent WM_ENTERSIZEMOVE");
				d->setState(DraggingFloatingWidget);
				d->updateDropOverlays(QCursor::pos());
			 }
			 break;

		case WM_EXITSIZEMOVE:
			 if (d->isState(DraggingFloatingWidget))
			 {
				ADS_PRINT("CFloatingDockContainer::nativeEvent WM_EXITSIZEMOVE");
				if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
				{
					d->handleEscapeKey();
				}
				else
				{
					d->titleMouseReleaseEvent();
				}
			 }
			 break;
	}
	return false;
}
#endif


//============================================================================
void CFloatingDockContainer::closeEvent(QCloseEvent *event)
{
	ADS_PRINT("CFloatingDockContainer closeEvent");
	d->setState(DraggingInactive);
	event->ignore();
	if (!isClosable())
	{
		return;
	}

	bool HasOpenDockWidgets = false;
	for (auto DockWidget : d->DockContainer->openedDockWidgets())
	{
		if (DockWidget->features().testFlag(CDockWidget::DockWidgetDeleteOnClose) || DockWidget->features().testFlag(CDockWidget::CustomCloseHandling))
		{
			bool Closed = DockWidget->closeDockWidgetInternal();
			if (!Closed)
			{
				HasOpenDockWidgets = true;
			}
		}
		else
		{
			DockWidget->toggleView(false);
		}
	}

	if (HasOpenDockWidgets)
	{
		return;
	}

	// New bug (QWebEngineView reload side effect):
	// when a WebEngine-based dock is tabified into a floating container, the
	// embedded native/web process can trigger delayed hide/show cycles on the
	// floating window. If every non-spontaneous hide propagates to
	// DockWidget->toggleView(false), unrelated tabs are marked closed and seem
	// to "disappear". We therefore arm HideContentOnNextHide only for the 
	// explicit close path.
	d->HideContentOnNextHide = true;

	// In Qt version after 5.9.2 there seems to be a bug that causes the
	// QWidget::event() function to not receive any NonClientArea mouse
	// events anymore after a close/show cycle. The bug is reported here:
	// https://bugreports.qt.io/browse/QTBUG-73295
	// The following code is a workaround for Qt versions > 5.9.2 that seems
	// to work
	// Starting from Qt version 5.12.2 this seems to work again. But
	// now the QEvent::NonClientAreaMouseButtonPress function returns always
	// Qt::RightButton even if the left button was pressed
	this->hide();
}

//============================================================================
void CFloatingDockContainer::hideEvent(QHideEvent *event)
{
	Super::hideEvent(event);
    if (event->spontaneous())
    {
        return;
    }

    // Prevent toogleView() events during restore state
    if (d->DockManager->isRestoringState())
    {
        return;
    }

	// Only a close operation should propagate hide->toggleView(false) to
	// child dock widgets. Generic hide/show cycles (e.g. from platform or
	// embedded native content) must not change dock open/closed state.
	if (!d->HideContentOnNextHide)
	{
		return;
	}
	d->HideContentOnNextHide = false;

	if ( d->AutoHideChildren )
	{
		d->Hiding = true;
		for ( auto DockArea : d->DockContainer->openedDockAreas() )
		{
			for ( auto DockWidget : DockArea->openedDockWidgets() )
			{
				DockWidget->toggleView( false );
			}
		}
		d->Hiding = false;
	}
}


//============================================================================
void CFloatingDockContainer::showEvent(QShowEvent *event)
{
	Super::showEvent(event);
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    if (CDockManager::testConfigFlag(CDockManager::FocusHighlighting))
    {
        this->window()->activateWindow();
    }
#endif
}


//============================================================================
void CFloatingDockContainer::startFloating(const QPoint &DragStartMousePos,
    const QSize &Size, eDragState DragState, QWidget *MouseEventHandler)
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
    if (!isMaximized())
    {
		resize(Size);
		d->DragStartMousePosition = DragStartMousePos;
    }
	d->setState(DragState);
	if (DraggingFloatingWidget == DragState)
	{
		d->MouseEventHandler = MouseEventHandler;
		if (d->MouseEventHandler)
		{
			d->MouseEventHandler->grabMouse();
		}
	}

	if (!isMaximized())
	{
		moveFloating();
	}
	show();
#else
    Q_UNUSED(MouseEventHandler)
	resize(Size);
	d->DragStartMousePosition = DragStartMousePos;
	d->setState(DragState);
	moveFloating();
	show();
#endif
}

//============================================================================
void CFloatingDockContainer::moveFloating()
{
	int BorderSize = (frameSize().width() - size().width()) / 2;
	const QPoint moveToPos = QCursor::pos() - d->DragStartMousePosition
	    - QPoint(BorderSize, 0);
	move(moveToPos);
	switch (d->DraggingState)
	{
	case DraggingMousePressed:
		d->setState(DraggingFloatingWidget);
		d->updateDropOverlays(QCursor::pos());
		break;

	case DraggingFloatingWidget:
		d->updateDropOverlays(QCursor::pos());
#ifdef Q_OS_MACOS
		// In OSX when hiding the DockAreaOverlay the application would set
		// the main window as the active window for some reason. This fixes
		// that by resetting the active window to the floating widget after
		// updating the overlays.
		activateWindow();
#endif
		break;
	default:
		break;
	}
}

//============================================================================
bool CFloatingDockContainer::isClosable() const
{
	return d->DockContainer->features().testFlag(
	    CDockWidget::DockWidgetClosable);
}

//============================================================================
void CFloatingDockContainer::onDockAreasAddedOrRemoved()
{
	ADS_PRINT("CFloatingDockContainer::onDockAreasAddedOrRemoved()");
	auto TopLevelDockArea = d->DockContainer->topLevelDockArea();
	if (TopLevelDockArea)
	{
		d->SingleDockArea = TopLevelDockArea;
		CDockWidget* CurrentWidget = d->SingleDockArea->currentDockWidget();
		d->reflectCurrentWidget(CurrentWidget);
		connect(d->SingleDockArea, SIGNAL(currentChanged(int)), this,
		    SLOT(onDockAreaCurrentChanged(int)));
	}
	else
	{
		if (d->SingleDockArea)
		{
			disconnect(d->SingleDockArea, SIGNAL(currentChanged(int)), this,
			    SLOT(onDockAreaCurrentChanged(int)));
			d->SingleDockArea = nullptr;
		}
		d->setWindowTitle(d->floatingContainersTitle());
		setWindowIcon(QApplication::windowIcon());
	}
}

//============================================================================
void CFloatingDockContainer::updateWindowTitle()
{
	// If this floating container will be hidden, then updating the window
	// tile is not required anymore
	if (d->Hiding)
	{
		return;
	}


	auto TopLevelDockArea = d->DockContainer->topLevelDockArea();
	if (TopLevelDockArea)
	{
		CDockWidget* CurrentWidget = TopLevelDockArea->currentDockWidget();
		if (CurrentWidget)
		{
			d->reflectCurrentWidget(CurrentWidget);
		}
	}
	else
	{
		d->setWindowTitle(d->floatingContainersTitle());
		setWindowIcon(QApplication::windowIcon());
	}
}

//============================================================================
void CFloatingDockContainer::onDockAreaCurrentChanged(int Index)
{
	Q_UNUSED(Index);
	CDockWidget* CurrentWidget = d->SingleDockArea->currentDockWidget();
	d->reflectCurrentWidget(CurrentWidget);
}

//============================================================================
bool CFloatingDockContainer::restoreState(CDockingStateReader &Stream,
    bool Testing)
{
	if (!d->DockContainer->restoreState(Stream, Testing))
	{
		return false;
	}
	onDockAreasAddedOrRemoved();
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
	if(d->TitleBar)
	{
		d->TitleBar->setMaximizedIcon(windowState() == Qt::WindowMaximized);
	}
#endif
	return true;
}


//============================================================================
bool CFloatingDockContainer::hasTopLevelDockWidget() const
{
	return d->DockContainer->hasTopLevelDockWidget();
}

//============================================================================
CDockWidget* CFloatingDockContainer::topLevelDockWidget() const
{
	return d->DockContainer->topLevelDockWidget();
}

//============================================================================
QList<CDockWidget*> CFloatingDockContainer::dockWidgets() const
{
	return d->DockContainer->dockWidgets();
}

//============================================================================
void CFloatingDockContainer::finishDropOperation()
{
	// Widget has been redocked, so it must be hidden right way (see 
	// https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System/issues/351)
	// but AutoHideChildren must be set to false because "this" still contains
	// dock widgets that shall not be toggled hidden.
	d->AutoHideChildren = false;
	hide();
	// The floating widget will be deleted now. Ensure, that the destructor
	// of the floating widget does not delete any dock areas that have been
	// moved to a new container - simply remove all dock areas before deleting
	// the floating widget
	d->DockContainer->removeAllDockAreas();
	deleteLater();
	if (d->DockManager)
	{
		d->DockManager->removeFloatingWidget(this);
		d->DockManager.clear();
	}
	this->dockContainer()->removeFromDockManager();
}

//============================================================================
void CFloatingDockContainer::finishDragging()
{
	ADS_PRINT("CFloatingDockContainer::finishDragging");
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
	setWindowOpacity(1);
	activateWindow();
	if (d->MouseEventHandler)
	{
	   d->MouseEventHandler->releaseMouse();
	   d->MouseEventHandler = nullptr;
	}
#endif
	d->titleMouseReleaseEvent();
}


//============================================================================
// MIME type that transports the CFloatingDockContainer pointer between the
// drag source and the CDockContainerWidget drop targets within this
// application
static const char* const FloatingWidgetMimeType = "application/x-ads-floating-dock-container";

// MIME types that QWaylandDataDevice translates into an
// xdg_toplevel_drag_v1 request so that the compositor attaches the floating
// widget window to the drag cursor. The format matches
// QMainWindowLayout::performPlatformWidgetDrag() in qtbase
static const char* const PlatformDragWindowMimeType = "application/x-qt-mainwindowdrag-window";
static const char* const PlatformDragPositionMimeType = "application/x-qt-mainwindowdrag-position";

// Maximum age of the recorded drop candidate that startPlatformDrag() still
// accepts as a drop. It must be longer than the interval between drag move
// events (so a drop right after the last move counts) but short enough to
// reject a release far away from the last drop area
static const int PlatformDragDropCandidateTimeoutMs = 400;


// Records the last drop candidate during a Wayland platform drag. Some
// compositors do not deliver a drop event when the dragged window overlaps
// the drop target (they send a leave event instead), so startPlatformDrag()
// falls back to this candidate when no drop event was delivered.
struct PlatformDragDropCandidate
{
	QPointer<CDockContainerWidget> Container;
	QPoint GlobalPos;
	bool ValidDropArea = false;
	bool DropHandled = false;
	QElapsedTimer SinceLastUpdate;
};
static PlatformDragDropCandidate s_PlatformDragDropCandidate;


//============================================================================
void CFloatingDockContainer::platformDragUpdateDropCandidate(
	CDockContainerWidget* Container, const QPoint& GlobalPos, bool ValidDropArea)
{
	s_PlatformDragDropCandidate.Container = Container;
	s_PlatformDragDropCandidate.GlobalPos = GlobalPos;
	s_PlatformDragDropCandidate.ValidDropArea = ValidDropArea;
	s_PlatformDragDropCandidate.SinceLastUpdate.start();
}


//============================================================================
void CFloatingDockContainer::platformDragNotifyDropHandled()
{
	s_PlatformDragDropCandidate.DropHandled = true;
}


//============================================================================
QString CFloatingDockContainer::waylandInheritedStyleSheet(QWidget* DockManager)
{
	QString StyleSheet;
	for (QWidget* Widget = DockManager; Widget; Widget = Widget->parentWidget())
	{
		const QString WidgetStyleSheet = Widget->styleSheet();
		if (!WidgetStyleSheet.isEmpty())
		{
			StyleSheet = WidgetStyleSheet + QLatin1Char('\n') + StyleSheet;
		}
	}
	return StyleSheet;
}


//============================================================================
Qt::DropAction CFloatingDockContainer::startPlatformDrag(
	CFloatingDockContainer* FloatingWidget, const QPoint& GlobalPressPos,
	QWidget* DragSource, const QPoint* DragOffset)
{
	auto serialize = [](const auto &object)
	{
		QByteArray Data;
		QDataStream DataStream(&Data, QIODevice::WriteOnly);
		DataStream << object;
		return Data;
	};

	// xdg_toplevel_drag_v1: the attach offset that places the new window under
	// the cursor is only honoured by the compositor while the toplevel is still
	// UNMAPPED. The protocol says the client "issues a xdg_toplevel_drag_v1.attach
	// request before mapping it", and the offset "might only be used when an
	// unmapped window is attached". Qt's QWaylandDataDevice::startDrag() performs
	// the attach when the data drag starts, reading the xdg_toplevel surface role
	// off the window handle.
	//
	// The caller already show()'d the floating widget, which creates the
	// xdg_toplevel role, but the surface is only mapped (a buffer committed)
	// once the event loop is pumped. We therefore deliberately do NOT wait for
	// exposure here: pumping events would map the window before exec() runs the
	// attach, after which the compositor ignores the offset and leaves the
	// window at its default position (typically screen center). Proceeding
	// straight to exec() keeps the toplevel created-but-unmapped so the attach
	// offset positions it under the cursor.

	// The offset is the press position relative to the floating widget top
	// left corner, so the compositor keeps the cursor at the same spot on
	// the window during the drag. For a freshly created floating window the
	// caller supplies it explicitly (DragOffset): on Wayland the new window has
	// no meaningful geometry yet, so mapFromGlobal() would give a wrong/random
	// offset and the compositor would place the window randomly.
	QPoint DragStartOffset;
	if (DragOffset)
	{
		DragStartOffset = *DragOffset;
		// The new floating window is a native window on Wayland (forced in the
		// constructor), so it has a title bar / decoration around the content -
		// drawn either by the compositor or by Qt's client-side decoration.
		// The supplied offset is relative to the source tab/area content, so
		// shift it by the window frame's top and left margins to keep the
		// grabbed content point - not the decoration - under the cursor. With
		// server-side decorations the margins are 0 and no shift is applied.
		if (FloatingWidget->windowHandle())
		{
			const QMargins Frame = FloatingWidget->windowHandle()->frameMargins();
			DragStartOffset.rx() += Frame.left();
			DragStartOffset.ry() += Frame.top();
		}
	}
	else
	{
		DragStartOffset = FloatingWidget->mapFromGlobal(GlobalPressPos);
	}

	auto Drag = new QDrag(DragSource);
	auto MimeData = new QMimeData();
	MimeData->setData(QLatin1String(PlatformDragWindowMimeType),
		serialize(reinterpret_cast<qintptr>(FloatingWidget->windowHandle())));
	MimeData->setData(QLatin1String(PlatformDragPositionMimeType),
		serialize(DragStartOffset));
	MimeData->setData(QLatin1String(FloatingWidgetMimeType),
		serialize(reinterpret_cast<qintptr>(FloatingWidget)));
	Drag->setMimeData(MimeData);

	// The drop targets dock the floating widget in their dropEvent() handler,
	// which deletes the floating widget before exec() returns
	QPointer<CFloatingDockContainer> GuardedFloatingWidget(FloatingWidget);
	auto DockManager = FloatingWidget->dockContainer()->dockManager();
	s_PlatformDragDropCandidate = PlatformDragDropCandidate();
	// The QDrag::exec() return value does not reliably encode whether the
	// floating widget was docked on Wayland (e.g. compositors that send a
	// leave event instead of a drop event report Qt::IgnoreAction even though
	// the drag ended over a drop area). Track the docking outcome explicitly
	// instead and translate it into the documented return value below.
	Drag->exec();

	// Some compositors (e.g. Mutter) do not deliver a drop event to the
	// target container when the dragged window overlaps it - they send a
	// leave event on release instead. If no drop event was delivered, but
	// the drag ended over a valid drop area, dock the floating widget into
	// the recorded candidate. A short timeout distinguishes a drop on a
	// drop area from a release outside of any drop area to keep the widget
	// floating
	auto& Candidate = s_PlatformDragDropCandidate;
	bool Docked = Candidate.DropHandled;
	if (!Candidate.DropHandled && !GuardedFloatingWidget.isNull()
	 && Candidate.Container && Candidate.ValidDropArea
	 && Candidate.SinceLastUpdate.isValid()
	 && Candidate.SinceLastUpdate.elapsed() < PlatformDragDropCandidateTimeoutMs)
	{
		Candidate.Container->dropFloatingWidget(GuardedFloatingWidget,
			Candidate.GlobalPos);
		Docked = true;
	}
	s_PlatformDragDropCandidate = PlatformDragDropCandidate();
	DockManager->containerOverlay()->hideOverlay();
	DockManager->dockAreaOverlay()->hideOverlay();
	Drag->deleteLater();

	if (GuardedFloatingWidget)
	{
		GuardedFloatingWidget->d->setState(DraggingInactive);
	}

	return Docked ? Qt::MoveAction : Qt::IgnoreAction;
}


//============================================================================
bool CFloatingDockContainer::waylandMoveOrLeaveInWindowPreview(
	CFloatingDragPreview* Preview, QWidget* SourceWindow, const QPoint& GlobalPos)
{
	const QPoint InWindow = SourceWindow->mapFromGlobal(GlobalPos);
	if (SourceWindow->rect().contains(InWindow))
	{
		if (Preview)
		{
			Preview->moveFloating(GlobalPos);
		}
		return true;
	}

	// Boundary cross: tear down the in-window preview without performing a
	// drop. The caller converts the gesture into a native compositor platform
	// drag while the implicit pointer grab from the original press is still
	// active, which is what QDrag::exec() / xdg_toplevel_drag_v1 requires.
	if (Preview)
	{
		Preview->cancelDraggingSilently();
	}
	return false;
}


//============================================================================
Qt::DropAction CFloatingDockContainer::startPlatformDragForFloatingWidget(
	IFloatingWidget* FloatingWidget, const QPoint& GrabOffset, const QSize& Size,
	const QPoint& GlobalGrabPos, QWidget* DragSource)
{
	FloatingWidget->startFloating(GrabOffset, Size, DraggingInactive, nullptr);
	// Keep the grab point under the cursor across the conversion. A client
	// cannot position a Wayland top-level and the new window has no usable
	// geometry yet, so pass the surface-local grab offset explicitly rather
	// than letting startPlatformDrag() derive it from the press via
	// mapFromGlobal() (which would be random on Wayland). We must NOT move()
	// the window here either: besides being ignored by the compositor, a
	// geometry change between show() and exec() risks mapping the surface
	// before the attach, which would discard the offset.
	QPoint Offset = GrabOffset;
	return startPlatformDrag(static_cast<CFloatingDockContainer*>(FloatingWidget),
		GlobalGrabPos, DragSource, &Offset);
}


//============================================================================
CFloatingDockContainer* CFloatingDockContainer::floatingWidgetFromMimeData(
	const QMimeData* MimeData)
{
	if (!MimeData || !MimeData->hasFormat(QLatin1String(FloatingWidgetMimeType)))
	{
		return nullptr;
	}

	qintptr FloatingWidgetPtr = 0;
	QDataStream DataStream(MimeData->data(QLatin1String(FloatingWidgetMimeType)));
	DataStream >> FloatingWidgetPtr;
	return reinterpret_cast<CFloatingDockContainer*>(FloatingWidgetPtr);
}

#ifdef Q_OS_MACOS
//============================================================================
bool CFloatingDockContainer::event(QEvent *e)
{
	switch (d->DraggingState)
	{
	case DraggingInactive:
	{
		// Normally we would check here, if the left mouse button is pressed.
		// But from QT version 5.12.2 on the mouse events from
		// QEvent::NonClientAreaMouseButtonPress return the wrong mouse button
		// The event always returns Qt::RightButton even if the left button
		// is clicked.
		// It is really great to work around the whole NonClientMouseArea
		// bugs
#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 2))
		if (e->type() == QEvent::NonClientAreaMouseButtonPress /*&& QGuiApplication::mouseButtons().testFlag(Qt::LeftButton)*/)
#else
		if (e->type() == QEvent::NonClientAreaMouseButtonPress && QGuiApplication::mouseButtons().testFlag(Qt::LeftButton))
#endif
		{
			ADS_PRINT("FloatingWidget::event Event::NonClientAreaMouseButtonPress" << e->type());
			d->DragStartPos = pos();
			d->setState(DraggingMousePressed);
		}
	}
	break;

	case DraggingMousePressed:
		switch (e->type())
		{
		case QEvent::NonClientAreaMouseButtonDblClick:
			ADS_PRINT("FloatingWidget::event QEvent::NonClientAreaMouseButtonDblClick");
			d->setState(DraggingInactive);
			break;

		case QEvent::Resize:
			// If the first event after the mouse press is a resize event, then
			// the user resizes the window instead of dragging it around.
			// But there is one exception. If the window is maximized,
			// then dragging the window via title bar will cause the widget to
			// leave the maximized state. This in turn will trigger a resize event.
			// To know, if the resize event was triggered by user via moving a
			// corner of the window frame or if it was caused by a windows state
			// change, we check, if we are not in maximized state.
			if (!isMaximized())
			{
				d->setState(DraggingInactive);
			}
			break;

		default:
			break;
		}
		break;

	case DraggingFloatingWidget:
		if (e->type() == QEvent::NonClientAreaMouseButtonRelease)
		{
			ADS_PRINT("FloatingWidget::event QEvent::NonClientAreaMouseButtonRelease");
			d->titleMouseReleaseEvent();
		}
		break;

	default:
		break;
	}

#if (ADS_DEBUG_LEVEL > 0)
	qDebug() << QTime::currentTime() << "CFloatingDockContainer::event " << e->type();
#endif
	return QWidget::event(e);
}


//============================================================================
void CFloatingDockContainer::moveEvent(QMoveEvent *event)
{
	QWidget::moveEvent(event);
	switch (d->DraggingState)
	{
	case DraggingMousePressed:
		d->setState(DraggingFloatingWidget);
		d->updateDropOverlays(QCursor::pos());
		break;

	case DraggingFloatingWidget:
		d->updateDropOverlays(QCursor::pos());
		// In OSX when hiding the DockAreaOverlay the application would set
		// the main window as the active window for some reason. This fixes
		// that by resetting the active window to the floating widget after
		// updating the overlays.
		activateWindow();
		break;
	default:
		break;
	}


}
#endif


#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
//============================================================================
void CFloatingDockContainer::onMaximizeRequest()
{
	if (windowState() == Qt::WindowMaximized)
	{
		showNormal();
	}
	else
	{
		showMaximized();
	}
}


//============================================================================
void CFloatingDockContainer::showNormal(bool fixGeometry)
{
    if ( (windowState() & Qt::WindowMaximized) != 0 ||
         (windowState() & Qt::WindowFullScreen) != 0)
	{
		QRect oldNormal = normalGeometry();
		Super::showNormal();
		if(fixGeometry)
		{
			setGeometry(oldNormal);
		}
	}
	if(d->TitleBar)
	{
		d->TitleBar->setMaximizedIcon(false);
	}
}


//============================================================================
void CFloatingDockContainer::showMaximized()
{
	Super::showMaximized();
	if (d->TitleBar)
	{
		d->TitleBar->setMaximizedIcon(true);
	}
}


//============================================================================
bool CFloatingDockContainer::isMaximized() const
{
	return windowState() == Qt::WindowMaximized;
}


//============================================================================
void CFloatingDockContainer::show()
{
	// Prevent this window from showing in the taskbar and pager (alt+tab)
	internal::xcb_add_prop(true, winId(), "_NET_WM_STATE", "_NET_WM_STATE_SKIP_TASKBAR");
	internal::xcb_add_prop(true, winId(), "_NET_WM_STATE", "_NET_WM_STATE_SKIP_PAGER");
	Super::show();
}


//============================================================================
void CFloatingDockContainer::resizeEvent(QResizeEvent *event)
{
	d->IsResizing = true;
	Super::resizeEvent(event);
}


//============================================================================
void CFloatingDockContainer::moveEvent(QMoveEvent *event)
{
	Super::moveEvent(event);
	// On Wayland, the global cursor position is not available and the
	// compositor does not report window moves, so the docking via window
	// moves cannot work. Floating widgets are docked via the platform drag
	// and drop implemented in CDockContainerWidget instead
    if (!d->IsResizing && event->spontaneous() && d->MousePressed
     && !internal::isWayland())
	{
        d->setState(DraggingFloatingWidget);
		d->updateDropOverlays(QCursor::pos());
	}
	d->IsResizing = false;
}


//============================================================================
bool CFloatingDockContainer::event(QEvent *e)
{
	bool result = Super::event(e);
	switch (e->type())
	{
	case QEvent::WindowActivate:
        d->MousePressed = false;
		break;
	case QEvent::WindowDeactivate:
        d->MousePressed = true;
		break;
	default:
		break;
	}
	return result;
}

//============================================================================
bool CFloatingDockContainer::hasNativeTitleBar()
{
	return d->TitleBar == nullptr;
}
#endif

} // namespace ads

//---------------------------------------------------------------------------
// EOF FloatingDockContainer.cpp

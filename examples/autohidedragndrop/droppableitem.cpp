#include "droppableitem.h"

#include <QDragEnterEvent>
#include <QDragLeaveEvent>
#include <QDropEvent>
#include <QMimeData>
#include <qsizepolicy.h>

DroppableItem::DroppableItem(const QString& text, QWidget* parent)
    : QPushButton(text, parent)
{
    setAcceptDrops(true);
    setSizePolicy(QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Expanding);
}

void DroppableItem::dragEnterEvent(QDragEnterEvent* event)
{
    if (event->mimeData()->hasText())
    {
        event->acceptProposedAction();
        setCursor(Qt::DragMoveCursor);
    }
}

void DroppableItem::dragLeaveEvent(QDragLeaveEvent* event)
{
	Q_UNUSED(event);
    unsetCursor();
}

void DroppableItem::dropEvent(QDropEvent* event)
{
    if (event->mimeData()->hasText())
    {
        event->acceptProposedAction();
        setText(event->mimeData()->text());
    }
}

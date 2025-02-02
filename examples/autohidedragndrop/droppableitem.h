#include <QObject>
#include <QPushButton>

class QDragEnterEvent;
class QDragLeaveEvent;
class QDropEvent;

class DroppableItem : public QPushButton
{
    Q_OBJECT;

public:
    DroppableItem(const QString& text = QString(), QWidget* parent = nullptr);

protected:
    void dragEnterEvent(QDragEnterEvent* event) override;
    void dragLeaveEvent(QDragLeaveEvent* event) override;
    void dropEvent(QDropEvent* event) override;
};

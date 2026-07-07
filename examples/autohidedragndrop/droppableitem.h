#include <QObject>
#include <QPushButton>

QT_FORWARD_DECLARE_CLASS(QDragEnterEvent)
QT_FORWARD_DECLARE_CLASS(QDragLeaveEvent)
QT_FORWARD_DECLARE_CLASS(QDropEvent)

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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDebug>
#include <QList>
#include <QMainWindow>
#include <QQmlError>

#include <DockAreaWidget.h>
#include <DockManager.h>
namespace QtPrivate
{
template<>
inline QDebug printSequentialContainer(QDebug debug, const char* /*begin*/,
                                const QList<QQmlError>& container)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << '[';
    bool first = true;
    for (const QQmlError& e : container)
    {
        if (!first)
            debug << ", ";
        debug << e.toString();
        first = false;
    }
    debug << ']';
    return debug;
}
}  // namespace QtPrivate

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow() = default;

    ads::CDockAreaWidget* createDockWidget(
        QWidget* embedded_widget, QString dock_widget_title,
        ads::DockWidgetArea area, ads::CDockAreaWidget* DockAreaWidget = nullptr);

private:
    ads::CDockManager* _dock_manager;
};

#endif  // MAINWINDOW_H

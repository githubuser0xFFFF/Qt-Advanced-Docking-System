#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <DockManager.h>
#include <DockAreaWidget.h>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = 0);
    ~MainWindow() = default;

    ads::CDockAreaWidget* createDockWidget(QWidget* embedded_widget, QString dock_widget_title, ads::DockWidgetArea area,
        ads::CDockAreaWidget* DockAreaWidget = nullptr);

private:
    ads::CDockManager* _dock_manager;
};

#endif  // MAINWINDOW_H

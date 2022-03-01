#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedLayout>
#include "DockManager.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void openProject();
    void closeProject();

private:
    Ui::MainWindow *ui;
    QWidget* m_welcomeWidget;
    ads::CDockManager* m_DockManager;
    QStackedLayout* m_layout;
};

#endif // MAINWINDOW_H

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <Qt>

#include <memory>

namespace Ui
{
class MainWindow;
}  // namespace Ui

namespace ads
{
class CDockManager;
};

class MainWindow : public QMainWindow
{
   Q_OBJECT

public:
   MainWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = { });
   virtual ~MainWindow();

   void SetDockManager(ads::CDockManager * const dockManager);

   virtual bool eventFilter(QObject * watched, QEvent * event) override;

private:
   void SetupDockWidgetFeatures();
   void SetupCreateCallbacks();

   ads::CDockManager * dockManager;

   std::unique_ptr< Ui::MainWindow > ui;

};

#endif // MAINWINDOW_H

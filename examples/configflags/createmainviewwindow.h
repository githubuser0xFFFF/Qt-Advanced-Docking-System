#ifndef CREATEMAINVIEWWINDOW_H
#define CREATEMAINVIEWWINDOW_H

#include <QMainWindow>
#include <Qt>

#include <memory>

namespace Ui
{
class CreateMainViewWindow;
}  // namespace Ui

class CreateMainViewWindow : public QMainWindow
{
   Q_OBJECT

public:
   CreateMainViewWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = { });
   virtual ~CreateMainViewWindow();

private:
   void SetupConfigFlagsTreeWidget();
   void SetupAutoHideFlagsListTreeWidget();
   void SetupCreateMainWindowPushButton();

   std::unique_ptr< Ui::CreateMainViewWindow > ui;

};

#endif // CREATEMAINVIEWWINDOW_H

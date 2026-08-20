#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <DockManager.h>
#include <DockWidget.h>

#include <QAction>
#include <QCalendarWidget>
#include <QDial>
#include <QLCDNumber>
#include <QMessageBox>
#include <QMetaType>
#include <QObject>
#include <QString>
#include <QTextEdit>

#include <array>
#include <functional>
#include <typeinfo>

Q_DECLARE_METATYPE(ads::CDockWidget::DockWidgetFeature);

template < typename FlagsT, size_t SIZE >
static FlagsT GetDockWidgetFlags(const std::array< const QAction *, SIZE > & actions)
{
   FlagsT flags { };

   for (const auto & action : actions)
   {
      if (action->isChecked())
      {
         flags |= action->data().template value< typename FlagsT::enum_type >();
      }
   }

   return flags;
}

template < typename WidgetT >
static void CreateWidget(
   const bool /*checked*/,
   Ui::MainWindow & ui,
   MainWindow & mainWindow,
   ads::CDockManager & dockManager)
{
   static size_t numberOfInstances { };

   ads::CDockWidget * const dockWidget =
      dockManager.createDockWidget((QString::fromStdString(typeid(WidgetT).name()) + " %1").arg(++numberOfInstances));

   WidgetT * const widget = new WidgetT;

   dockWidget->setWidget(widget);

   const std::array< const QAction *, 10 > dockWidgetActions {
      ui.actionDockWidgetClosable,
      ui.actionDockWidgetMovable,
      ui.actionDockWidgetFloatable,
      ui.actionDockWidgetDeleteOnClose,
      ui.actionCustomCloseHandling,
      ui.actionDockWidgetFocusable,
      ui.actionDockWidgetForceCloseWithArea,
      ui.actionNoTab,
      ui.actionDeleteContentOnClose,
      ui.actionDockWidgetPinnable
   };

   const auto dockWidgetFlags = GetDockWidgetFlags< ads::CDockWidget::DockWidgetFeatures >(dockWidgetActions);

   dockWidget->setFeatures(dockWidgetFlags);

   if (dockWidgetFlags.testFlag(ads::CDockWidget::DockWidgetFeature::CustomCloseHandling))
   {
      QObject::connect(
         dockWidget,
         &ads::CDockWidget::closeRequested,
         dockWidget,
         [ dockWidget ] ( )
         {
            const int result = QMessageBox::question(dockWidget, "Close Dock Widget",
               QString { "Close %1?" }.arg(dockWidget->windowTitle()));

            if (result == QMessageBox::StandardButton::Yes)
            {
               dockWidget->closeDockWidget();
            }
         });
   }

   dockManager.addDockWidget(ads::DockWidgetArea::AllDockAreas, dockWidget);
}

MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags) :
QMainWindow { parent, flags },
dockManager { nullptr },
ui { std::make_unique< Ui::MainWindow >() }
{
   ui->setupUi(this);
}

MainWindow::~MainWindow()
{
}

void MainWindow::SetDockManager(ads::CDockManager * const dockManager)
{
   this->dockManager = dockManager;

   SetupDockWidgetFeatures();
   SetupCreateCallbacks();

   ui->menuDockWidgetFeatures->installEventFilter(this);
   ui->menuCreate->installEventFilter(this);
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
   if (event->type() == QEvent::MouseButtonRelease)
   {
      auto action = qobject_cast< QMenu * >(watched)->activeAction();

      if (action && action->isCheckable())
      {
         action->trigger();

         return true;
      }
   }

   return QObject::eventFilter(watched, event);
}

void MainWindow::SetupDockWidgetFeatures()
{
   using Features = ads::CDockWidget::DockWidgetFeature;

   ui->actionDockWidgetClosable->setData(QVariant { Features::DockWidgetClosable });
   ui->actionDockWidgetMovable->setData(QVariant { Features::DockWidgetMovable });
   ui->actionDockWidgetFloatable->setData(QVariant { Features::DockWidgetFloatable });
   ui->actionDockWidgetDeleteOnClose->setData(QVariant { Features::DockWidgetDeleteOnClose });
   ui->actionCustomCloseHandling->setData(QVariant { Features::CustomCloseHandling });
   ui->actionDockWidgetFocusable->setData(QVariant { Features::DockWidgetFocusable });
   ui->actionDockWidgetForceCloseWithArea->setData(QVariant { Features::DockWidgetForceCloseWithArea });
   ui->actionNoTab->setData(QVariant { Features::NoTab });
   ui->actionDeleteContentOnClose->setData(QVariant { Features::DeleteContentOnClose });
   ui->actionDockWidgetPinnable->setData(QVariant { Features::DockWidgetPinnable });
}

void MainWindow::SetupCreateCallbacks()
{
   QObject::connect(
      ui->actionCalendar,
      &QAction::triggered,
      ui->actionCalendar,
      std::bind(
         &CreateWidget< QCalendarWidget >,
         std::placeholders::_1,
         std::ref(*ui),
         std::ref(*this),
         std::ref(*dockManager)));

   QObject::connect(
      ui->actionDial,
      &QAction::triggered,
      ui->actionDial,
      std::bind(
         &CreateWidget< QDial >,
         std::placeholders::_1,
         std::ref(*ui),
         std::ref(*this),
         std::ref(*dockManager)));

   QObject::connect(
      ui->actionLCDNumber,
      &QAction::triggered,
      ui->actionLCDNumber,
      std::bind(
         &CreateWidget< QLCDNumber >,
         std::placeholders::_1,
         std::ref(*ui),
         std::ref(*this),
         std::ref(*dockManager)));

   QObject::connect(
      ui->actionTextEdit,
      &QAction::triggered,
      ui->actionTextEdit,
      std::bind(
         &CreateWidget< QTextEdit >,
         std::placeholders::_1,
         std::ref(*ui),
         std::ref(*this),
         std::ref(*dockManager)));
}

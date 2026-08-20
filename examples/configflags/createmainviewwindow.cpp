#include "createmainviewwindow.h"
#include "mainwindow.h"

#include "ui_createmainviewwindow.h"

#include <DockManager.h>

#include <QHeaderView>
#include <QMetaType>
#include <QObject>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QTreeWidgetItemIterator>
#include <QVariant>

Q_DECLARE_METATYPE(ads::CDockManager::eConfigFlag);
Q_DECLARE_METATYPE(ads::CDockManager::eAutoHideFlag);

template < typename FlagsT >
static FlagsT GetDockManagerFlags(QTreeWidget & treeWidget)
{
   FlagsT flags { };

   for (QTreeWidgetItemIterator item { &treeWidget }; *item; ++item)
   {
      if ((*item)->checkState(1) == Qt::CheckState::Checked)
      {
         flags |= (*item)->data(0, Qt::ItemDataRole::UserRole).value< typename FlagsT::enum_type >();
      }
   }

   return flags;
}

CreateMainViewWindow::CreateMainViewWindow(QWidget *parent, Qt::WindowFlags flags) :
QMainWindow(parent, flags),
ui(std::make_unique<Ui::CreateMainViewWindow>())
{
    ui->setupUi(this);

    SetupConfigFlagsTreeWidget();
    SetupAutoHideFlagsListTreeWidget();
    SetupCreateMainWindowPushButton();
}

CreateMainViewWindow::~CreateMainViewWindow()
{
}

void CreateMainViewWindow::SetupConfigFlagsTreeWidget()
{
   ui->configFlagsListTreeWidget->setColumnCount(2);

   ui->configFlagsListTreeWidget->setHeaderLabels(
      QStringList { } << "Dock Manager Flag" << "Enabled");

   ui->configFlagsListTreeWidget->header()->setSectionResizeMode(
      QHeaderView::ResizeMode::ResizeToContents);

   using ConfigFlag = ads::CDockManager::eConfigFlag;

   struct Item
   {
      QString name;
      QString tooltip;
      ConfigFlag flag;
      bool checked;
   };

   const Item items[]
   {
      { "ActiveTabHasCloseButton", "If this flag is set, the active tab in a tab area has a close button", ConfigFlag::ActiveTabHasCloseButton, true },
      { "DockAreaHasCloseButton", "If the flag is set each dock area has a close button", ConfigFlag::DockAreaHasCloseButton, true },
      { "DockAreaCloseButtonClosesTab", "If the flag is set, the dock area close button closes the active tab, if not set, it closes the complete dock area", ConfigFlag::DockAreaCloseButtonClosesTab, false },
      { "OpaqueSplitterResize", "See QSplitter::setOpaqueResize() documentation", ConfigFlag::OpaqueSplitterResize, true },
      { "XmlAutoFormattingEnabled", "If enabled, the XML writer automatically adds line-breaks and indentation to empty sections between elements (ignorable whitespace).", ConfigFlag::XmlAutoFormattingEnabled, false },
      { "XmlCompressionEnabled", "If enabled, the XML output will be compressed and is not human readable anymore", ConfigFlag::XmlCompressionEnabled, true },
      { "TabCloseButtonIsToolButton", "If enabled the tab close buttons will be QToolButtons instead of QPushButtons - disabled by default", ConfigFlag::TabCloseButtonIsToolButton, false },
      { "AllTabsHaveCloseButton", "if this flag is set, then all tabs that are closable show a close button", ConfigFlag::AllTabsHaveCloseButton, false },
      { "RetainTabSizeWhenCloseButtonHidden", "if this flag is set, the space for the close button is reserved even if the close button is not visible", ConfigFlag::RetainTabSizeWhenCloseButtonHidden, false },
      { "DragPreviewIsDynamic", "If opaque undocking is disabled, this flag defines the behavior of the drag preview window, if this flag is enabled, the preview will be adjusted dynamically to the drop area", ConfigFlag::DragPreviewIsDynamic, false },
      { "DragPreviewShowsContentPixmap", "If opaque undocking is disabled, the created drag preview window shows a copy of the content of the dock widget / dock are that is dragged", ConfigFlag::DragPreviewShowsContentPixmap, true },
      { "DragPreviewHasWindowFrame", "If opaque undocking is disabled, then this flag configures if the drag preview is frameless or looks like a real window", ConfigFlag::DragPreviewHasWindowFrame, false },
      { "AlwaysShowTabs", "If this option is enabled, the tab of a dock widget is always displayed - even if it is the only visible dock widget in a floating widget.", ConfigFlag::AlwaysShowTabs, false },
      { "DockAreaHasUndockButton", "If the flag is set each dock area has an undock button", ConfigFlag::DockAreaHasUndockButton, true },
      { "DockAreaHasTabsMenuButton", "If the flag is set each dock area has a tabs menu button", ConfigFlag::DockAreaHasTabsMenuButton, true },
      { "DockAreaHideDisabledButtons", "If the flag is set disabled dock area buttons will not appear on the toolbar at all (enabling them will bring them back)", ConfigFlag::DockAreaHideDisabledButtons, false },
      { "DockAreaDynamicTabsMenuButtonVisibility", "If the flag is set, the tabs menu button will be shown only when it is required - that means, if the tabs are elided. If the tabs are not elided, it is hidden", ConfigFlag::DockAreaDynamicTabsMenuButtonVisibility, false },
      { "FloatingContainerHasWidgetTitle", "If set, the Floating Widget window title reflects the title of the current dock widget otherwise it displays the title set with `CDockManager::setFloatingContainersTitle` or application name as window title", ConfigFlag::FloatingContainerHasWidgetTitle, true },
      { "FloatingContainerHasWidgetIcon", "If set, the Floating Widget icon reflects the icon of the current dock widget otherwise it displays application icon", ConfigFlag::FloatingContainerHasWidgetIcon, false },
      { "HideSingleCentralWidgetTitleBar", "If there is only one single visible dock widget in the main dock container (the dock manager) and if this flag is set, then the titlebar of this dock widget will be hidden\nthis only makes sense for non draggable and non floatable widgets and enables the creation of some kind of \"central\" widget", ConfigFlag::HideSingleCentralWidgetTitleBar, false },
      { "FocusHighlighting", "enables styling of focused dock widget tabs or floating widget titlebar", ConfigFlag::FocusHighlighting, false },
      { "EqualSplitOnInsertion", "if enabled, the space is equally distributed to all widgets in a  splitter", ConfigFlag::EqualSplitOnInsertion, false },
#ifdef Q_OS_LINUX
      { "FloatingContainerForceNativeTitleBar",
        "Linux only ! Forces all FloatingContainer to use the native title bar. This might break docking for FloatinContainer on some Window Managers (like Kwin/KDE).\n"
        "If neither this nor FloatingContainerForceCustomTitleBar is set (the default) native titlebars are used except on known bad systems.\n"
        "Users can overwrite this by setting the environment variable ADS_UseNativeTitle to \"1\" or \"0\".",
        ConfigFlag::FloatingContainerForceNativeTitleBar, false },
      { "FloatingContainerForceQWidgetTitleBar",
        "Linux only ! Forces all FloatingContainer to use a QWidget based title bar.\n"
        "If neither this nor FloatingContainerForceNativeTitleBar is set (the default) native titlebars are used except on known bad systems.\n"
        "Users can overwrite this by setting the environment variable ADS_UseNativeTitle to \"1\" or \"0\".",
        ConfigFlag::FloatingContainerForceQWidgetTitleBar, false },
#endif // Q_OS_LINUX
		{ "MiddleMouseButtonClosesTab", "If the flag is set, the user can use the mouse middle button to close the tab under the mouse", ConfigFlag::MiddleMouseButtonClosesTab, false },
		{ "DisableTabTextEliding", "Set this flag to disable eliding of tab texts in dock area tabs", ConfigFlag::DisableTabTextEliding, false },
		{ "ShowTabTextOnlyForActiveTab", "Set this flag to show label texts in dock area tabs only for active tabs", ConfigFlag::ShowTabTextOnlyForActiveTab, false },
		{ "DoubleClickUndocksWidget", "If the flag is set, a double click on a tab undocks the widget", ConfigFlag::DoubleClickUndocksWidget, true },
		{ "TabsAtBottom", "If the flag is set, tabs will be shown at the bottom instead of in the title bar.", ConfigFlag::TabsAtBottom, false },
		{ "UseNativeWindows", "If the flag is set, windows for the dock and area widgets will be native.", ConfigFlag::UseNativeWindows, false },
      { "DisableStylesheet", "If the flag is set, the dock manager will not apply the default stylesheet", ConfigFlag::DisableStylesheet, false }
   };

   for (const auto & item : items)
   {
      auto treeItem = new QTreeWidgetItem {
         ui->configFlagsListTreeWidget,
         QStringList { item.name }
      };

      treeItem->setFlags(treeItem->flags() | Qt::ItemFlag::ItemIsUserCheckable);
      treeItem->setCheckState(1, item.checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

      treeItem->setToolTip(0, item.tooltip);
      treeItem->setToolTip(1, item.tooltip);

      treeItem->setData(0, Qt::ItemDataRole::UserRole, QVariant { item.flag });
   }
}

void CreateMainViewWindow::SetupAutoHideFlagsListTreeWidget()
{
   ui->autoHideFlagsListTreeWidget->setColumnCount(2);

   ui->autoHideFlagsListTreeWidget->setHeaderLabels(
      QStringList { } << "Auto Hide Flag" << "Enabled");

   ui->autoHideFlagsListTreeWidget->header()->setSectionResizeMode(
      QHeaderView::ResizeMode::ResizeToContents);

   using ConfigFlag = ads::CDockManager::eAutoHideFlag;

   struct Item
   {
      QString name;
      QString tooltip;
      ConfigFlag flag;
      bool checked;
   };

   const Item items[]
   {
      { "AutoHideFeatureEnabled", "enables / disables auto hide feature", ConfigFlag::AutoHideFeatureEnabled, true },
      { "DockAreaHasAutoHideButton", "If the flag is set each dock area has a auto hide menu button", ConfigFlag::DockAreaHasAutoHideButton, true },
      { "AutoHideButtonTogglesArea", "If the flag is set, the auto hide button enables auto hiding for all dock widgets in an area, if disabled, only the current dock widget will be toggled", ConfigFlag::AutoHideButtonTogglesArea, false },
      { "AutoHideButtonCheckable", "If the flag is set, the auto hide button will be checked and unchecked depending on the auto hide state. Mainly for styling purposes.", ConfigFlag::AutoHideButtonCheckable, false },
      { "AutoHideSideBarsIconOnly", "show only icons in auto hide side tab - if a tab has no icon, then the text will be shown", ConfigFlag::AutoHideSideBarsIconOnly, false },
      { "AutoHideShowOnMouseOver", "show the auto hide window on mouse over tab and hide it if mouse leaves auto hide container", ConfigFlag::AutoHideShowOnMouseOver, false },
      { "AutoHideCloseButtonCollapsesDock", "Close button of an auto hide container collapses the dock instead of hiding it completely", ConfigFlag::AutoHideCloseButtonCollapsesDock, false },
      { "AutoHideHasCloseButton", "If the flag is set an auto hide title bar has a close button", ConfigFlag::AutoHideHasCloseButton, false },
      { "AutoHideHasMinimizeButton", "if this flag is set, the auto hide title bar has a minimize button to collapse the dock widget", ConfigFlag::AutoHideHasMinimizeButton, true },
      { "AutoHideOpenOnDragHover", "if this flag is set, dragging hover the tab bar will open the dock", ConfigFlag::AutoHideOpenOnDragHover, false },
      { "AutoHideCloseOnOutsideMouseClick", "if this flag is set, the auto hide dock container will collapse if the user clicks outside of the container, if not set, the auto hide container can be closed only via click on sidebar tab", ConfigFlag::AutoHideCloseOnOutsideMouseClick, true }
   };

   for (const auto & item : items)
   {
      auto treeItem = new QTreeWidgetItem {
         ui->autoHideFlagsListTreeWidget,
         QStringList { item.name }
      };

      treeItem->setFlags(treeItem->flags() | Qt::ItemFlag::ItemIsUserCheckable);
      treeItem->setCheckState(1, item.checked ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

      treeItem->setToolTip(0, item.tooltip);
      treeItem->setToolTip(1, item.tooltip);

      treeItem->setData(0, Qt::ItemDataRole::UserRole, QVariant { item.flag });
   }
}

void CreateMainViewWindow::SetupCreateMainWindowPushButton()
{
   QObject::connect(
      ui->createMainWindowPushButton,
      &QPushButton::clicked,
      [ this ] (
         const bool /*checked*/ )
      {
         const auto configFlags = GetDockManagerFlags< ads::CDockManager::ConfigFlags >(*ui->configFlagsListTreeWidget);
         const auto autoHideFlags = GetDockManagerFlags< ads::CDockManager::AutoHideFlags >(*ui->autoHideFlagsListTreeWidget);

         ads::CDockManager::setConfigFlags(configFlags);
         ads::CDockManager::setAutoHideConfigFlags(autoHideFlags);

         auto mainWindow = new MainWindow;

         auto dockManager = new ads::CDockManager { mainWindow };

         mainWindow->SetDockManager(dockManager);
         mainWindow->setAttribute(Qt::WidgetAttribute::WA_DeleteOnClose);
         mainWindow->show();

         ui->createMainWindowPushButton->setEnabled(false);

         QObject::connect(
            mainWindow,
            &QObject::destroyed,
            mainWindow,
            std::bind(
               &QPushButton::setEnabled,
               ui->createMainWindowPushButton,
               true));
      });
}

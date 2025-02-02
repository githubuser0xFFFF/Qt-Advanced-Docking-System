#include "mainwindow.h"
#include "droppableitem.h"

#include "ui_mainwindow.h"

#include <QWidgetAction>
#include <QFileSystemModel>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QFileDialog>
#include <QSettings>
#include <QPlainTextEdit>
#include <QToolBar>

#include "AutoHideDockContainer.h"
#include "DockAreaWidget.h"
#include "DockAreaTitleBar.h"

using namespace ads;

CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CMainWindow)
{
    ui->setupUi(this);
    CDockManager::setConfigFlag(CDockManager::OpaqueSplitterResize, true);
    CDockManager::setConfigFlag(CDockManager::XmlCompressionEnabled, false);
    CDockManager::setConfigFlag(CDockManager::FocusHighlighting, true);
    CDockManager::setAutoHideConfigFlags(CDockManager::DefaultAutoHideConfig);
    CDockManager::setAutoHideConfigFlag(CDockManager::AutoHideOpenOnDragHover, true);
    CDockManager::setConfigParam(CDockManager::AutoHideOpenOnDragHoverDelay_ms, 500);
    DockManager = new CDockManager(this);

    // Set central widget
    QPlainTextEdit* w = new QPlainTextEdit();
	w->setPlaceholderText("This is the central editor. Enter your text here.");
    CDockWidget* CentralDockWidget = new CDockWidget("CentralWidget");
    CentralDockWidget->setWidget(w);
    auto* CentralDockArea = DockManager->setCentralWidget(CentralDockWidget);
    CentralDockArea->setAllowedAreas(DockWidgetArea::OuterDockAreas);

    {
    	DroppableItem* droppableItem = new DroppableItem("Drop text here.");
		CDockWidget* dropDockWidget = new CDockWidget("Tab 1");
		dropDockWidget->setWidget(droppableItem);
		dropDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
		dropDockWidget->setMinimumSize(200,150);
		dropDockWidget->setAcceptDrops(true);
		const auto autoHideContainer = DockManager->addAutoHideDockWidget(SideBarLocation::SideBarLeft, dropDockWidget);
		autoHideContainer->setSize(480);
		autoHideContainer->setAcceptDrops(true);
		ui->menuView->addAction(dropDockWidget->toggleViewAction());
    }
    {
    	DroppableItem* droppableItem = new DroppableItem("Drop text here.");
		CDockWidget* dropDockWidget = new CDockWidget("Tab 2");
		dropDockWidget->setWidget(droppableItem);
		dropDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
		dropDockWidget->setMinimumSize(200,150);
		dropDockWidget->setAcceptDrops(true);
		const auto autoHideContainer = DockManager->addAutoHideDockWidget(SideBarLocation::SideBarRight, dropDockWidget);
		autoHideContainer->setSize(480);
		autoHideContainer->setAcceptDrops(true);
		ui->menuView->addAction(dropDockWidget->toggleViewAction());
    }

    QTableWidget* propertiesTable = new QTableWidget();
    propertiesTable->setColumnCount(3);
    propertiesTable->setRowCount(10);
    CDockWidget* PropertiesDockWidget = new CDockWidget("Properties");
    PropertiesDockWidget->setWidget(propertiesTable);
    PropertiesDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
    PropertiesDockWidget->resize(250, 150);
    PropertiesDockWidget->setMinimumSize(200,150);
    DockManager->addDockWidget(DockWidgetArea::RightDockWidgetArea, PropertiesDockWidget, CentralDockArea);
    ui->menuView->addAction(PropertiesDockWidget->toggleViewAction());

    createPerspectiveUi();
}

CMainWindow::~CMainWindow()
{
    delete ui;
}


void CMainWindow::createPerspectiveUi()
{
	SavePerspectiveAction = new QAction("Create Perspective", this);
	connect(SavePerspectiveAction, SIGNAL(triggered()), SLOT(savePerspective()));
	PerspectiveListAction = new QWidgetAction(this);
	PerspectiveComboBox = new QComboBox(this);
	PerspectiveComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
	PerspectiveComboBox->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	connect(PerspectiveComboBox, SIGNAL(currentTextChanged(const QString&)),
		DockManager, SLOT(openPerspective(const QString&)));
	PerspectiveListAction->setDefaultWidget(PerspectiveComboBox);
	ui->toolBar->addSeparator();
	ui->toolBar->addAction(PerspectiveListAction);
	ui->toolBar->addAction(SavePerspectiveAction);
}


void CMainWindow::savePerspective()
{
	QString PerspectiveName = QInputDialog::getText(this, "Save Perspective", "Enter unique name:");
	if (PerspectiveName.isEmpty())
	{
		return;
	}

	DockManager->addPerspective(PerspectiveName);
	QSignalBlocker Blocker(PerspectiveComboBox);
	PerspectiveComboBox->clear();
	PerspectiveComboBox->addItems(DockManager->perspectiveNames());
	PerspectiveComboBox->setCurrentText(PerspectiveName);
}


//============================================================================
void CMainWindow::closeEvent(QCloseEvent* event)
{
    // Delete dock manager here to delete all floating widgets. This ensures
    // that all top level windows of the dock manager are properly closed
    DockManager->deleteLater();
	QMainWindow::closeEvent(event);
}




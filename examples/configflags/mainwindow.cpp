#include "mainwindow.h"

#include "ui_mainwindow.h"

#include <QLabel>
#include <QToolBar>

#include "DockAreaWidget.h"


using namespace ads;


CMainWindow::CMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CMainWindow)
{
	ui->setupUi(this);

	// Add the toolbar
	auto toolbar_ = addToolBar("Top Toolbar");

	// Create the dock manager
	ads::CDockManager::setConfigFlags(ads::CDockManager::DefaultOpaqueConfig);
	ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasCloseButton,
	    false);
	ads::CDockManager::setConfigFlag(ads::CDockManager::DockAreaHasUndockButton,
	    false);
	ads::CDockManager::setConfigFlag(
	    ads::CDockManager::DockAreaHasTabsMenuButton, false);
	auto DockManager = new ads::CDockManager(this);

	// Create a dockable widget
	QLabel *l1 = new QLabel();
	l1->setWordWrap(true);
	l1->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	l1->setText("Docking widget 1");
    ads::CDockWidget *dockWidget1 = DockManager->createDockWidget("Dock 1");
	dockWidget1->setWidget(l1);
	DockManager->addDockWidget(ads::LeftDockWidgetArea, dockWidget1);

	QLabel *l2 = new QLabel();
	l2->setWordWrap(true);
	l2->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	l2->setText("Docking widget 2");
    ads::CDockWidget *dockWidget2 = DockManager->createDockWidget("Dock 2");
	dockWidget2->setWidget(l2);
	DockManager->addDockWidget(ads::RightDockWidgetArea, dockWidget2);

	// Add menu actions
	ui->menuView->addAction(dockWidget1->toggleViewAction());
	ui->menuView->addAction(dockWidget2->toggleViewAction());
	toolbar_->addAction(dockWidget1->toggleViewAction());
	toolbar_->addAction(dockWidget2->toggleViewAction());
}


CMainWindow::~CMainWindow()
{
    delete ui;
}




#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "digitalclock.h"

#include <QWidgetAction>
#include <QLabel>
#include <QCalendarWidget>
#include <QTreeView>
#include <QFileSystemModel>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QRadioButton>
#include <QPushButton>
#include <QInputDialog>
#include <QFileDialog>
#include <QSettings>
#include <QMessageBox>

#include "DockAreaWidget.h"
#include "DockAreaTitleBar.h"
#include "DockAreaTabBar.h"
#include "FloatingDockContainer.h"
#include "DockComponentsFactory.h"

using namespace ads;

const QString CMainWindow::kTableTopLayout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<QtAdvancedDockingSystem Version=\"1\" UserVersion=\"0\" Containers=\"1\">"
            "<Container Floating=\"0\">"
                "<Splitter Orientation=\"-\" Count=\"3\">"
                    "<Area Tabs=\"1\" Current=\"Timeline\" AllowedAreas=\"15\">"
                        "<Widget Name=\"Timeline\" Closed=\"0\" TitleBarVisible=\"0\"/>"
                    "</Area>"
                    "<Splitter Orientation=\"|\" Count=\"3\">"
                        "<Splitter Orientation=\"-\" Count=\"2\">"
                            "<Area Tabs=\"1\" Current=\"File system\">"
                                "<Widget Name=\"File system\" Closed=\"0\" TitleBarVisible=\"1\"/>"
                            "</Area>"
                            "<Area Tabs=\"1\" Current=\"Table\">"
                                "<Widget Name=\"Table\" Closed=\"0\" TitleBarVisible=\"1\"/>"
                            "</Area>"
                            "<Sizes>344 272 </Sizes>"
                        "</Splitter>"
                        "<Area Tabs=\"1\" Current=\"\" AllowedAreas=\"15\">"
                            "<Widget Name=\"CentralWidget\" Closed=\"0\" TitleBarVisible=\"0\"/>"
                        "</Area>"
                        "<Area Tabs=\"1\" Current=\"Properties\">"
                            "<Widget Name=\"Properties\" Closed=\"0\" TitleBarVisible=\"1\"/>"
                        "</Area>"
                        "<Sizes>258 758 258 </Sizes>"
                    "</Splitter>"
                    "<Area Tabs=\"1\" Current=\"Status\" AllowedAreas=\"15\">"
                        "<Widget Name=\"Status\" Closed=\"0\" TitleBarVisible=\"0\"/>"
                    "</Area>"
                    "<Sizes>52 621 52 </Sizes>"
                "</Splitter>"
            "</Container>"
        "</QtAdvancedDockingSystem>";

const QString CMainWindow::kTableBottomLayout = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>"
        "<QtAdvancedDockingSystem Version=\"1\" UserVersion=\"0\" Containers=\"1\">"
            "<Container Floating=\"0\">"
                "<Splitter Orientation=\"-\" Count=\"3\">"
                    "<Splitter Orientation=\"|\" Count=\"3\">"
                        "<Area Tabs=\"2\" Current=\"Table\">"
                            "<Widget Name=\"Table\" Closed=\"0\" TitleBarVisible=\"1\"/>"
                            "<Widget Name=\"File system\" Closed=\"0\" TitleBarVisible=\"1\"/>"
                        "</Area>"
                        "<Area Tabs=\"1\" Current=\"\" AllowedAreas=\"15\">"
                            "<Widget Name=\"CentralWidget\" Closed=\"0\" TitleBarVisible=\"0\"/>"
                        "</Area>"
                        "<Area Tabs=\"1\" Current=\"Properties\">"
                            "<Widget Name=\"Properties\" Closed=\"0\" TitleBarVisible=\"1\"/>"
                        "</Area>"
                        "<Sizes>258 758 258 </Sizes>"
                    "</Splitter>"
                    "<Area Tabs=\"1\" Current=\"Timeline\" AllowedAreas=\"15\">"
                        "<Widget Name=\"Timeline\" Closed=\"0\" TitleBarVisible=\"0\"/>"
                    "</Area>"
                    "<Area Tabs=\"1\" Current=\"Status\" AllowedAreas=\"15\">"
                        "<Widget Name=\"Status\" Closed=\"0\" TitleBarVisible=\"0\"/>"
                    "</Area>"
                    "<Sizes>621 1 1 </Sizes>"
                "</Splitter>"
            "</Container>"
        "</QtAdvancedDockingSystem>";

CMainWindow::CMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::CMainWindow)
{
    ui->setupUi(this);
    CDockManager::setConfigFlag(CDockManager::OpaqueSplitterResize, true);
    CDockManager::setConfigFlag(CDockManager::XmlCompressionEnabled, false);
    DockManager = new CDockManager(this);
    QCalendarWidget* calendar = new QCalendarWidget();
//    CentralLabel->setAlignment(Qt::AlignCenter);
    CDockWidget* CentralDockWidget = new CDockWidget("CentralWidget");
    CentralDockWidget->setWidget(calendar);
    CentralDockWidget->setFeature(CDockWidget::DockWidgetClosable, false);
    CentralDockWidget->setFeature(CDockWidget::DockWidgetMovable, false);
    CentralDockWidget->setFeature(CDockWidget::DockWidgetFloatable, false);
    CentralDockWidget->setTitleBarVisible(false);
    auto* CentralDockArea = DockManager->setCentralWidget(CentralDockWidget);
    CentralDockArea->setAllowedAreas(DockWidgetArea::OuterDockAreas);

    QTreeView* fileTree = new QTreeView();
    fileTree->setFrameShape(QFrame::NoFrame);
    QFileSystemModel* fileModel = new QFileSystemModel(fileTree);
    fileModel->setRootPath(QDir::currentPath());
    fileTree->setModel(fileModel);
    CDockWidget* DataDockWidget = new CDockWidget("File system");
    DataDockWidget->setWidget(fileTree);
    DataDockWidget->resize(150, 250);
    DataDockWidget->setMinimumSize(100, 250);
    auto* fileArea = DockManager->addDockWidget(DockWidgetArea::LeftDockWidgetArea, DataDockWidget, CentralDockArea);

    QTableWidget* table = new QTableWidget();
    table->setColumnCount(3);
    table->setRowCount(10);
    CDockWidget* TableDockWidget = new CDockWidget("Table");
    TableDockWidget->setWidget(table);
    TableDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
    TableDockWidget->resize(250, 150);
    TableDockWidget->setMinimumSize(200,150);
    DockManager->addDockWidget(DockWidgetArea::BottomDockWidgetArea, TableDockWidget, fileArea);

    QTableWidget* propertiesTable = new QTableWidget();
    table->setColumnCount(3);
    table->setRowCount(10);
    CDockWidget* PropertiesDockWidget = new CDockWidget("Properties");
    PropertiesDockWidget->setWidget(propertiesTable);
    PropertiesDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
    PropertiesDockWidget->resize(250, 150);
    PropertiesDockWidget->setMinimumSize(200,150);
    DockManager->addDockWidget(DockWidgetArea::RightDockWidgetArea, PropertiesDockWidget, CentralDockArea);

    QWidget* timeLineWidget = new QWidget();
    QHBoxLayout* timelineLayout = new QHBoxLayout(timeLineWidget);
    QRadioButton* radioDockTop = new QRadioButton("Top", timeLineWidget);
    QRadioButton* radioDockBottom = new QRadioButton("Bottom", timeLineWidget);
    radioDockTop->setChecked(true);
    timelineLayout->addWidget(new QLabel("Fixed height Dock widget."));
    timelineLayout->addStretch(1);
    timelineLayout->addWidget(new QLabel("Apply predefined perspective: ", this));
    timelineLayout->addWidget(radioDockTop);
    timelineLayout->addWidget(radioDockBottom);
    TimelineDockWidget = new CDockWidget("Timeline");
    TimelineDockWidget->setWidget(timeLineWidget);
    TimelineDockWidget->setResizeMode(CDockWidget::eResizeMode::ResizeHorizontal);
    TimelineDockWidget->setFeature(CDockWidget::DockWidgetClosable, false);
    TimelineDockWidget->setFeature(CDockWidget::DockWidgetMovable, false);
    TimelineDockWidget->setFeature(CDockWidget::DockWidgetFloatable, false);
    TimelineDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
    TimelineDockWidget->setMinimumSize(QSize(50, 50));
    TimelineDockWidget->setFixedHeight(50);
    TimelineDockWidget->setTitleBarVisible(false);
    auto *TimelineDockArea = DockManager->addDockWidget(DockWidgetArea::TopDockWidgetArea, TimelineDockWidget);
    TimelineDockArea->setAllowedAreas(DockWidgetArea::OuterDockAreas);
    connect(radioDockTop, &QRadioButton::toggled, [this](bool checked){
        bool ok = true;
        if(!checked)
        {
            ok = DockManager->restoreState(kTableBottomLayout.toUtf8());
        }
        else
        {
            ok = DockManager->restoreState(kTableTopLayout.toUtf8());
        }
        if(!ok)
        {
            QMessageBox msgBox;
            msgBox.setText("Failed to apply perspective!");
            msgBox.exec();
        }
    });

    QWidget* statusWidget = new QWidget();
    QHBoxLayout* statusLayout = new QHBoxLayout(statusWidget);
    statusLayout->setSpacing(10);
    CDigitalClock* clock = new CDigitalClock(statusWidget);
    statusLayout->addWidget(new QLabel("Status Bar"));
    QPushButton* OpenPerspectiveButton = new QPushButton("Open Perspective", statusWidget);
    connect(OpenPerspectiveButton, &QPushButton::clicked, [this](bool checked){
        QString PerspectiveName = QFileDialog::getOpenFileName(this, "Open Perspective", "", "Perspective files (*.xml)");
        if (PerspectiveName.isEmpty())
        {
            return;
        }

        QFile stateFile(PerspectiveName);
        stateFile.open(QIODevice::ReadOnly);
        QByteArray state = stateFile.readAll();
        stateFile.close();
        if(!DockManager->restoreState(state))
        {
            QMessageBox msgBox;
            msgBox.setText("Failed to apply perspective " + stateFile.fileName());
            msgBox.exec();
        }
    });
    QPushButton* SavePerspectiveButton = new QPushButton("Create Perspective", statusWidget);
    connect(SavePerspectiveButton, &QPushButton::clicked, [this](bool checked){
        QString PerspectiveName = QInputDialog::getText(this, "Save Perspective", "Enter unique name:");
        if (PerspectiveName.isEmpty())
        {
            return;
        }

        QByteArray state = DockManager->saveState();
        QFile stateFile(PerspectiveName + ".xml");
        stateFile.open(QIODevice::WriteOnly);
        stateFile.write(state);
        stateFile.close();
    });
    statusLayout->addWidget(OpenPerspectiveButton);
    statusLayout->addWidget(SavePerspectiveButton);
    statusLayout->addStretch(1);
    statusLayout->addWidget(clock);
    CDockWidget* StatusDockWidget = new CDockWidget("Status");
//    StatusDockWidget->setResizeWithSplitter(CDockWidget::ResizeHorizontal);
    StatusDockWidget->setWidget(statusWidget);
    StatusDockWidget->setResizeMode(CDockWidget::eResizeMode::ResizeHorizontal);
    StatusDockWidget->setFeature(CDockWidget::DockWidgetClosable, false);
    StatusDockWidget->setFeature(CDockWidget::DockWidgetMovable, false);
    StatusDockWidget->setFeature(CDockWidget::DockWidgetFloatable, false);
    StatusDockWidget->setMinimumSizeHintMode(CDockWidget::MinimumSizeHintFromDockWidget);
    StatusDockWidget->setMinimumSize(QSize(50, 50));
    StatusDockWidget->setFixedHeight(50);
    StatusDockWidget->setTitleBarVisible(false);
    StatusDockArea = DockManager->addDockWidget(DockWidgetArea::BottomDockWidgetArea, StatusDockWidget);
    StatusDockArea->setAllowedAreas(DockWidgetArea::OuterDockAreas);
}

CMainWindow::~CMainWindow()
{
    delete ui;


}


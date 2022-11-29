#include "../../examples/hideshow/MainWindow.h"

#include "ui_MainWindow.h"

#include <QLabel>
#include <QPushButton>

using namespace ads;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->centralWidget->setLayout( m_layout = new QStackedLayout() );

    m_welcomeWidget = new QWidget(this);
    auto welcomeLayout = new QVBoxLayout(m_welcomeWidget);
    welcomeLayout->addStretch();
    QPushButton* openButton = new QPushButton("Open project");
    welcomeLayout->addWidget( openButton );
    welcomeLayout->addStretch();

    connect( openButton, SIGNAL(clicked()), this, SLOT(openProject()) );

    m_DockManager = new ads::CDockManager(ui->centralWidget);

    // Create example content label - this can be any application specific
	// widget
	QLabel* l = new QLabel();
	l->setWordWrap(true);
	l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
	l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

	// Create a dock widget with the title Label 1 and set the created label
	// as the dock widget content
	ads::CDockWidget* DockWidget = new ads::CDockWidget("Label 1");
	DockWidget->setWidget(l);

	// Add the toggleViewAction of the dock widget to the menu to give
	// the user the possibility to show the dock widget if it has been closed
	ui->menuView->addAction(DockWidget->toggleViewAction());

    connect( ui->actionOpen, SIGNAL(triggered()), this, SLOT(openProject()) );
    connect( ui->actionClose, SIGNAL(triggered()), this, SLOT(closeProject()) );

	// Add the dock widget to the top dock widget area
	m_DockManager->addDockWidget(ads::TopDockWidgetArea, DockWidget);

    ui->centralWidget->layout()->addWidget( m_welcomeWidget );
    ui->centralWidget->layout()->addWidget( m_DockManager );

    closeProject();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::openProject()
{
    ui->actionOpen->setEnabled(false);
    ui->actionClose->setEnabled(true);
    ui->menuView->setEnabled(true);

    m_layout->setCurrentWidget( m_DockManager );
}

void MainWindow::closeProject()
{
    ui->actionOpen->setEnabled(true);
    ui->actionClose->setEnabled(false);
    ui->menuView->setEnabled(false);

    m_DockManager->hideManagerAndFloatingWidgets();
    m_layout->setCurrentWidget( m_welcomeWidget );
}


#include "mainframe.h"

#include "dockindock.h"
#include "perspectives.h"

#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_perspectivesManager( new QtAdsUtl::PerspectivesManager( "persist" ) )
{
    resize( 400, 400 );

    setCentralWidget( m_dockManager = new QtAdsUtl::DockInDockWidget(this,true,m_perspectivesManager.get()) );

    m_dockManager->attachViewMenu( menuBar()->addMenu( "View" ) );

    ads::CDockAreaWidget* previousDockWidget = NULL;
    for ( int i = 0; i != 3; ++i )
    {
        // Create example content label - this can be any application specific
        // widget
        QLabel* l = new QLabel();
        l->setWordWrap(true);
        l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

        previousDockWidget = m_dockManager->addTabWidget( l, "Top label " + QString::number(i), previousDockWidget );
    }

    auto lastTopLevelDock = previousDockWidget;

    for ( int j = 0; j != 2; ++j )
    {
        QtAdsUtl::DockInDockWidget* groupManager = m_dockManager->createGroup( "Group " + QString::number(j), lastTopLevelDock );

        previousDockWidget = NULL;
        for ( int i = 0; i != 3; ++i )
        {
            // Create example content label - this can be any application specific
            // widget
            QLabel* l = new QLabel();
            l->setWordWrap(true);
            l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

            previousDockWidget = groupManager->addTabWidget( l, "ZInner " + QString::number(j) + "/" + QString::number(i), previousDockWidget );
        }

        // create sub-group
        auto subGroup = groupManager->createGroup( "SubGroup " + QString::number(j), previousDockWidget );
        previousDockWidget = NULL;
        for ( int i = 0; i != 3; ++i )
        {
            // Create example content label - this can be any application specific
            // widget
            QLabel* l = new QLabel();
            l->setWordWrap(true);
            l->setAlignment(Qt::AlignTop | Qt::AlignLeft);
            l->setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ");

            previousDockWidget = subGroup->addTabWidget( l, "SubInner " + QString::number(j) + "/" + QString::number(i), previousDockWidget );
        }
    }

    m_perspectivesManager->loadPerspectives();
}

MainWindow::~MainWindow()
{
    m_perspectivesManager->savePerspectives();
}


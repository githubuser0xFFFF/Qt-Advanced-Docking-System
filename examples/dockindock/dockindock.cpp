#include "dockindock.h"
#include "perspectives.h"
#include "dockindockmanager.h"
#include "perspectiveactions.h"

#include <QMenu>
#include <QDir>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QMessageBox>
#include <QInputDialog>

#include <set>
#include <assert.h>

using namespace QtAdsUtl;

DockInDockWidget::DockInDockWidget( QWidget* parent, bool canCreateNewGroups, PerspectivesManager* perspectivesManager ) : 
    DockInDockWidget( parent, (DockInDockWidget*)NULL, perspectivesManager )
{
    m_canCreateNewGroups = canCreateNewGroups;
    m_topLevelDockWidget = this;
}

DockInDockWidget::DockInDockWidget( QWidget* parent, DockInDockWidget* topLevelDockWidget, PerspectivesManager* perspectivesManager ) : 
    baseClass( parent ),
    m_topLevelDockWidget( topLevelDockWidget ),
    m_canCreateNewGroups( (topLevelDockWidget) ? topLevelDockWidget->m_canCreateNewGroups : false ),
    m_perspectivesManager( perspectivesManager )
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins( 0,0,0,0 );
    layout->addWidget( m_mgr = new DockInDockManager(*this) );
}

DockInDockWidget::~DockInDockWidget()
{
    
}

ads::CDockAreaWidget* DockInDockWidget::addTabWidget( QWidget* widget, const QString& name, ads::CDockAreaWidget* after )
{
    return addTabWidget( widget, name, QIcon(), after );
}

ads::CDockAreaWidget* DockInDockWidget::addTabWidget( QWidget* widget, const QString& name, QIcon icon, ads::CDockAreaWidget* after )
{
    for ( auto existing : getTopLevelDockWidget()->getManager()->allDockWidgets(true,true) )
    {
        if ( existing.second->objectName() == name )
        {
            QMessageBox::critical( this, "Error", "Name '" + name + "' already in use" );
            return NULL;
        }
    }

    ads::CDockWidget* DockWidget = new ads::CDockWidget(name);
    DockWidget->setWidget(widget);
    DockWidget->setIcon( icon );

    // Add the dock widget to the top dock widget area
    return m_mgr->addDockWidget(ads::CenterDockWidgetArea, DockWidget, after);
}

bool DockInDockWidget::isTopLevel()
{
    return objectName().isEmpty();
}

QString DockInDockWidget::getGroupNameError( const QString& groupName )
{
    if ( groupName.isEmpty() )
    {
        return "Group must have a non-empty name";
    }

    std::vector<DockInDockManager*> dockManagers = m_mgr->allManagers( true, true );
    for ( auto mgr : dockManagers )
    {
        if ( mgr->getGroupName() == groupName )
            return "Group name '" + groupName + "' already used";
    }

    return "";
}

DockInDockWidget* DockInDockWidget::createGroup( const QString& groupName, ads::CDockAreaWidget*& insertPos )
{
    return createGroup( groupName, QIcon(), insertPos );
}

DockInDockWidget* DockInDockWidget::createGroup( const QString& groupName, QIcon icon, ads::CDockAreaWidget*& insertPos )
{
    QString error = getGroupNameError( groupName );
    if ( !error.isEmpty() )
    {
        QMessageBox::critical( NULL, "Error", error );
        return NULL;
    }

    DockInDockWidget* child = new DockInDockWidget( this, m_topLevelDockWidget, m_perspectivesManager );
    child->setObjectName( groupName );

    ads::CDockWidget* DockWidget = new ads::CDockWidget(groupName);
    DockWidget->setWidget(child);
    DockWidget->setIcon(icon);

    insertPos = m_mgr->addDockWidget(ads::CenterDockWidgetArea, DockWidget, insertPos);

    return child;
}

void DockInDockWidget::destroyGroup( DockInDockWidget* widgetToRemove )
{
    auto topLevelWidget = widgetToRemove->getTopLevelDockWidget();

    if ( topLevelWidget && topLevelWidget != widgetToRemove )
    {
        // reaffect all child docks to top-level
        for ( auto dockwidget : widgetToRemove->getManager()->getWidgetsInGUIOrder() ) // don't use allDockWidgets to preserve sub-groups
        {
            MoveDockWidgetAction::move( dockwidget, topLevelWidget->getManager() );
        }
        assert( widgetToRemove->getManager()->allDockWidgets( true, true ).empty() );

        // find widget's parent:
        for ( auto dockwidget : topLevelWidget->getManager()->allDockWidgets( true, true ) )
        {
            if ( dockwidget.second->widget() == widgetToRemove )
            {
                dockwidget.first->removeDockWidget( dockwidget.second );
                delete dockwidget.second;
                //delete widgetToRemove; automatically deleted when dockWidget is deleted
                widgetToRemove = NULL;
                break;
            }
        }

        assert( widgetToRemove == NULL );
    }
    else
    {
        assert( false );
    }
}

void DockInDockWidget::attachViewMenu( QMenu* menu )
{
    connect( menu, SIGNAL(aboutToShow()), this, SLOT(autoFillAttachedViewMenu()) );
}

void DockInDockWidget::autoFillAttachedViewMenu()
{
    QMenu* menu = dynamic_cast<QMenu*>( QObject::sender() );

    if ( menu )
    {
        menu->clear();
        setupViewMenu( menu );
    }
    else
    {
        assert( false );
    }
}

void DockInDockWidget::setupViewMenu( QMenu* menu )
{
    std::vector<DockInDockManager*> dockManagers = m_mgr->allManagers( true, true );

    bool hasPerspectivesMenu = false;
    if ( getTopLevelDockWidget() == this )
        hasPerspectivesMenu = (m_perspectivesManager != NULL);
    else
        assert( false );

    QMenu* organize = menu;
    if ( hasPerspectivesMenu )
        organize = menu->addMenu( "Organize" );

    setupMenu( organize, dockManagers );

    if ( hasPerspectivesMenu )
    {
        QMenu* perspectives = menu->addMenu( "Perspectives" );
        fillPerspectivesMenu( perspectives );
    }
}

void DockInDockWidget::setupMenu( QMenu* menu, const std::vector<DockInDockManager*>& moveTo )
{
    m_mgr->fillViewMenu( menu, moveTo );
    menu->addSeparator();
    auto moveMenu = menu->addMenu( "Move" );
    m_mgr->fillMoveMenu( moveMenu, moveTo );
}

void DockInDockWidget::fillPerspectivesMenu( QMenu* menu )
{
    menu->addAction( "Create perspective...", this, SLOT(createPerspective()) );

    QStringList perspectiveNames;
    if ( m_perspectivesManager )
        perspectiveNames = m_perspectivesManager->perspectiveNames();

    if ( !perspectiveNames.isEmpty() )
    {
        QMenu* load = menu->addMenu( "Load perspective" );
        for (const auto& name : perspectiveNames)
        {
            load->addAction(new LoadPerspectiveAction( load, name, *this));
        }
        QMenu* remove = menu->addMenu( "Remove perspective" );
        for (const auto& name : perspectiveNames)
        {
            remove->addAction( new RemovePerspectiveAction( remove, name, *this ));
        }
    }
}

void DockInDockWidget::setNewPerspectiveDefaultName( const QString& defaultName )
{
    m_newPerspectiveDefaultName = defaultName;
}

void DockInDockWidget::createPerspective()
{
    if ( !m_perspectivesManager )
        return;

    QString name = m_newPerspectiveDefaultName;
    if ( !m_newPerspectiveDefaultName.isEmpty() )
    {
        int index = 2;
        while ( m_perspectivesManager->perspectiveNames().contains( name ) )
        {
            name = m_newPerspectiveDefaultName + " (" + QString::number(index) + ")";
            ++index;
        }
    }

    while ( true )
    {
        bool ok = false;
        name = QInputDialog::getText( NULL, "Create perspective", "Enter perspective name", QLineEdit::Normal, name, &ok );
        if ( ok )
        {
            if ( name.isEmpty() )
            {
                QMessageBox::critical( NULL, "Error", "Perspective name cannot be empty" );
                continue;
            }
            else if ( m_perspectivesManager->perspectiveNames().contains( name ) )
            {
                if ( QMessageBox::critical( NULL, "Error", "Perspective '" + name + "' already exists, overwrite it?", QMessageBox::Yes | QMessageBox::No, QMessageBox::No ) == QMessageBox::No )
                    continue;
            }
            
            m_perspectivesManager->addPerspective( name, *this );
            break;
        }
        else
        {
            break;
        }
    }
}

static void dumpStatus( std::ostream& str, ads::CDockWidget* widget, const std::string& tab, std::string suffix )
{
    DockInDockManager* asMgr = DockInDockManager::dockInAManager( widget );
    if ( asMgr )
    {
        asMgr->parent().dumpStatus( str, tab );
    }
    else
    {
        str << tab << widget->objectName().toStdString() << suffix << std::endl;
    }
}

void DockInDockWidget::dumpStatus( std::ostream& str, std::string tab )
{
    str << tab << "Group: " << getManager()->getGroupName().toStdString() << std::endl;
    tab += "   ";
    std::set<ads::CDockWidget*> visibleWidgets;
    for ( auto widget : getManager()->getWidgetsInGUIOrder() )
    {
        visibleWidgets.insert( widget );
        ::dumpStatus( str, widget, tab, "" );
    }

    for ( auto closed : getManager()->dockWidgetsMap() )
    {
        if ( visibleWidgets.find( closed ) == visibleWidgets.end() )
        {
            ::dumpStatus( str, closed, tab, " (closed)" );
        }
    }
}

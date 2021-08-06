#include "dockindockmanager.h"
#include "dockindock.h"

#include "DockAreaWidget.h"

#include <QMenu>
#include <QInputDialog>
#include <QSettings>
#include <QMessageBox>

#include <assert.h>

using namespace QtAdsUtl;

/////////////////////////////////////
// DockInDockManager
/////////////////////////////////////
DockInDockManager::DockInDockManager( DockInDockWidget& parent ) :
    baseClass( &parent ),
    m_parent( parent )
{

}

DockInDockManager::~DockInDockManager()
{

}

void DockInDockManager::fillViewMenu( QMenu* menu, const std::vector<DockInDockManager*>& moveTo )
{
    auto widgetsMap = dockWidgetsMap();
    for ( auto iter = widgetsMap.begin(); iter != widgetsMap.end(); ++iter )
    {
        auto widget = iter.value()->widget();
        auto action = iter.value()->toggleViewAction();

        DockInDockWidget* asMgr = dynamic_cast<DockInDockWidget*>( widget );
        if ( asMgr )
        {
            auto subMenu = menu->addMenu( iter.key() );

            subMenu->addAction( action );
            subMenu->addSeparator();

            asMgr->setupMenu( subMenu, moveTo );
        }
        else
        {
            menu->addAction(action);
        }
    }

    if ( parent().canCreateNewGroups() )
    {
        // see how this works, to create it in the right place, 
        // and also to have load perspective work when some groups are missing
        menu->addSeparator();
        menu->addAction( new QtAdsUtl::CreateChildDockAction( m_parent, menu ) );

        if ( parent().getTopLevelDockWidget()->getManager() != this )
            menu->addAction( new QtAdsUtl::DestroyGroupAction( &m_parent, menu ) );
    }
}

void DockInDockManager::fillMoveMenu( QMenu* menu, const std::vector<DockInDockManager*>& moveTo )
{
    auto widgetsMap = dockWidgetsMap();
    for ( auto iter = widgetsMap.begin(); iter != widgetsMap.end(); ++iter )
    {
        auto subMenu = menu->addMenu( iter.key() );

        for ( auto mgr : moveTo )
        {
            // iterate over all possible target managers
            if ( mgr == this )
            {
                // if dock is already in mgr, no reason to move it there
            }
            else if ( mgr == dockInAManager( iter.value() ) )
            {
                // if target is the group itself, can't move it there, would make no sense
            }
            else
            {
                subMenu->addAction( new MoveDockWidgetAction( iter.value(), mgr, subMenu ) );
            }
        }
    }
}

void DockInDockManager::addPerspectiveRec( const QString& name )
{
    std::vector<DockInDockManager*> managers = allManagers( true, true );

    for ( auto child : managers )
        child->addPerspective( name );
}

void DockInDockManager::openPerspectiveRec( const QString& name )
{
    std::vector<DockInDockManager*> managers = allManagers( true, true );

    for ( auto child : managers )
        child->openPerspective( name );
}

QString DockInDockManager::getGroupName()
{
    return parent().objectName();
}

#define CHILD_PREFIX QString("Child-")
QString DockInDockManager::getPersistGroupName()
{
    QString group = "Top";
    if ( !getGroupName().isEmpty() )
        group = CHILD_PREFIX + getGroupName();
    return group;
}

QString DockInDockManager::getGroupNameFromPersistGroupName( QString persistGroupName )
{
    if ( persistGroupName.startsWith( CHILD_PREFIX ) )
    {
        persistGroupName = persistGroupName.mid( CHILD_PREFIX.size() );
    }
    else
    {
        assert( false );
    }
    return persistGroupName;
}

void DockInDockManager::loadPerspectivesRec(QSettings& Settings)
{
    std::vector<DockInDockManager*> children = allManagers( true, true );

    for ( auto mgr : children )
    {
        Settings.beginGroup(mgr->getPersistGroupName());
        mgr->loadPerspectives( Settings );
        Settings.endGroup();
    }
}

void DockInDockManager::savePerspectivesRec(QSettings& Settings) const
{
    std::vector<DockInDockManager*> children = allManagers( true, true );

    for ( auto mgr : children )
    {
        Settings.beginGroup(mgr->getPersistGroupName());
        mgr->savePerspectives( Settings );
        Settings.endGroup();
    }
}

void DockInDockManager::removePerspectivesRec()
{
    std::vector<DockInDockManager*> managers = allManagers( true, true );

    for ( auto child : managers )
        child->removePerspectives( child->perspectiveNames() );
}

DockInDockManager* DockInDockManager::dockInAManager( ads::CDockWidget* widget )
{
    DockInDockWidget* dockWidget = widget ? dynamic_cast<DockInDockWidget*>( widget->widget() ) : NULL;
    return ( dockWidget ) ? dockWidget->getManager() : NULL;
}

void DockInDockManager::childManagers( std::vector<DockInDockManager*>& managers, bool rec ) const
{
    auto widgets = getWidgetsInGUIOrder();
    for ( auto widget : widgets )
    {
        DockInDockManager* asMgr = dockInAManager( widget );
        if ( asMgr )
        {
            managers.push_back( asMgr );
            if ( rec )
                asMgr->childManagers( managers, rec );
        }
    }
}

std::vector<DockInDockManager*> DockInDockManager::allManagers( bool includeThis, bool rec ) const
{
    std::vector<DockInDockManager*> managers;
    if ( includeThis )
        managers.push_back( const_cast<DockInDockManager*>(this) );
    childManagers( managers, rec );
    return managers;
}

std::vector<std::pair<DockInDockManager*,ads::CDockWidget*>> DockInDockManager::allDockWidgets( bool includeThis, bool rec ) const
{
    std::vector<std::pair<DockInDockManager*,ads::CDockWidget*>> widgets;
    for ( auto mgr : allManagers( includeThis, rec ) )
    {
        for ( auto widget : mgr->getWidgetsInGUIOrder() )
            widgets.push_back( std::make_pair(mgr, widget) );
    }
    return widgets;
}

QMap<QString,QStringList> DockInDockManager::getGroupContents()
{
    QMap<QString,QStringList> result;
    std::vector<DockInDockManager*> managers = allManagers( true, true );
    for ( auto mgr : managers )
    {
        result[mgr->getPersistGroupName()] = mgr->dockWidgetsMap().keys();
    }
    return result;
}

ads::CDockAreaWidget* DockInDockManager::getInsertDefaultPos()
{
    ads::CDockAreaWidget* defaultPos = NULL;
    if ( dockAreaCount() != 0 )
        defaultPos = dockArea(dockAreaCount()-1);
    return defaultPos;
}

std::vector<ads::CDockWidget*> DockInDockManager::getWidgetsInGUIOrder() const
{
    std::vector<ads::CDockWidget*> result;
    result.reserve( dockWidgetsMap().size() );
    for ( int i = 0; i != dockAreaCount(); ++i )
    {
        for ( auto widget : dockArea(i)->dockWidgets() )
            result.push_back( widget );
    }
    return result;
}

/////////////////////////////////////
// CreateChildDockAction
/////////////////////////////////////
CreateChildDockAction::CreateChildDockAction( DockInDockWidget& dockInDock, QMenu* menu ) :
    QAction("New group...", menu),
    m_dockInDock( dockInDock )
{
    connect( this, SIGNAL(triggered()), this, SLOT(createGroup()) );
}

void CreateChildDockAction::createGroup()
{
    QString name = "";
    while ( true )
    {
        bool ok = false;
        name = QInputDialog::getText( NULL, this->text(), "Enter group name", QLineEdit::Normal, name, &ok );
        if ( ok )
        {
            QString error = "";
            if ( m_dockInDock.getTopLevelDockWidget() )
                error = m_dockInDock.getTopLevelDockWidget()->getGroupNameError( name );
            else
                assert( false );

            if ( error.isEmpty() )
            {
                ads::CDockAreaWidget* insertPos = NULL;
                m_dockInDock.createGroup( name, insertPos );
                break;
            }
            else
            {
                QMessageBox::critical( NULL, "Error", error );
                continue;
            }
        }
        else
        {
            break;
        }
    }
}

/////////////////////////////////////
// DestroyGroupAction
/////////////////////////////////////
DestroyGroupAction::DestroyGroupAction( DockInDockWidget* widget, QMenu* menu ) :
    QAction("Destroy " + widget->getManager()->getGroupName(), menu),
    m_widget( widget )
{
    connect( this, SIGNAL(triggered()), this, SLOT(destroyGroup()) );
}

void DestroyGroupAction::destroyGroup()
{
    m_widget->getTopLevelDockWidget()->destroyGroup( m_widget );
}


/////////////////////////////////////
// MoveDockWidgetAction
/////////////////////////////////////
MoveDockWidgetAction::MoveDockWidgetAction( ads::CDockWidget* widget, DockInDockManager* moveTo, QMenu* menu ) :
    QAction(menu),
    m_widget( widget ),
    m_moveTo( moveTo )
{
    if ( moveTo->parent().isTopLevel() )
    {
        setText( "To top" );
    }
    else
    {
        setText( "To " + moveTo->parent().objectName() );
    }
    connect( this, SIGNAL(triggered()), this, SLOT(move()) );
}

void MoveDockWidgetAction::move()
{
    move( m_widget, m_moveTo );
}

void MoveDockWidgetAction::move( ads::CDockWidget* widget, DockInDockManager* moveTo )
{
    if ( widget && moveTo )
    {
        widget->dockManager()->removeDockWidget( widget );
        moveTo->addDockWidget(ads::CenterDockWidgetArea, widget, moveTo->getInsertDefaultPos());
    }
    else
    {
        assert( false );
    }
}

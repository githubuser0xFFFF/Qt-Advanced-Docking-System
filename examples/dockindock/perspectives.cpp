#include "perspectives.h"
#include "dockindock.h"
#include "dockindockmanager.h"

#include <QSettings>
#include <QFile>
#include <QDir>

#include <assert.h>

#define GROUP_PREFIX QString("Group")

using namespace QtAdsUtl;

PerspectivesManager::PerspectivesManager( const QString& perspectivesFolder ) :
    m_perspectivesFolder( perspectivesFolder )
{
    
}

PerspectivesManager::~PerspectivesManager()
{
    // remove temp files:
    for ( auto perspective : m_perspectives )
    {
        QString fileName = perspective.settings->fileName();
        perspective.settings.reset();
        QFile::remove(fileName);
    }
}


QStringList PerspectivesManager::perspectiveNames() const
{
    return m_perspectives.keys();
}

void PerspectivesManager::addPerspective( const QString& name, DockInDockWidget& widget )
{
    if ( !m_perspectivesFolder.isEmpty() )
    {
        m_perspectives[name].settings = getSettingsObject( getSettingsFileName( name, true ) );
        m_perspectives[name].groups = widget.getManager()->getGroupContents();

        // save perspective internally
        widget.getManager()->addPerspectiveRec( name );
        // store it in QSettings object
        widget.getManager()->savePerspectivesRec( *(m_perspectives[name].settings) );
        // remove internal perspectives
        widget.getManager()->removePerspectives( widget.getManager()->perspectiveNames() );
    }
    else
    {
        assert( false );
    }

    emit perspectivesListChanged();
}


ads::CDockWidget* findWidget( QString name, const std::vector<DockInDockManager*>& managers )
{
    for ( auto mgr : managers )
    {
        auto widget = mgr->findDockWidget(name);
        if ( widget )
            return widget;
    }
    return NULL;
}

void PerspectivesManager::openPerspective( const QString& name, DockInDockWidget& widget )
{
    assert( widget.getTopLevelDockWidget() == &widget );

    if ( !m_perspectivesFolder.isEmpty() )
    {
        if ( m_perspectives.contains( name ) )
        {
            emit openingPerspective();

            if ( widget.canCreateNewGroups() )
            {
                auto curGroups = widget.getManager()->allManagers(true,true);
                for ( auto group : m_perspectives[name].groups.keys() )
                {
                    bool found = false;
                    for ( auto curgroup : curGroups )
                    {
                        if ( curgroup->getPersistGroupName() == group )
                        {
                            found = true;
                            break;
                        }
                    }
                    if ( !found )
                    {
                        group = DockInDockManager::getGroupNameFromPersistGroupName( group );

                        // restore group in file but not in GUI yet
                        ads::CDockAreaWidget* insertPos = NULL;
                        widget.createGroup( group, insertPos );
                    }
                }

                curGroups = widget.getManager()->allManagers(false,true);
                for ( auto curgroup : curGroups )
                {
                    if ( !m_perspectives[name].groups.keys().contains( curgroup->getPersistGroupName() ) )
                    {
                        widget.destroyGroup( &curgroup->parent() );
                    }
                }
            }

            auto managers = widget.getManager()->allManagers(true,true);
            for ( auto group : m_perspectives[name].groups.keys() )
            {
                for ( auto mgr : managers )
                {
                    if ( mgr->getPersistGroupName() == group )
                    {
                        for ( QString widgetName : m_perspectives[name].groups[group] )
                        {
                            ads::CDockWidget* widget = findWidget( widgetName, { mgr } );
                            if ( widget )
                            {
                                // OK, widget is already in the good manager!
                            }
                            else
                            {
                                widget = findWidget( widgetName, managers );
                                if ( widget )
                                {
                                    // move dock widget in the same group as it used to be when perspective was saved
                                    // this guarantee load/open perspectives will work smartly
                                    MoveDockWidgetAction::move( widget, mgr );
                                }
                            }
                        }
                    }
                }
            }

            // internally load perspectives from QSettings
            widget.getManager()->loadPerspectivesRec( *(m_perspectives[name].settings) );
            // load perspective (update GUI)
            widget.getManager()->openPerspectiveRec( name );
            // remove internal perspectives
            widget.getManager()->removePerspectives( widget.getManager()->perspectiveNames() );

            emit openedPerspective();
        }
    }
    else
    {
        assert( false );
    }
}

void PerspectivesManager::removePerspectives()
{
    m_perspectives.clear();
    emit perspectivesListChanged();
}

void PerspectivesManager::removePerspective( const QString& name )
{
    m_perspectives.remove( name );
    emit perspectivesListChanged();
}

QString PerspectivesManager::getSettingsFileName( const QString& perspective, bool temp ) const
{
    auto name = ( perspective.isEmpty() ) ? "perspectives.ini" : "perspective_" + perspective + (temp?".tmp":".ini");

    return m_perspectivesFolder + "/" + name;
}

std::shared_ptr<QSettings> PerspectivesManager::getSettingsObject( const QString& filePath ) const
{
    return std::make_shared<QSettings>(filePath, QSettings::IniFormat);
}

void PerspectivesManager::loadPerspectives()
{
    if ( !m_perspectivesFolder.isEmpty() )
    {
        QDir().mkpath( m_perspectivesFolder );

        m_perspectives.clear();
        
        auto mainSettings = getSettingsObject( getSettingsFileName( "", false ) );
        std::string debug = mainSettings->fileName().toStdString();

        int Size = mainSettings->beginReadArray("Perspectives");
        
        for (int i = 0; i < Size; ++i)
        {
            mainSettings->setArrayIndex(i);
            QString perspective = mainSettings->value("Name").toString();
            
            if ( !perspective.isEmpty() )
            {
                // load perspective file:
                auto toLoad = getSettingsFileName( perspective, false );
                auto loaded = getSettingsFileName( perspective, true );

#ifdef _DEBUG
                std::string debug1 = loaded.toStdString();
                std::string debug2 = toLoad.toStdString();
#endif

                QFile::remove( loaded );
                if ( !QFile::copy( toLoad, loaded ) )
                    assert( false );

                m_perspectives[perspective].settings = getSettingsObject( loaded );

                // load group info:
                mainSettings->beginGroup(GROUP_PREFIX);
                for (const auto& key : mainSettings->allKeys())
                {
                    m_perspectives[perspective].groups[key] = mainSettings->value( key ).toStringList();
                }
                mainSettings->endGroup();
            }
            else
            {
                assert( false );
            }
        }

        mainSettings->endArray();
    }

    emit perspectivesListChanged();
}

void PerspectivesManager::savePerspectives() const
{
    if ( !m_perspectivesFolder.isEmpty() )
    {
        auto mainSettings = getSettingsObject( getSettingsFileName( "", false ) );

        // Save list of perspective and group organization
        mainSettings->beginWriteArray("Perspectives", m_perspectives.size());
        int i = 0;
        for ( auto perspective : m_perspectives.keys() )
        {
            mainSettings->setArrayIndex(i);
            mainSettings->setValue("Name", perspective);
            mainSettings->beginGroup(GROUP_PREFIX);
            for ( auto group : m_perspectives[perspective].groups.keys() )
            {
                mainSettings->setValue( group, m_perspectives[perspective].groups[group] );
            }
            mainSettings->endGroup();
            ++i;
        }
        mainSettings->endArray();

        // Save perspectives themselves
        for ( auto perspectiveName : m_perspectives.keys() )
        {
            auto toSave = getSettingsFileName( perspectiveName, false );
            QSettings& settings = *(m_perspectives[perspectiveName].settings);
            settings.sync();

#ifdef _DEBUG
            std::string debug1 = settings.fileName().toStdString();
            std::string debug2 = toSave.toStdString();
#endif

            QFile::remove( toSave );
            if ( !QFile::copy( settings.fileName(), toSave ) )
                assert( false );
        }
    }
}

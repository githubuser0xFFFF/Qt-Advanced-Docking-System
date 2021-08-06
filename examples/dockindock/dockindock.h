#pragma once

#include <QWidget>
#include <QMap>

#include <memory>
#include <ostream>

class QMenu;

namespace ads
{
    class CDockAreaWidget;
}

namespace QtAdsUtl
{

class DockInDockManager;
class PerspectivesManager;
// tab of tab example for https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System/issues/306
class DockInDockWidget : public QWidget
{
    typedef QWidget baseClass;

    Q_OBJECT

public:
    DockInDockWidget( QWidget* parent, bool canCreateNewGroups, PerspectivesManager* perspectivesManager );
    ~DockInDockWidget() override; 

    ads::CDockAreaWidget* addTabWidget( QWidget* widget, const QString& name, ads::CDockAreaWidget* after );
    DockInDockWidget* createGroup( const QString& groupName, ads::CDockAreaWidget*& insertPos );

    ads::CDockAreaWidget* addTabWidget( QWidget* widget, const QString& name, QIcon icon, ads::CDockAreaWidget* after );
    DockInDockWidget* createGroup( const QString& groupName, QIcon icon, ads::CDockAreaWidget*& insertPos );
    
    QString getGroupNameError( const QString& groupName );
    void destroyGroup( DockInDockWidget* widget );

    /** Manually fill a given view menu */
    void setupViewMenu( QMenu* menu );

    /** Attach a view menu that will be automatically fill */
    void attachViewMenu( QMenu* menu );

    bool isTopLevel();
    void setupMenu( QMenu* menu, const std::vector<DockInDockManager*>& moveTo );

    inline DockInDockManager* getManager() { return m_mgr; }
    inline DockInDockWidget* getTopLevelDockWidget() { return m_topLevelDockWidget; }

    inline bool canCreateNewGroups() const { return m_canCreateNewGroups; }

    void dumpStatus( std::ostream& str, std::string tab = "" );

    inline PerspectivesManager* getPerspectivesManager() { return m_perspectivesManager; }

    void setNewPerspectiveDefaultName( const QString& defaultName );

private slots:
    void autoFillAttachedViewMenu();
    void createPerspective();

private:
    DockInDockManager* m_mgr;
    DockInDockWidget* m_topLevelDockWidget;

    bool m_canCreateNewGroups;

    DockInDockWidget( QWidget* parent, DockInDockWidget* topLevelDockWidget,  PerspectivesManager* perspectivesManager );

    PerspectivesManager* m_perspectivesManager;
    QString m_newPerspectiveDefaultName;

    void fillPerspectivesMenu( QMenu* menu );
};

}


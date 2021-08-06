#include "perspectiveactions.h"
#include "dockindock.h"
#include "perspectives.h"

#include <QMenu>

using namespace QtAdsUtl;

//////////////////////////////
// LoadPerspectiveAction
//////////////////////////////
LoadPerspectiveAction::LoadPerspectiveAction( QMenu* parent, const QString& name, QtAdsUtl::DockInDockWidget& dockManager ) :
    QAction( name, parent ),
    name( name ),
    dockManager( dockManager )
{
    connect( this, SIGNAL(triggered()), this, SLOT(load()) );
}

void LoadPerspectiveAction::load()
{
    dockManager.getPerspectivesManager()->openPerspective( name, dockManager );
}

//////////////////////////////
// RemovePerspectiveAction
//////////////////////////////
RemovePerspectiveAction::RemovePerspectiveAction( QMenu* parent, const QString& name, QtAdsUtl::DockInDockWidget& dockManager ) :
    QAction( name, parent ),
    name( name ),
    dockManager( dockManager )
{
    connect( this, SIGNAL(triggered()), this, SLOT(remove()) );
}

void RemovePerspectiveAction::remove()
{
    dockManager.getPerspectivesManager()->removePerspective( name );
}

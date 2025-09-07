//============================================================================
/// \file   DockComponentsFactory.cpp
/// \author Uwe Kindler
/// \date   10.02.2020
/// \brief  Implementation of DockComponentsFactory
//============================================================================

//============================================================================
//                                   INCLUDES
//============================================================================
#include <AutoHideTab.h>
#include "DockComponentsFactory.h"

#include "DockWidgetTab.h"
#include "DockAreaTabBar.h"
#include "DockAreaTitleBar.h"
#include "DockWidget.h"
#include "DockAreaWidget.h"

namespace ads
{

using FACTORY_SHAREDPOINTER = QSharedPointer<ads::CDockComponentsFactory>;
Q_GLOBAL_STATIC(FACTORY_SHAREDPOINTER, DefaultFactory);

//============================================================================
CDockWidgetTab* CDockComponentsFactory::createDockWidgetTab(CDockWidget* DockWidget) const
{
	return new CDockWidgetTab(DockWidget);
}

//============================================================================
CAutoHideTab* CDockComponentsFactory::createDockWidgetSideTab(CDockWidget *DockWidget) const
{
	return new CAutoHideTab(DockWidget);
}


//============================================================================
CDockAreaTabBar* CDockComponentsFactory::createDockAreaTabBar(CDockAreaWidget* DockArea) const
{
	return new CDockAreaTabBar(DockArea);
}


//============================================================================
CDockAreaTitleBar* CDockComponentsFactory::createDockAreaTitleBar(CDockAreaWidget* DockArea) const
{
	return new CDockAreaTitleBar(DockArea);
}


//============================================================================
QSharedPointer<ads::CDockComponentsFactory> CDockComponentsFactory::factory()
{
    if (DefaultFactory->isNull())
    {
        DefaultFactory->reset(new CDockComponentsFactory());
    }
    return *DefaultFactory;
}


//============================================================================
void CDockComponentsFactory::setFactory(CDockComponentsFactory* Factory)
{
    DefaultFactory->reset(Factory);
}


//============================================================================
void CDockComponentsFactory::resetDefaultFactory()
{
    DefaultFactory->reset(new CDockComponentsFactory());
}

} // namespace ads

//---------------------------------------------------------------------------
// EOF DockComponentsFactory.cpp

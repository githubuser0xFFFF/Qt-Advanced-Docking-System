/*******************************************************************************
** Qt Advanced Docking System
** Copyright (C) 2017 Uwe Kindler
** 
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
** 
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
** 
** You should have received a copy of the GNU Lesser General Public
** License along with this library; If not, see <http://www.gnu.org/licenses/>.
******************************************************************************/


//============================================================================
/// \file   DockManager.cpp
/// \author Uwe Kindler
/// \date   26.02.2017
/// \brief  Implementation of CDockManager class
//============================================================================


//============================================================================
//                                   INCLUDES
//============================================================================
#include <DockWidgetTab.h>
#include "DockManager.h"

#include <algorithm>
#include <iostream>

#include <QMainWindow>
#include <QList>
#include <QMap>
#include <QVariant>
#include <QDebug>
#include <QFile>
#include <QAction>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>
#include <QSettings>
#include <QMenu>
#include <QApplication>

#include "FloatingDockContainer.h"
#include "DockOverlay.h"
#include "DockWidget.h"
#include "ads_globals.h"
#include "DockAreaWidget.h"


namespace ads
{
/**
 * Private data class of CDockManager class (pimpl)
 */
struct DockManagerPrivate
{
	CDockManager* _this;
	QList<CFloatingDockContainer*> FloatingWidgets;
	QList<CDockContainerWidget*> Containers;
	CDockOverlay* ContainerOverlay;
	CDockOverlay* DockAreaOverlay;
	QMap<QString, CDockWidget*> DockWidgetsMap;
	QMap<QString, QByteArray> Perspectives;
	QMap<QString, QMenu*> ViewMenuGroups;
	QMenu* ViewMenu;
	CDockManager::eViewMenuInsertionOrder MenuInsertionOrder = CDockManager::MenuAlphabeticallySorted;
	bool RestoringState = false;
	CDockManager::ConfigFlags ConfigFlags{CDockManager::DefaultConfig};

	/**
	 * Private data constructor
	 */
	DockManagerPrivate(CDockManager* _public);

	/**
	 * Checks if the given data stream is a valid docking system state
	 * file.
	 */
	bool checkFormat(const QByteArray &state, int version);

	/**
	 * Restores the state
	 */
	bool restoreStateFromXml(const QByteArray &state, int version, bool Testing = internal::Restore);

	/**
	 * Restore state
	 */
	bool restoreState(const QByteArray &state, int version);

	void restoreDockWidgetsOpenState();
	void restoreDockAreasIndices();
	void emitTopLevelEvents();

	void hideFloatingWidgets()
	{
		// Hide updates of floating widgets from use
		for (auto FloatingWidget : FloatingWidgets)
		{
			FloatingWidget->hide();
		}
	}

	void markDockWidgetsDirty()
	{
		for (auto DockWidget : DockWidgetsMap)
		{
			DockWidget->setProperty("dirty", true);
		}
	}

	/**
	 * Restores the container with the given index
	 */
	bool restoreContainer(int Index, QXmlStreamReader& stream, bool Testing);

	/**
	 * Loads the stylesheet
	 */
	void loadStylesheet();

	/**
	 * Adds action to menu - optionally in sorted order
	 */
	void addActionToMenu(QAction* Action, QMenu* Menu, bool InsertSorted);
};
// struct DockManagerPrivate

//============================================================================
DockManagerPrivate::DockManagerPrivate(CDockManager* _public) :
	_this(_public)
{

}


//============================================================================
void DockManagerPrivate::loadStylesheet()
{
	QString Result;
	QFile StyleSheetFile(":ads/stylesheets/default.css");
	StyleSheetFile.open(QIODevice::ReadOnly);
	QTextStream StyleSheetStream(&StyleSheetFile);
	Result = StyleSheetStream.readAll();
	StyleSheetFile.close();
	_this->setStyleSheet(Result);
}


//============================================================================
bool DockManagerPrivate::restoreContainer(int Index, QXmlStreamReader& stream, bool Testing)
{
	if (Testing)
	{
		Index = 0;
	}

	bool Result = false;
	if (Index >= Containers.count())
	{
		CFloatingDockContainer* FloatingWidget = new CFloatingDockContainer(_this);
		Result = FloatingWidget->restoreState(stream, Testing);
	}
	else
	{
		qDebug() << "d->Containers[i]->restoreState ";
		auto Container = Containers[Index];
		if (Container->isFloating())
		{
			Result = Container->floatingWidget()->restoreState(stream, Testing);
		}
		else
		{
			Result = Container->restoreState(stream, Testing);
		}
	}

	return Result;
}


//============================================================================
bool DockManagerPrivate::checkFormat(const QByteArray &state, int version)
{
    return restoreStateFromXml(state, version, internal::RestoreTesting);
}


//============================================================================
bool DockManagerPrivate::restoreStateFromXml(const QByteArray &state,  int version,
	bool Testing)
{
    if (state.isEmpty())
    {
        return false;
    }
    QXmlStreamReader s(state);
    s.readNextStartElement();
    if (s.name() != "QtAdvancedDockingSystem")
    {
    	return false;
    }
    qDebug() << s.attributes().value("Version");
    bool ok;
    int v = s.attributes().value("Version").toInt(&ok);
    if (!ok || v != version)
    {
    	return false;
    }

    bool Result = true;
    int  DockContainers = s.attributes().value("Containers").toInt();
    qDebug() << DockContainers;
    int DockContainerCount = 0;
    while (s.readNextStartElement())
    {
    	if (s.name() == "Container")
    	{
    		Result = restoreContainer(DockContainerCount, s, Testing);
			if (!Result)
			{
				break;
			}
			DockContainerCount++;
    	}
    }

    if (!Testing)
    {
		// Delete remaining empty floating widgets
		int FloatingWidgetIndex = DockContainerCount - 1;
		int DeleteCount = FloatingWidgets.count() - FloatingWidgetIndex;
		for (int i = 0; i < DeleteCount; ++i)
		{
			FloatingWidgets[FloatingWidgetIndex + i]->deleteLater();
			_this->removeDockContainer(FloatingWidgets[FloatingWidgetIndex + i]->dockContainer());
		}
    }

    return Result;
}


//============================================================================
void DockManagerPrivate::restoreDockWidgetsOpenState()
{
    // All dock widgets, that have not been processed in the restore state
    // function are invisible to the user now and have no assigned dock area
    // They do not belong to any dock container, until the user toggles the
    // toggle view action the next time
    for (auto DockWidget : DockWidgetsMap)
    {
    	if (DockWidget->property("dirty").toBool())
    	{
    		DockWidget->flagAsUnassigned();
    	}
    	else
    	{
    		DockWidget->toggleViewInternal(!DockWidget->property("closed").toBool());
    	}
    }
}


//============================================================================
void DockManagerPrivate::restoreDockAreasIndices()
{
    // Now all dock areas are properly restored and we setup the index of
    // The dock areas because the previous toggleView() action has changed
    // the dock area index
    int Count = 0;
    for (auto DockContainer : Containers)
    {
    	Count++;
    	for (int i = 0; i < DockContainer->dockAreaCount(); ++i)
    	{
    		CDockAreaWidget* DockArea = DockContainer->dockArea(i);
    		QString DockWidgetName = DockArea->property("currentDockWidget").toString();
    		CDockWidget* DockWidget = nullptr;
    		if (!DockWidgetName.isEmpty())
    		{
    			DockWidget = _this->findDockWidget(DockWidgetName);
    		}

    		if (!DockWidget || DockWidget->isClosed())
    		{
    			int Index = DockArea->indexOfFirstOpenDockWidget();
    			if (Index < 0)
    			{
    				continue;
    			}
    			DockArea->setCurrentIndex(Index);
    		}
    		else
    		{
    			DockArea->internalSetCurrentDockWidget(DockWidget);
    		}
    	}
    }
}



//============================================================================
void DockManagerPrivate::emitTopLevelEvents()
{
    // Finally we need to send the topLevelChanged() signals for all dock
    // widgets if top level changed
    for (auto DockContainer : Containers)
    {
    	CDockWidget* TopLevelDockWidget = DockContainer->topLevelDockWidget();
    	if (TopLevelDockWidget)
    	{
    		TopLevelDockWidget->emitTopLevelChanged(true);
    	}
    	else
    	{
			for (int i = 0; i < DockContainer->dockAreaCount(); ++i)
			{
				auto DockArea = DockContainer->dockArea(i);
				for (auto DockWidget : DockArea->dockWidgets())
				{
					DockWidget->emitTopLevelChanged(false);
				}
			}
    	}
    }
}


//============================================================================
bool DockManagerPrivate::restoreState(const QByteArray &state, int version)
{
    if (!checkFormat(state, version))
    {
    	qDebug() << "checkFormat: Error checking format!!!!!!!";
    	return false;
    }

    // Hide updates of floating widgets from use
    hideFloatingWidgets();
    markDockWidgetsDirty();

    if (!restoreStateFromXml(state, version))
    {
    	qDebug() << "restoreState: Error restoring state!!!!!!!";
    	return false;
    }

    restoreDockWidgetsOpenState();
    restoreDockAreasIndices();
    emitTopLevelEvents();

    return true;
}


//============================================================================
void DockManagerPrivate::addActionToMenu(QAction* Action, QMenu* Menu, bool InsertSorted)
{
	if (InsertSorted)
	{
		auto Actions = Menu->actions();
		auto it = std::find_if(Actions.begin(), Actions.end(),
			[&Action](const QAction* a)
			{
				return a->text().compare(Action->text(), Qt::CaseInsensitive) > 0;
			});

		if (it == Actions.end())
		{
			Menu->addAction(Action);
		}
		else
		{
			Menu->insertAction(*it, Action);
		}
	}
	else
	{
		Menu->addAction(Action);
	}
}


//============================================================================
CDockManager::CDockManager(QWidget *parent) :
	CDockContainerWidget(this, parent),
	d(new DockManagerPrivate(this))
{
	createRootSplitter();
	QMainWindow* MainWindow = dynamic_cast<QMainWindow*>(parent);
	if (MainWindow)
	{
		MainWindow->setCentralWidget(this);
	}

	d->ViewMenu = new QMenu(tr("Show View"), this);
	d->DockAreaOverlay = new CDockOverlay(this, CDockOverlay::ModeDockAreaOverlay);
	d->ContainerOverlay = new CDockOverlay(this, CDockOverlay::ModeContainerOverlay);
	d->Containers.append(this);
	d->loadStylesheet();
}

//============================================================================
CDockManager::~CDockManager()
{
	auto FloatingWidgets = d->FloatingWidgets;
	for (auto FloatingWidget : FloatingWidgets)
	{
		delete FloatingWidget;
	}
	delete d;
}


//============================================================================
void CDockManager::registerFloatingWidget(CFloatingDockContainer* FloatingWidget)
{
	d->FloatingWidgets.append(FloatingWidget);
	qDebug() << "d->FloatingWidgets.count() " << d->FloatingWidgets.count();
}


//============================================================================
void CDockManager::removeFloatingWidget(CFloatingDockContainer* FloatingWidget)
{
	d->FloatingWidgets.removeAll(FloatingWidget);
}


//============================================================================
void CDockManager::registerDockContainer(CDockContainerWidget* DockContainer)
{
	d->Containers.append(DockContainer);
}


//============================================================================
void CDockManager::removeDockContainer(CDockContainerWidget* DockContainer)
{
	if (this != DockContainer)
	{
		d->Containers.removeAll(DockContainer);
	}
}


//============================================================================
CDockOverlay* CDockManager::containerOverlay() const
{
	return d->ContainerOverlay;
}


//============================================================================
CDockOverlay* CDockManager::dockAreaOverlay() const
{
	return d->DockAreaOverlay;
}


//============================================================================
const QList<CDockContainerWidget*> CDockManager::dockContainers() const
{
	return d->Containers;
}


//============================================================================
const QList<CFloatingDockContainer*> CDockManager::floatingWidgets() const
{
	return d->FloatingWidgets;
}


//============================================================================
unsigned int CDockManager::zOrderIndex() const
{
	return 0;
}


//============================================================================
QByteArray CDockManager::saveState(eXmlMode XmlMode, int version) const
{
    QByteArray xmldata;
    QXmlStreamWriter s(&xmldata);
	s.setAutoFormatting(XmlAutoFormattingEnabled == XmlMode);
    s.writeStartDocument();
		s.writeStartElement("QtAdvancedDockingSystem");
		s.writeAttribute("Version", QString::number(version));
		s.writeAttribute("Containers", QString::number(d->Containers.count()));
		for (auto Container : d->Containers)
		{
			Container->saveState(s);
		}

		s.writeEndElement();
    s.writeEndDocument();

    return xmldata;
}


//============================================================================
bool CDockManager::restoreState(const QByteArray &state, int version)
{
	// Prevent multiple calls as long as state is not restore. This may
	// happen, if QApplication::processEvents() is called somewhere
	if (d->RestoringState)
	{
		return false;
	}

	// We hide the complete dock manager here. Restoring the state means
	// that DockWidgets are removed from the DockArea internal stack layout
	// which in turn  means, that each time a widget is removed the stack
	// will show and raise the next available widget which in turn
	// triggers show events for the dock widgets. To avoid this we hide the
	// dock manager. Because there will be no processing of application
	// events until this function is finished, the user will not see this
	// hiding
	bool IsHidden = this->isHidden();
	if (!IsHidden)
	{
		hide();
	}
	d->RestoringState = true;
	emit restoringState();
	bool Result = d->restoreState(state, version);
	d->RestoringState = false;
	emit stateRestored();
	if (!IsHidden)
	{
		show();
	}

	return Result;
}


//============================================================================
CDockAreaWidget* CDockManager::addDockWidget(DockWidgetArea area,
	CDockWidget* Dockwidget, CDockAreaWidget* DockAreaWidget)
{
	d->DockWidgetsMap.insert(Dockwidget->objectName(), Dockwidget);
	return CDockContainerWidget::addDockWidget(area, Dockwidget, DockAreaWidget);
}


//============================================================================
CDockAreaWidget* CDockManager::addDockWidgetTab(DockWidgetArea area,
	CDockWidget* Dockwidget)
{
	CDockAreaWidget* AreaWidget = lastAddedDockAreaWidget(area);
	if (AreaWidget)
	{
		return addDockWidget(ads::CenterDockWidgetArea, Dockwidget, AreaWidget);
	}
	else
	{
		return addDockWidget(area, Dockwidget, AreaWidget);
	}
}


//============================================================================
CDockAreaWidget* CDockManager::addDockWidgetTabToArea(CDockWidget* Dockwidget,
	CDockAreaWidget* DockAreaWidget)
{
	return addDockWidget(ads::CenterDockWidgetArea, Dockwidget, DockAreaWidget);
}


//============================================================================
CDockWidget* CDockManager::findDockWidget(const QString& ObjectName) const
{
	return d->DockWidgetsMap.value(ObjectName, nullptr);
}


//============================================================================
QMap<QString, CDockWidget*> CDockManager::dockWidgetsMap() const
{
	return d->DockWidgetsMap;
}


//============================================================================
void CDockManager::addPerspective(const QString& UniquePrespectiveName)
{
	d->Perspectives.insert(UniquePrespectiveName, saveState());
	emit perspectiveListChanged();
}


//============================================================================
void CDockManager::removePerspective(const QString& Name)
{
	removePerspectives({Name});
}


//============================================================================
void CDockManager::removePerspectives(const QStringList& Names)
{
	int Count = 0;
	for (auto Name : Names)
	{
		Count += d->Perspectives.remove(Name);
	}

	if (Count)
	{
		emit perspectivesRemoved();
		emit perspectiveListChanged();
	}
}


//============================================================================
QStringList CDockManager::perspectiveNames() const
{
	return d->Perspectives.keys();
}


//============================================================================
void CDockManager::openPerspective(const QString& PerspectiveName)
{
	const auto Iterator = d->Perspectives.find(PerspectiveName);
	if (d->Perspectives.end() == Iterator)
	{
		return;
	}

	emit openingPerspective(PerspectiveName);
	restoreState(Iterator.value());
	emit perspectiveOpened(PerspectiveName);
}


//============================================================================
void CDockManager::savePerspectives(QSettings& Settings) const
{
	Settings.beginWriteArray("Perspectives", d->Perspectives.size());
	int i = 0;
	for (auto it = d->Perspectives.constBegin(); it != d->Perspectives.constEnd(); ++it)
	{
		Settings.setArrayIndex(i);
		Settings.setValue("Name", it.key());
		Settings.setValue("State", it.value());
		++i;
	}
	Settings.endArray();
}


//============================================================================
void CDockManager::loadPerspectives(QSettings& Settings)
{
	d->Perspectives.clear();
	int Size = Settings.beginReadArray("Perspectives");
	if (!Size)
	{
		Settings.endArray();
		return;
	}

	for (int i = 0; i < Size; ++i)
	{
		Settings.setArrayIndex(i);
		QString Name = Settings.value("Name").toString();
		QByteArray Data = Settings.value("State").toByteArray();
		if (Name.isEmpty() || Data.isEmpty())
		{
			continue;
		}

		d->Perspectives.insert(Name, Data);
	}

	Settings.endArray();
}

//============================================================================
QAction* CDockManager::addToggleViewActionToMenu(QAction* ToggleViewAction,
	const QString& Group, const QIcon& GroupIcon)
{
	bool AlphabeticallySorted = (MenuAlphabeticallySorted == d->MenuInsertionOrder);
	if (!Group.isEmpty())
	{
		QMenu* GroupMenu = d->ViewMenuGroups.value(Group, 0);
		if (!GroupMenu)
		{
			GroupMenu = new QMenu(Group, this);
			GroupMenu->setIcon(GroupIcon);
			d->addActionToMenu(GroupMenu->menuAction(), d->ViewMenu, AlphabeticallySorted);
			d->ViewMenuGroups.insert(Group, GroupMenu);
		}

		d->addActionToMenu(ToggleViewAction, GroupMenu, AlphabeticallySorted);
		return GroupMenu->menuAction();
	}
	else
	{
		d->addActionToMenu(ToggleViewAction, d->ViewMenu, AlphabeticallySorted);
		return ToggleViewAction;
	}
}


//============================================================================
QMenu* CDockManager::viewMenu() const
{
	return d->ViewMenu;
}


//============================================================================
void CDockManager::setViewMenuInsertionOrder(eViewMenuInsertionOrder Order)
{
	d->MenuInsertionOrder = Order;
}


//===========================================================================
bool CDockManager::isRestoringState() const
{
	return d->RestoringState;
}


//===========================================================================
int CDockManager::startDragDistance()
{
	return QApplication::startDragDistance() * 1.5;
}


//===========================================================================
CDockManager::ConfigFlags CDockManager::configFlags() const
{
	return d->ConfigFlags;
}


//===========================================================================
void CDockManager::setConfigFlags(const ConfigFlags Flags)
{
	d->ConfigFlags = Flags;
}


} // namespace ads

//---------------------------------------------------------------------------
// EOF DockManager.cpp

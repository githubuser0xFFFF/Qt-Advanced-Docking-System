import os
import tempfile
import shutil
import atexit

from PyQt5.QtCore import pyqtSignal, QSettings, QObject
import PyQtAds as QtAds

from dockindockmanager import DockInDockManager
from dockindock import DockInDockWidget

GROUP_PREFIX = "Group"

def findWidget(name, managers: 'list[DockInDockManager]') -> QtAds.CDockWidget:
        for mgr in managers:
            widget = mgr.findDockWidget(name)
            if widget:
                return widget


class PerspectiveInfo:
    # Partially bypass ADS perspective management, store list here
    # and then ADS will only have one perspective loaded
    # this is because all docking widgets must exist when a perspective is loaded
    # we will guarantee that!
    
    settings = QSettings()
    groups: 'dict[str, list[str]]' = {}


class PerspectivesManager(QObject):
    perspectivesListChanged = pyqtSignal()
    openingPerspective = pyqtSignal()
    openedPerspective = pyqtSignal()

    def __init__(self, perspectives_folder):
        super().__init__()
        self.__perspectives_folder = perspectives_folder
        self.__perspectives = {}
        atexit.register(self.cleanup)
        
    def cleanup(self):
        for perspective in self.__perspectives.values():
            filename = perspective.settings.fileName()
            try:
                os.remove(filename)
            except FileNotFoundError:
                pass
            
    def perspectiveNames(self) -> 'list[str]':
        return self.__perspectives.keys()
        
    def addPerspective(self, name: str, widget: DockInDockWidget) -> None:
        if self.__perspectives_folder:
            self.__perspectives[name] = perspective = PerspectiveInfo()
            perspective.settings = self.getSettingsObject(self.getSettingsFileName(name, True))
            perspective.groups = widget.getManager().getGroupContents()
            
            # save perspective internally
            widget.getManager().addPerspectiveRec(name)
            # store it in QSettings object
            widget.getManager().savePerspectivesRec(perspective.settings)
            # remove internal perspectives
            widget.getManager().removePerspectives(widget.getManager().perspectiveNames())
            
        self.perspectivesListChanged.emit()
                
    def openPerspective(name: str, widget: DockInDockWidget) -> None:
        assert widget.getTopLevelDockWidget() == widget
        
        if self.__perspectives_folder:
            if name in self.__perspectives:
                self.openingPerspective.emit()
                
                if widget.canCreateNewGroups():
                    cur_groups = widget.getManager().allManagers(True, True)
                    for group in self.__perspectives[name].groups.keys():
                        found = False
                        for curgroup in cur_groups:
                            if curgroup.getPerspectiveGroupName() == group:
                                found = True
                                break
                        if not found:
                            group = DockInDockManager.getGroupNameFromPersistGroupName(group)
                            
                        # restore group in file but not in GUI yet
                        widget.createGroup(group, None)
                        
                    cur_groups = widget.getManager().allManagers(False, True)
                    for curgroup in cur_groups:
                        if curgroup.getPersistGroupName() not in self.__perspectives[name].groups.keys():
                            widget.destroyGroup(curgroup.parent())
                            
                managers = widget.getManager().allManagers(True, True)
                for group in self.__perspectives[name].groups().keys():
                    for mgr in managers:
                        if mgr.getPersistGroupName() == group:
                            for widget_name in self.__perspectives[name].groups[group]:
                                widget = findWidget(widget_name, [mgr])
                                if widget:
                                    pass # OK, widget is already in the good manager!
                                else:
                                    widget = findWidget(widget_name, managers)
                                    if widget:
                                        # move dock widget in the same group as it used to be when perspective was saved
                                        # this guarantee load/open perspectives will work smartly
                                        MoveDockWidgetAction.move(widget, mgr)
                                        
                 # internally load perspectives from QSettings
                widget.getManager().loadPerspectivesRec(self.__perspectives[name].settings)
                # load perspective (update GUI)
                widget.getManager().openPerspectiveRec(name)
                # remove internal perspectives
                widget.getManager().removePerspectives(widget.getManager().perspectiveNames())

                self.openedPerspective().emit()
        else:
            assert False
            
    def removePerspectives(self) -> None:
        self.__perspectives.clear()
        self.perspectivesListChanged.emit()
        
    def removePerspective(self, name: str) -> None:
        del self.__perspectives[name]
        self.perspectivesListChanged.emit()
        
    def getSettingsFileName(self, perspective: str, temp: bool) -> str:
        name = "perspectives.ini" if not perspective else f"perspectives_{perspective + '.tmp' if temp else perspective + '.ini'}"
        
        return os.path.join(self.__perspectives_folder, name)
        
    def getSettingsObject(self, file_path: str) -> QSettings:
        return QSettings(file_path, QSettings.IniFormat)
        
    def loadPerspectives(self) -> None:
        if self.__perspectives_folder:
            tempfile.mktemp(dir=self.__perspectives_folder)
            
            self.__perspectives.clear()
            
            main_settings = self.getSettingsObject(self.getSettingsFileName("", False))
            debug = main_settings.fileName()
            
            size = main_settings.beginReadArray("Perspectives")
            
            for i in range(0, size):
                main_settings.setArrayIndex(i)
                perspective = main_settings.value("Name")
                
                if perspective:
                    to_load = self.getSettingsFileName(perspective, False)
                    loaded = self.getSettingsFileName(perspective, True)
                    
                    try:
                        os.remove(loaded)
                    except FileNotFoundError:
                        pass
                    if not shutil.copy(to_load, loaded):
                        assert False
                        
                    self.__perspectives[perspective] = PerspectiveInfo()
                    self.__perspectives[perspective].settings = self.getSettingsObject(loaded)
                    
                    # load group info:
                    main_settings.beginGroup(GROUP_PREFIX)
                    for key in main_settings.allKeys():
                        self.__perspectives[perspective].groups[key] = main_settings.value(key)
                    main_settings.endGroup()
                else:
                    assert False
                    
            main_settings.endArray()
            
        self.perspectivesListChanged.emit()
        
    def savePerspectives(self) -> None:
        if self.__perspectives_folder:
            main_settings = self.getSettingsObject(self.getSettingsFileName("", False))
            
            # Save list of perspective and group organization
            main_settings.beginWriteArray("Perspectives", len(self.__perspectives))
            for i, perspective in enumerate(self.__perspectives.keys()):
                main_settings.setArrayIndex(i)
                main_settings.setValue("Name", perspective)
                main_settings.beginGroup(GROUP_PREFIX)
                for group in self.__perspectives[perspective].groups.keys():
                    main_settings.setValue(group, list(self.__perspectives[perspective].groups[group]))
                main_settings.endGroup()
            main_settings.endArray()
            
            # Save perspectives themselves
            for perspective_name in self.__perspectives.keys():
                to_save = self.getSettingsFileName(perspective_name, False)
                settings = self.__perspectives[perspective_name].settings
                settings.sync()

                try:
                    os.remove(to_save)
                except FileNotFoundError:
                    pass
                if not shutil.copy(settings.fileName(), to_save):
                    assert False

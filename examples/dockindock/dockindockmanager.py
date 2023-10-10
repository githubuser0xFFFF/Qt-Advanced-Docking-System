from PyQt5.QtWidgets import QAction, QMenu, QInputDialog, QLineEdit
from PyQt5.QtCore import QSettings

import PyQtAds as QtAds

CHILD_PREFIX = "Child-"

class DockInDockManager(QtAds.CDockManager):
    def __init__(self, parent: 'DockInDockWidget'):
        super().__init__()
        self.__parent = parent

    def parent(self) -> 'DockInDockWidget':
        return self.__parent
    
    def fillViewMenu(self, menu: QMenu, move_to: 'dict[DockInDockManager]') -> None:
        from dockindock import DockInDockWidget # Prevent cyclic import
        
        widgets_map = self.dockWidgetsMap()
        for key, value in widgets_map.items():
            widget = value.widget()
            action = value.toggleViewAction()
            
            if isinstance(widget, DockInDockWidget):
                sub_menu = menu.addMenu(key)
                
                sub_menu.addAction(action)
                sub_menu.addSeparator()
                
                widget.setupMenu(sub_menu, move_to)
            else:
                menu.addAction(action)
                
        if self.parent().canCreateNewGroups():
            # see how this works, to create it in the right place, 
            # and also to have load perspective work when some groups are missing
            menu.addSeparator()
            menu.addAction(CreateChildDockAction(self.__parent, menu))

            if self.parent().getTopLevelDockWidget().getManager() != self:
                menu.addAction(DestroyGroupAction( self.parent, menu))
                
    def fillMoveMenu(self, menu: QMenu, move_to: 'list[DockInDockManager]') -> None:
        widgets_map = self.dockWidgetsMap()
        for key, value in widgets_map.items():
            sub_menu = menu.addMenu(key)
            
            for mgr in move_to:
                # iterate over all possible target managers
                if mgr == self:
                    pass # if dock is already in mgr, no reason to move it there
                elif mgr == DockInDockManager.dockInAManager(value):
                    pass # if target is the group itself, can't move it there, would make no sense
                else:
                    sub_menu.addAction(MoveDockWidgetAction(value, mgr, sub_menu))
    
    def addPerspectiveRec(self, name: str) -> None:
        managers = self.allManagers(True, True)
        
        for child in managers:
            child.addPerspective(name)
            
    def openPerspectiveRec(self, name: str) -> None:
        managers = self.allManagers(True, True)
        
        for child in managers:
            child.openPerspective(name)
            
    def getGroupName(self) -> str:
        return self.parent().objectName()
        
    def getPersistGroupName(self) -> str:
        group = "Top"
        if self.getGroupName():
            group = CHILD_PREFIX + self.getGroupName()
        return group
        
    def getGroupNameFromPersistGroupName(self, persist_group_name) -> str:
        if persist_group_name.startswith(CHILD_PREFIX):
            persist_group_name = persist_group_name[len(CHILD_PREFIX):]
        else:
            assert False
        return persist_group_name
        
    def loadPerspectivesRec(self, settings: QSettings) -> None:
        children = self.allManagers(True, True)
        
        for mgr in children:
            settings.beginGroup(mgr.getPersistGroupName())
            mgr.loadPerspectives(settings)
            settings.endGroup()
            
    def savePerspectivesRec(self, settings: QSettings) -> None:
        children = self.allManagers(True, True)
        
        for mgr in children:
            settings.beginGroup(mgr.getPersistGroupName())
            mgr.savePerspectives(settings)
            settings.endGroup()
            
    def removePerspectivesRec(self, settings: QSettings) -> None:
        children = self.allManagers(True, True)
        
        for mgr in children:
            child.removePerspectives(child.perspectiveNames())
            
    @staticmethod
    def dockInAManager(widget) -> 'DockInDockManager':
        from dockindock import DockInDockWidget # Prevent cyclic import
        
        dock_widget =  widget.widget() if widget else None
        return dock_widget.getManager() if isinstance(dock_widget, DockInDockWidget) else None
        
    def childManagers(self, managers: 'list[DockInDockManager]', rec: bool) -> None:
        widgets = self.getWidgetsInGUIOrder()
        for widget in widgets:
            as_mgr = DockInDockManager.dockInAManager(widget)
            if as_mgr:
                managers.append(as_mgr)
                if rec:
                    as_mgr.childManagers(managers, rec)
                    
    def allManagers(self, include_self: bool, rec: bool) -> 'list[DockInDockManager]':
        managers = []
        if include_self:
            managers.append(self)
        self.childManagers(managers, rec)
        return managers
        
    def allDockWidgets(self, include_self: bool, rec: bool) -> 'list[tuple[DockInDockManager, QtAds.CDockWidget]]':
        widgets = []
        for mgr in self.allManagers(include_self, rec):
            for widget in mgr.getWidgetsInGUIOrder():
                widgets.append((mgr, widget))
        return widgets
        
    def getGroupContents(self) -> 'dict[str, list[str]]':
        result = {}
        managers = self.allManagers(True, True)
        for mgr in managers:
            result[mgr.getPersistGroupName()] = mgr.dockWidgetsMap().keys()
        return result
        
    def getInsertDefaultPos(self) -> QtAds.CDockAreaWidget:
        default_pos = None
        if self.dockAreaCount() != 0:
            default_pos = self.dockArea(self.dockAreaCount()-1)
        return default_pos
        
    def getWidgetsInGUIOrder(self) -> 'list[QtAds.CDockWidget]':
        result = []
        for i in range(self.dockAreaCount()):
            for widget in self.dockArea(i).dockWidgets():
                result.append(widget)
        return result
        
    
class CreateChildDockAction(QAction):
    def __init__(self, dock_in_dock: 'DockInDockWidget', menu: QMenu):
        super().__init__("New group...", menu)
        self.__dock_in_dock = dock_in_dock
        self.triggered.connect(self.createGroup)
        
    def createGroup(self) -> None:
        name = ""
        while True:
            name, ok = QInputDialog.getText(None, self.text(), "Enter group name", QLineEdit.Normal, name)
            if ok:
                error = ""
                if self.__dock_in_dock.getTopLevelDockWidget():
                    error = self.__dock_in_dock.getTopLevelDockWidget().getGroupNameError(name)
                else:
                    assert False
                    
                if not error:
                    self.__dock_in_dock.createGroup(name, None)
                    break
                else:
                    QMessageBox.critical(None, "Error", error)
                    continue
            else:
                break
        
class DestroyGroupAction(QAction):
    def __init__(self, widget: 'DockInDockWidget', menu: QMenu):
        super().__init__("Destroy" + widget.getManager().getGroupName(), menu)
        self.__widget = widget
        self.triggered.connect(self.destroyGroup)
        
    def destroyGroup(self) -> None:
        self.__widget.getTopLevelDockWidget().destroyGroup(self.__widget)
        
        
class MoveDockWidgetAction(QAction):
    def __init__(self, widget: 'DockInDockWidget', move_to: DockInDockManager, menu: QMenu):
        super().__init__(menu)
        self.__widget = widget
        self.__move_to = move_to
        
        if move_to.parent().isTopLevel():
            self.setText("To top")
        else:
            self.setText(f"To {move_to.parent().objectName()}")
        self.triggered.connect(self._move)
        
    def _move(self) -> None:
        self.move(self.__widget, self.__move_to)
        
    def move(self, widget: QtAds.CDockWidget, move_to: QtAds.CDockManager) -> None:
        if widget and move_to:
            widget.dockManager().removeDockWidget(widget)
            move_to.addDockWidget(QtAds.CenterDockWidgetArea, widget, move_to.getInsertDefaultPos())
        else:
            assert False

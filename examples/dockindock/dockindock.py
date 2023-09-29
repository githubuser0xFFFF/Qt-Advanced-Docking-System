import sys

from PyQt5.QtWidgets import (QApplication, QWidget, QVBoxLayout, QMessageBox,
                             QInputDialog, QMenu, QLineEdit)
from PyQt5.QtGui import QIcon
import PyQtAds as QtAds

from dockindockmanager import DockInDockManager
from perspectiveactions import LoadPerspectiveAction, RemovePerspectiveAction


class DockInDockWidget(QWidget):
    def __init__(self, parent, perspectives_manager: 'PerspectivesManager', can_create_new_groups: bool = False, top_level_widget = None):
        super().__init__(parent)
        
        if top_level_widget is not None:
            self.__can_create_new_groups = top_level_widget.can_create_new_groups
        else:
            self.__can_create_new_groups = can_create_new_groups
        self.__top_level_dock_widget = top_level_widget if top_level_widget else self
        self.__perspectives_manager = perspectives_manager        
        self.__new_perspective_default_name: str = ''
        
        layout = QVBoxLayout(self)
        layout.setContentsMargins(0,0,0,0)
        self.__mgr = DockInDockManager(self)
        layout.addWidget(self.__mgr)
        
    def getManager(self) -> 'DockInDockManager':
        return self.__mgr
        
    def getTopLevelDockWidget(self) -> 'DockInDockWidget':
        return self.__top_level_dock_widget
        
    def canCreateNewGroups(self) -> bool:
        return self.__can_create_new_groups
        
    def getPerspectivesManager(self) -> 'PerspectivesManager':
        return self.__perspectives_manager
        
    def addTabWidget(self, widget: QWidget, name: str, after: QtAds.CDockAreaWidget, icon = QIcon()) -> QtAds.CDockAreaWidget:
        for existing in self.getTopLevelDockWidget().getManager().allDockWidgets(True, True):
            if existing[1].objectName() == name:
                QMessageBox.critical(self, "Error", "Name '" + name + "' already in use")
                return
                
        dock_widget = QtAds.CDockWidget(name)
        dock_widget.setWidget(widget)
        dock_widget.setIcon(icon)
        
        # Add the dock widget to the top dock widget area
        return self.__mgr.addDockWidget(QtAds.CenterDockWidgetArea, dock_widget, after)
        
    def isTopLevel(self) -> bool:
        return not self.objectName()
        
    def getGroupNameError(self, group_name: str) -> str:
        if not group_name:
            return "Group must have a non-empty name"
            
        dock_managers = self.__mgr.allManagers(True, True)
        for mgr in dock_managers:
            if mgr.getGroupName() == group_name:
                return "Group name '" + group_name + "' already used"
                
        return ""
        
    def createGroup(self, group_name: str, insert_pos: QtAds.CDockAreaWidget, icon = QIcon()) -> 'DockInDockWidget':
        error = self.getGroupNameError(group_name)
        if error:
            QMessageBox.critical(None, "Error", error)
            return
            
        child = DockInDockWidget(self, self.__top_level_dock_widget, self.__perspectives_manager)
        child.setObjectName(group_name)
        
        dock_widget = QtAds.CDockWidget(group_name)
        dock_widget.setWidget(child)
        dock_widget.setIcon(icon)
        
        insert_pos = self.__mgr.addDockWidget(QtAds.CenterDockWidgetArea, dock_widget, insert_pos)

        return child, insert_pos
        
    def destroyGroup(self, widget_to_remove: 'DockInDockWidget') -> None:
        top_level_widget = widget_to_remove.getTopLevelDockWidget()
        
        if top_level_widget and top_level_widget != widget_to_remove:
            for dock_widget in widget_to_remove.getManager().getWidgetsInGUIOrder(): #don't use allDockWidgets to preserve sub-groups
                MoveDockWidgetAction.move(dock_widget, top_level_widget.getManager())
            assert not widget_to_remove.getManager().allDockWidgets(True, True)

            # find widget's parent:
            for dock_widget in top_level_widget.getManager().allDockWidgets(True, True):
                if dockwidget[1].widget() == widget_to_remove:
                    dockwidget[0].removeDockWidget(dockwidget[1])
                    del dockwidget[1]
                    # delete widgetToRemove; automatically deleted when dockWidget is deleted
                    widget_to_remove = None
                    break

            assert widget_to_remove == None
        else:
            assert False
            
    def attachViewMenu(self, menu: QMenu) -> None:
        menu.aboutToShow.connect(self.autoFillAttachedViewMenu)
        
    def autoFillAttachedViewMenu(self) -> None:
        menu = self.sender()
        
        if menu:
            menu.clear()
            self.setupViewMenu(menu)
        else:
            assert False
            
    def setupViewMenu(self, menu):
        dock_managers = self.__mgr.allManagers(True, True)
        
        has_perspectives_menu = False
        if self.getTopLevelDockWidget() == self:
            has_perspectives_menu = (self.__perspectives_manager != None)
        else:
            assert False
            
        organize = menu
        if has_perspectives_menu:
            organize = menu.addMenu("Organize")
            
        self.setupMenu(organize, dock_managers)
        
        if has_perspectives_menu:
            perspectives = menu.addMenu("Perspectives")
            self.fillPerspectivesMenu(perspectives)
            
    def setupMenu(self, menu: QMenu, move_to: 'list[DockInDockManager]') -> None:
        self.__mgr.fillViewMenu(menu, move_to)
        menu.addSeparator()
        move_menu = menu.addMenu("Move")
        self.__mgr.fillMoveMenu(move_menu, move_to)
            
    def fillPerspectivesMenu(self, menu: QMenu):
        menu.addAction("Create perspective...", self.createPerspective)
        perspectives_names = []
        if self.__perspectives_manager:
            perspectives_names = self.__perspectives_manager.perspectiveNames()
            
        if perspectives_names:
            load = menu.addMenu("Load perspective")
            for name in perspectives_names:
                load.addAction(LoadPerspectiveAction(load, name, self))
            remove = menu.addMenu("Remove perspective")
            for name in perspectives_names:
                remove.addAction(RemovePerspectiveAction(remove, name, self))
                
    def setNewPerspectiveDefaultName(default_name: str) -> None:
        self.__new_perspective_default_name = default_name
        
    def createPerspective(self) -> None:
        if not self.__perspectives_manager:
            return
            
        name = self.__new_perspective_default_name
        if self.__new_perspective_default_name:
            index = 2
            while name in self.__perspectives_manager.perspectiveNames():
                name = f"{self.__new_perspective_default_name}({index})"
                index += 1
                
        while True:
            name, ok = QInputDialog.getText(None, "Create perspective", "Enter perspective name", QLineEdit.Normal, name)
            if ok:
                if not name:
                    QMessageBox.critical(None, "Error", "Perspective name cannot be empty")
                    continue
                elif name in self.__perspectives_manager.perspectiveNames():
                    if QMessageBox.critical(None, "Error", f"Perspective '{name}' already exists, overwrite it?", QMessageBox.Yes | QMessageBox.No, QMessageBox.No) == QMessageBox.No:
                        continue
                    
                self.__perspectives_manager.addPerspective(name, self)
                break
            else:
                break
                
    def dumpStatus(self, echo: callable = print, widget: QtAds.CDockWidget = None, tab: str = '', suffix: str = '') -> str:
        if widget is not None:
            as_mgr = DockInDockManager.dockInAManager(widget)
            if as_mgr:
                as_mgr.parent().dumpStatus(tab=tab)
            else:
                echo(tab + widget.objectName() + suffix)
        else:
            echo(tab + "Group: " + self.getManager().getGroupName())
            tab += "   "
            visible_widgets = set()
            for widget in self.getManager().getWidgetsInGUIOrder():
                visible_widgets.add(widget)
                self.dumpStatus(widget=widget, tab=tab)
                
            for closed in self.getManager().dockWidgetsMap().values():
                if not closed in visible_widgets:
                    self.dumpStatus(widget=closed, tab=tab, suffix=" (closed)")

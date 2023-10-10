import sys
import os

from PyQt5 import uic
from PyQt5.QtCore import Qt, QSignalBlocker
from PyQt5.QtWidgets import (QApplication, QMainWindow, QLabel, QComboBox, QTableWidget,
                             QAction, QWidgetAction, QSizePolicy, QInputDialog)
from PyQt5.QtGui import QCloseEvent
import PyQtAds as QtAds

    
UI_FILE = os.path.join(os.path.dirname(__file__), 'mainwindow.ui')
MainWindowUI, MainWindowBase = uic.loadUiType(UI_FILE)


class CMainWindow(MainWindowUI, MainWindowBase):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.setupUi(self)

        QtAds.CDockManager.setConfigFlag(QtAds.CDockManager.OpaqueSplitterResize, True)
        QtAds.CDockManager.setConfigFlag(QtAds.CDockManager.XmlCompressionEnabled, False)
        QtAds.CDockManager.setConfigFlag(QtAds.CDockManager.FocusHighlighting, True)
        self.dock_manager = QtAds.CDockManager(self)
        
        # Set central widget
        label = QLabel()
        label.setText("This is a DockArea which is always visible, even if it does not contain any DockWidgets.")
        label.setAlignment(Qt.AlignCenter)
        central_dock_widget = QtAds.CDockWidget("CentralWidget")
        central_dock_widget.setWidget(label)
        central_dock_widget.setFeature(QtAds.CDockWidget.NoTab, True)
        central_dock_area = self.dock_manager.setCentralWidget(central_dock_widget)

        # create other dock widgets
        table = QTableWidget()
        table.setColumnCount(3)
        table.setRowCount(10)
        table_dock_widget = QtAds.CDockWidget("Table 1")
        table_dock_widget.setWidget(table)
        table_dock_widget.setMinimumSizeHintMode(QtAds.CDockWidget.MinimumSizeHintFromDockWidget)
        table_dock_widget.resize(250, 150)
        table_dock_widget.setMinimumSize(200,150)
        self.dock_manager.addDockWidgetTabToArea(table_dock_widget, central_dock_area)
        table_area = self.dock_manager.addDockWidget(QtAds.DockWidgetArea.LeftDockWidgetArea, table_dock_widget)
        self.menuView.addAction(table_dock_widget.toggleViewAction())

        table = QTableWidget()
        table.setColumnCount(5)
        table.setRowCount(1020)
        table_dock_widget = QtAds.CDockWidget("Table 2")
        table_dock_widget.setWidget(table)
        table_dock_widget.setMinimumSizeHintMode(QtAds.CDockWidget.MinimumSizeHintFromDockWidget)
        table_dock_widget.resize(250, 150)
        table_dock_widget.setMinimumSize(200,150)
        self.dock_manager.addDockWidget(QtAds.DockWidgetArea.BottomDockWidgetArea, table_dock_widget, table_area)
        self.menuView.addAction(table_dock_widget.toggleViewAction())

        properties_table = QTableWidget()
        properties_table.setColumnCount(3)
        properties_table.setRowCount(10)
        properties_dock_widget = QtAds.CDockWidget("Properties")
        properties_dock_widget.setWidget(properties_table)
        properties_dock_widget.setMinimumSizeHintMode(QtAds.CDockWidget.MinimumSizeHintFromDockWidget)
        properties_dock_widget.resize(250, 150)
        properties_dock_widget.setMinimumSize(200,150)
        self.dock_manager.addDockWidget(QtAds.DockWidgetArea.RightDockWidgetArea, properties_dock_widget, central_dock_area)
        self.menuView.addAction(properties_dock_widget.toggleViewAction())

        self.createPerspectiveUi()
        
    def createPerspectiveUi(self):
        save_perspective_action = QAction("Create Perspective", self)
        save_perspective_action.triggered.connect(self.savePerspective)
        perspective_list_action = QWidgetAction(self)
        self.perspective_combo_box = QComboBox(self)
        self.perspective_combo_box.setSizeAdjustPolicy(QComboBox.AdjustToContents)
        self.perspective_combo_box.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        self.perspective_combo_box.activated[str].connect(self.dock_manager.openPerspective)
        perspective_list_action.setDefaultWidget(self.perspective_combo_box)
        self.toolBar.addSeparator()
        self.toolBar.addAction(perspective_list_action)
        self.toolBar.addAction(save_perspective_action)
        
    def savePerspective(self):
        perspective_name, ok = QInputDialog.getText(self, "Save Perspective", "Enter unique name:")
        if not perspective_name or not ok:
            return
        
        self.dock_manager.addPerspective(perspective_name)
        blocker = QSignalBlocker(self.perspective_combo_box)
        self.perspective_combo_box.clear()
        self.perspective_combo_box.addItems(self.dock_manager.perspectiveNames())
        self.perspective_combo_box.setCurrentText(perspective_name)
        
    def closeEvent(self, event: QCloseEvent):
        # Delete dock manager here to delete all floating widgets. This ensures
        # that all top level windows of the dock manager are properly closed
        self.dock_manager.deleteLater()
        super().closeEvent(event)
        
if __name__ == '__main__':
    app = QApplication(sys.argv)
    
    w = CMainWindow()
    w.show()
    app.exec_()

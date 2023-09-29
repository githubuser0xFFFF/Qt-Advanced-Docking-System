import sys
import os
import atexit

from PyQt5.QtWidgets import QApplication, QMainWindow, QLabel
from PyQt5.QtCore import Qt
import PyQtAds as QtAds

from perspectives import PerspectivesManager
from dockindock import DockInDockWidget
    
class MainWindow(QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        
        self.perspectives_manager = PerspectivesManager("persist")
        self.resize(400, 400)
        self.dock_manager = DockInDockWidget(self, self.perspectives_manager, can_create_new_groups=True)
        self.setCentralWidget(self.dock_manager)
        self.dock_manager.attachViewMenu(self.menuBar().addMenu("View"))

        previous_dock_widget = None
        for i in range(3):
            l = QLabel()
            l.setWordWrap(True)
            l.setAlignment(Qt.AlignTop | Qt.AlignLeft)
            l.setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ")
            
            previous_dock_widget = self.dock_manager.addTabWidget(l, f"Top label {i}", previous_dock_widget)
            
        last_top_level_dock = previous_dock_widget
        
        for j in range(2):
            group_manager, _ = self.dock_manager.createGroup(f"Group {j}", last_top_level_dock)
            
            previous_dock_widget = None
            
            for i in range(3):
                # Create example content label - this can be any application specific widget
                l = QLabel()
                l.setWordWrap(True)
                l.setAlignment(Qt.AlignTop | Qt.AlignLeft)
                l.setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ")

                previous_dock_widget = group_manager.addTabWidget(l, f"ZInner {j}/{i}", previous_dock_widget)
                
            # create sub-group
            sub_group, _ = group_manager.createGroup(f"SubGroup {j}", previous_dock_widget)
            previous_dock_widget = None
            for i in range(3):
                # Create example content label - this can be any application specific widget
                l = QLabel()
                l.setWordWrap(True)
                l.setAlignment(Qt.AlignTop | Qt.AlignLeft)
                l.setText("Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ")

                previous_dock_widget = sub_group.addTabWidget(l, f"SubInner {j}/{i}", previous_dock_widget)
                
        self.perspectives_manager.loadPerspectives()
        
        atexit.register(self.cleanup)
        
    def cleanup(self):
        self.perspectives_manager.savePerspectives()

        
if __name__ == '__main__':
    app = QApplication(sys.argv)
    
    w = MainWindow()
    w.show()
    app.exec_()

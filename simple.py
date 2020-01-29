import logging

from PyQt5 import QtWidgets, QtCore
from PyQt5.QtCore import Qt
from PyQtAds import QtAds


class MainWindow(QtWidgets.QMainWindow):
    def __init__(self, parent=None):
        super().__init__(parent)
        self.setup_ui()
        self.dock_manager = QtAds.CDockManager(self)

        self.dock_widgets = []

        for label_text, area in (
                ('1 Top', QtAds.TopDockWidgetArea),
                ('2 Bottom', QtAds.BottomDockWidgetArea),
                ('3 Left', QtAds.LeftDockWidgetArea),
                ('4 Right', QtAds.RightDockWidgetArea),
        ):
            # Create example content label - this can be any application specific
            # widget
            label = QtWidgets.QLabel()
            label.setWordWrap(True)
            label.setAlignment(Qt.AlignTop | Qt.AlignLeft)
            label.setText(f"{label_text}: Lorem ipsum dolor sit amet, consectetuer adipiscing elit. ")

            # Create a dock widget with the title Label 1 and set the created label
            # as the dock widget content
            dock_widget = QtAds.CDockWidget(label_text)
            dock_widget.setWidget(label)
            self.dock_widgets.append(dock_widget)

            # Add the toggleViewAction of the dock widget to the menu to give
            # the user the possibility to show the dock widget if it has been closed
            self.menu_view.addAction(dock_widget.toggleViewAction())

            # Add the dock widget to the top dock widget area
            self.dock_manager.addDockWidget(area, dock_widget)

    def setup_ui(self):
        self.setWindowTitle("MainWindow")
        self.setObjectName("MainWindow")
        self.resize(400, 300)
        self.central_widget = QtWidgets.QWidget(self)
        self.central_widget.setObjectName("central_widget")
        self.setCentralWidget(self.central_widget)

        self.menu_bar = QtWidgets.QMenuBar(self)
        self.menu_bar.setGeometry(QtCore.QRect(0, 0, 400, 21))
        self.menu_bar.setObjectName("menuBar")

        self.menu_view = QtWidgets.QMenu(self.menu_bar)
        self.menu_view.setObjectName("menu_view")
        self.menu_view.setTitle("View")
        self.setMenuBar(self.menu_bar)

        self.status_bar = QtWidgets.QStatusBar(self)
        self.status_bar.setObjectName("statusBar")
        self.setStatusBar(self.status_bar)
        self.menu_bar.addAction(self.menu_view.menuAction())


def main(app):
    main = MainWindow()
    main.show()
    state = main.dock_manager.saveState()
    print('This is what the saved state looks like in XML:')
    print(state)
    print()
    main.dock_manager.restoreState(state)
    return main


if __name__ == '__main__':
    logging.basicConfig(level='DEBUG')
    app = QtWidgets.QApplication([])
    window = main(app)
    window.show()
    print('shown')
    app.exec_()

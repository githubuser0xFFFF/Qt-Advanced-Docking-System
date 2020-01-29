import datetime
import logging

from PyQt5.QtCore import (QCoreApplication, QDir, Qt, QSettings, QSignalBlocker,
                         QRect)
from PyQt5.QtGui import QGuiApplication
from PyQt5.QtWidgets import (QCalendarWidget, QFileSystemModel, QFrame, QLabel,
                            QMenu, QTreeView, QAction, QWidgetAction,
                            QComboBox, QStyle, QSizePolicy, QInputDialog)

from PyQt5 import QtWidgets

from PyQtAds import QtAds


class _State:
    label_count = 0
    calendar_count = 0
    file_system_count = 0


def create_long_text_label_dock_widget(view_menu: QMenu) -> QtAds.CDockWidget:
    '''
    Create long text label dock widget

    Parameters
    ----------
    view_menu : QMenu

    Returns
    -------
    value : QtAds.CDockWidget
    '''
    label = QLabel()
    label.setWordWrap(True)
    label.setAlignment(Qt.AlignTop | Qt.AlignLeft)
    label.setText('''\
Label {} {} - Lorem ipsum dolor sit amet, consectetuer
adipiscing elit.  Aenean commodo ligula eget dolor. Aenean massa. Cum
sociis natoque penatibus et magnis dis parturient montes, nascetur
ridiculus mus.  Donec quam felis, ultricies nec, pellentesque eu, pretium
quis, sem.  Nulla consequat massa quis enim. Donec pede justo, fringilla
vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut,
imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis
pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi.
Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu,
consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra
quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet.
    '''.format(_State.label_count, str(datetime.datetime.now())))
    _State.label_count += 1

    dock_widget = QtAds.CDockWidget("Label {}".format(_State.label_count))
    dock_widget.setWidget(label)

    view_menu.addAction(dock_widget.toggleViewAction())
    return dock_widget


def create_calendar_dock_widget(view_menu: QMenu) -> QtAds.CDockWidget:
    '''
    Create calendar dock widget

    Parameters
    ----------
    view_menu : QMenu

    Returns
    -------
    value : QtAds.CDockWidget
    '''
    widget = QCalendarWidget()

    dock_widget = QtAds.CDockWidget("Calendar {}".format(_State.calendar_count))
    _State.calendar_count += 1
    dock_widget.setWidget(widget)
    dock_widget.setToggleViewActionMode(QtAds.CDockWidget.ActionModeShow)
    view_menu.addAction(dock_widget.toggleViewAction())
    return dock_widget


def create_file_system_tree_dock_widget(view_menu: QMenu) -> QtAds.CDockWidget:
    '''
    Create file system tree dock widget

    Parameters
    ----------
    view_menu : QMenu

    Returns
    -------
    value : QtAds.CDockWidget
    '''
    widget = QTreeView()
    widget.setFrameShape(QFrame.NoFrame)

    m = QFileSystemModel(widget)
    m.setRootPath(QDir.currentPath())
    widget.setModel(m)

    dock_widget = QtAds.CDockWidget("Filesystem {}".format(_State.file_system_count))
    _State.file_system_count += 1
    dock_widget.setWidget(widget)
    view_menu.addAction(dock_widget.toggleViewAction())
    return dock_widget


class MainWindow(QtWidgets.QMainWindow):
    save_perspective_action: QAction
    perspective_list_action: QWidgetAction
    perspective_combo_box: QComboBox
    dock_manager: QtAds.CDockManager

    def __init__(self, parent=None):
        super().__init__(parent)
        self.save_perspective_action = None
        self.perspective_list_action = None
        self.perspective_combo_box = None
        self.dock_manager = None

        self.setup_ui()

        self.dock_manager = QtAds.CDockManager(self)
        self.perspective_combo_box.activated[str].connect(self.dock_manager.openPerspective)
        self.create_content()
        self.resize(800, 600)
        self.restore_state()
        self.restore_perspectives()

    def setup_ui(self):
        self.setObjectName("MainWindow")
        self.resize(400, 300)
        self.setDockOptions(QtWidgets.QMainWindow.AllowTabbedDocks)
        self.centralWidget = QtWidgets.QWidget(self)
        self.centralWidget.setObjectName("centralWidget")
        self.setCentralWidget(self.centralWidget)
        self.status_bar = QtWidgets.QStatusBar(self)
        self.status_bar.setObjectName("statusBar")
        self.setStatusBar(self.status_bar)
        self.menu_bar = QtWidgets.QMenuBar(self)
        self.menu_bar.setGeometry(QRect(0, 0, 400, 21))
        self.menu_bar.setObjectName("menuBar")
        self.menu_file = QtWidgets.QMenu(self.menu_bar)
        self.menu_file.setObjectName("menuFile")
        self.menu_view = QtWidgets.QMenu(self.menu_bar)
        self.menu_view.setObjectName("menuView")
        self.menu_about = QtWidgets.QMenu(self.menu_bar)
        self.menu_about.setObjectName("menuAbout")
        self.setMenuBar(self.menu_bar)
        self.tool_bar = QtWidgets.QToolBar(self)
        self.tool_bar.setObjectName("toolBar")
        self.addToolBar(Qt.TopToolBarArea, self.tool_bar)
        self.action_exit = QtWidgets.QAction(self)
        self.action_exit.setObjectName("actionExit")
        self.action_save_state = QtWidgets.QAction(self)
        self.action_save_state.setObjectName("actionSaveState")
        self.action_save_state.triggered.connect(self.saveState)

        self.action_restore_state = QtWidgets.QAction(self)
        self.action_restore_state.setObjectName("actionRestoreState")
        self.action_restore_state.triggered.connect(self.restore_state)

        self.menu_file.addAction(self.action_exit)
        self.menu_file.addAction(self.action_save_state)
        self.menu_file.addAction(self.action_restore_state)
        self.menu_bar.addAction(self.menu_file.menuAction())
        self.menu_bar.addAction(self.menu_view.menuAction())
        self.menu_bar.addAction(self.menu_about.menuAction())

        self.setWindowTitle("MainWindow")
        self.menu_file.setTitle("File")
        self.menu_view.setTitle("View")
        self.menu_about.setTitle("About")
        self.tool_bar.setWindowTitle("toolBar")
        self.action_exit.setText("Exit")
        self.action_save_state.setText("Save State")
        self.action_restore_state.setText("Restore State")
        self.create_actions()

    def create_actions(self):
        '''
        Creates the toolbar actions
        '''
        self.tool_bar.addAction(self.action_save_state)
        self.action_save_state.setIcon(self.style().standardIcon(QStyle.SP_DialogSaveButton))
        self.tool_bar.addAction(self.action_restore_state)
        self.action_restore_state.setIcon(self.style().standardIcon(QStyle.SP_DialogOpenButton))
        self.save_perspective_action = QAction("Save Perspective", self)
        self.save_perspective_action.triggered.connect(self.save_perspective)

        self.perspective_list_action = QWidgetAction(self)
        self.perspective_combo_box = QComboBox(self)
        self.perspective_combo_box.setSizeAdjustPolicy(QComboBox.AdjustToContents)
        self.perspective_combo_box.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred)
        self.perspective_list_action.setDefaultWidget(self.perspective_combo_box)
        self.tool_bar.addSeparator()
        self.tool_bar.addAction(self.perspective_list_action)
        self.tool_bar.addAction(self.save_perspective_action)

    def create_content(self):
        '''
        Fill the dock manager with dock widgets
        '''
        # Test container docking
        view_menu = self.menu_view
        dock_widget = create_calendar_dock_widget(view_menu)
        dock_widget.setIcon(self.style().standardIcon(QStyle.SP_DialogOpenButton))
        dock_widget.setFeature(QtAds.CDockWidget.DockWidgetClosable, False)
        self.dock_manager.addDockWidget(QtAds.LeftDockWidgetArea, dock_widget)
        self.dock_manager.addDockWidget(QtAds.LeftDockWidgetArea, create_long_text_label_dock_widget(view_menu))
        file_system_widget = create_file_system_tree_dock_widget(view_menu)
        tool_bar = file_system_widget.createDefaultToolBar()
        tool_bar.addAction(self.action_save_state)
        tool_bar.addAction(self.action_restore_state)
        self.dock_manager.addDockWidget(QtAds.BottomDockWidgetArea, file_system_widget)
        file_system_widget = create_file_system_tree_dock_widget(view_menu)
        tool_bar = file_system_widget.createDefaultToolBar()
        tool_bar.addAction(self.action_save_state)
        tool_bar.addAction(self.action_restore_state)
        file_system_widget.setFeature(QtAds.CDockWidget.DockWidgetMovable, False)
        top_dock_area = self.dock_manager.addDockWidget(QtAds.TopDockWidgetArea, file_system_widget)
        dock_widget = create_calendar_dock_widget(view_menu)
        dock_widget.setFeature(QtAds.CDockWidget.DockWidgetClosable, False)
        dock_widget.setTabToolTip("Tab ToolTip\nHodie est dies magna")
        self.dock_manager.addDockWidget(QtAds.CenterDockWidgetArea, dock_widget, top_dock_area)

        # Test dock area docking
        right_dock_area = self.dock_manager.addDockWidget(
            QtAds.RightDockWidgetArea,
            create_long_text_label_dock_widget(view_menu), top_dock_area)
        self.dock_manager.addDockWidget(
            QtAds.TopDockWidgetArea,
            create_long_text_label_dock_widget(view_menu), right_dock_area)

        bottom_dock_area = self.dock_manager.addDockWidget(
            QtAds.BottomDockWidgetArea,
            create_long_text_label_dock_widget(view_menu), right_dock_area)

        self.dock_manager.addDockWidget(
            QtAds.RightDockWidgetArea,
            create_long_text_label_dock_widget(view_menu), right_dock_area)
        self.dock_manager.addDockWidget(
            QtAds.CenterDockWidgetArea,
            create_long_text_label_dock_widget(view_menu), bottom_dock_area)

    def save_state(self):
        '''
        Saves the dock manager state and the main window geometry
        '''
        settings = QSettings("Settings.ini", QSettings.IniFormat)
        settings.setValue("mainWindow/Geometry", self.saveGeometry())
        settings.setValue("mainWindow/State", self.saveState())
        settings.setValue("mainWindow/DockingState", self.dock_manager.saveState())

    def save_perspectives(self):
        '''
        Save the list of perspectives
        '''
        settings = QSettings("Settings.ini", QSettings.IniFormat)
        self.dock_manager.savePerspectives(settings)

    def restore_state(self):
        '''
        Restores the dock manager state
        '''
        settings = QSettings("Settings.ini", QSettings.IniFormat)
        geom = settings.value("mainWindow/Geometry")
        if geom is not None:
            self.restoreGeometry(geom)

        state = settings.value("mainWindow/State")
        if state is not None:
            self.restoreState(state)

        state = settings.value("mainWindow/DockingState")
        if state is not None:
            self.dock_manager.restore_state(state)

    def restore_perspectives(self):
        '''
        Restore the perspective listo of the dock manager
        '''
        settings = QSettings("Settings.ini", QSettings.IniFormat)
        self.dock_manager.loadPerspectives(settings)
        self.perspective_combo_box.clear()
        self.perspective_combo_box.addItems(self.dock_manager.perspectiveNames())

    def save_perspective(self):
        perspective_name, ok = QInputDialog.getText(self, 'Save perspective', 'Enter unique name:')
        if ok and perspective_name:
            self.dock_manager.addPerspective(perspective_name)
            _ = QSignalBlocker(self.perspective_combo_box)
            self.perspective_combo_box.clear()
            self.perspective_combo_box.addItems(self.dock_manager.perspectiveNames())
            self.perspective_combo_box.setCurrentText(perspective_name)
            self.save_perspectives()


def main(app_):
    main_window = MainWindow()
    main_window.show()
    state = main_window.dock_manager.saveState()
    # print('This is what the saved state looks like in XML:')
    # print(str(state, 'utf-8'))
    # print()
    # main_window.dock_manager.restore_state(state)
    return main_window


if __name__ == '__main__':
    # logging.basicConfig(level='DEBUG')
    QCoreApplication.setAttribute(Qt.AA_UseHighDpiPixmaps)
    QGuiApplication.setAttribute(Qt.AA_EnableHighDpiScaling)
    app = QtWidgets.QApplication([])
    window = main(app)
    window.show()
    app.exec_()

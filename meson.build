project('ads', 'cpp')
qt = import('qt6')
qt6_dep = dependency('qt6', modules:['Core', 'Gui', 'Widgets', 'Quick'], private_headers: true)
resources = qt.compile_resources(sources: 'src/ads.qrc')
deps = [qt6_dep]
headers = [
    'src/ads_globals.h',
    'src/DockAreaWidget.h',
    'src/DockAreaTabBar.h',
    'src/DockContainerWidget.h',
    'src/DockManager.h',
    'src/DockWidget.h',
    'src/DockWidgetTab.h',
    'src/DockingStateReader.h',
    'src/FloatingDockContainer.h',
    'src/FloatingDragPreview.h',
    'src/DockOverlay.h',
    'src/DockSplitter.h',
    'src/DockAreaTitleBar_p.h',
    'src/DockAreaTitleBar.h',
    'src/ElidingLabel.h',
    'src/IconProvider.h',
    'src/DockComponentsFactory.h',
    'src/DockFocusController.h',
    'src/AutoHideDockContainer.h',
    'src/AutoHideSideBar.h',
    'src/AutoHideTab.h',
    'src/PushButton.h',
    'src/ResizeHandle.h'
]


sources = [
    'src/ads_globals.cpp',
    'src/DockAreaWidget.cpp',
    'src/DockAreaTabBar.cpp',
    'src/DockContainerWidget.cpp',
    'src/DockManager.cpp',
    'src/DockWidget.cpp',
    'src/DockingStateReader.cpp',
    'src/DockWidgetTab.cpp',
    'src/FloatingDockContainer.cpp',
    'src/FloatingDragPreview.cpp',
    'src/DockOverlay.cpp',
    'src/DockSplitter.cpp',
    'src/DockAreaTitleBar.cpp',
    'src/ElidingLabel.cpp',
    'src/IconProvider.cpp',
    'src/DockComponentsFactory.cpp',
    'src/DockFocusController.cpp',
    'src/AutoHideDockContainer.cpp',
    'src/AutoHideSideBar.cpp',
    'src/AutoHideTab.cpp',
    'src/PushButton.cpp',
    'src/ResizeHandle.cpp'
]


if build_machine.system() == 'linux'
headers += 'src/linux/FloatingWidgetTitleBar.h'
sources += 'src/linux/FloatingWidgetTitleBar.cpp'
deps += dependency('xcb')
endif

moc_files = qt.compile_moc(headers: headers, include_directories: 'src/', dependencies: qt6_dep)

lib = library('qt6advanceddocking', sources, moc_files, resources,dependencies: deps, include_directories: 'src/')
qt6ads_dep = declare_dependency(link_with: lib, include_directories: 'src/')

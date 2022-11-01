cmake_minimum_required(VERSION 3.5)
project(QtAdvancedDockingSystem LANGUAGES CXX VERSION ${VERSION_SHORT})
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)
if (UNIX AND NOT APPLE)
    include_directories(${Qt${QT_VERSION_MAJOR}Gui_PRIVATE_INCLUDE_DIRS})
endif()
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(BUILD_STATIC)
    set(CMAKE_STATIC_LIBRARY_SUFFIX "_static${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()
set(ads_SRCS
    ads_globals.cpp
    DockAreaTabBar.cpp
    DockAreaTitleBar.cpp
    DockAreaWidget.cpp
    DockContainerWidget.cpp
    DockManager.cpp
    DockOverlay.cpp
    DockSplitter.cpp
    DockWidget.cpp
    DockWidgetTab.cpp
    DockingStateReader.cpp
    DockFocusController.cpp
    ElidingLabel.cpp
    FloatingDockContainer.cpp
    FloatingDragPreview.cpp
    IconProvider.cpp
    DockComponentsFactory.cpp
    AutoHideSideBar.cpp
    AutoHideTab.cpp
    AutoHideDockContainer.cpp
    PushButton.cpp
	ResizeHandle.cpp
    ads.qrc
)
set(ads_HEADERS
    ads_globals.h
    DockAreaTabBar.h
    DockAreaTitleBar.h
    DockAreaTitleBar_p.h
    DockAreaWidget.h
    DockContainerWidget.h
    DockManager.h
    DockOverlay.h
    DockSplitter.h
    DockWidget.h
    DockWidgetTab.h
    DockingStateReader.h
    DockFocusController.h
    ElidingLabel.h
    FloatingDockContainer.h
    FloatingDragPreview.h
    IconProvider.h
    DockComponentsFactory.h
    AutoHideSideBar.h
    AutoHideTab.h
    AutoHideDockContainer.h
    PushButton.h
	ResizeHandle.h
)
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
if (UNIX AND NOT APPLE)
    set(ads_SRCS linux/FloatingWidgetTitleBar.cpp ${ads_SRCS})
    set(ads_HEADERS linux/FloatingWidgetTitleBar.h ${ads_HEADERS})
endif()
if(BUILD_STATIC)
    add_library(qtadvanceddocking STATIC ${ads_SRCS} ${ads_HEADERS})
    target_compile_definitions(qtadvanceddocking PUBLIC ADS_STATIC)
else()
    add_library(qtadvanceddocking SHARED ${ads_SRCS} ${ads_HEADERS})
    target_compile_definitions(qtadvanceddocking PRIVATE ADS_SHARED_EXPORT)
endif()

add_library(ads::qtadvanceddocking ALIAS qtadvanceddocking)

target_link_libraries(qtadvanceddocking PUBLIC Qt${QT_VERSION_MAJOR}::Core 
                                               Qt${QT_VERSION_MAJOR}::Gui 
                                               Qt${QT_VERSION_MAJOR}::Widgets)
if (UNIX AND NOT APPLE)
  target_link_libraries(qtadvanceddocking PUBLIC xcb)
endif()
set_target_properties(qtadvanceddocking PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    CXX_EXTENSIONS OFF
    VERSION ${VERSION_SHORT}
    EXPORT_NAME "qtadvanceddocking"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/bin"
)
if(QT_VERSION_MAJOR STREQUAL "5")
    set_target_properties(qtadvanceddocking PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON)
elseif(QT_VERSION_MAJOR STREQUAL "6")
    set_target_properties(qtadvanceddocking PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON)
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "qtadvanceddockingConfigVersion.cmake"
    VERSION ${VERSION_SHORT}
    COMPATIBILITY SameMajorVersion
)
install(FILES ${ads_HEADERS}
    DESTINATION include
    COMPONENT headers
)
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE"
    "${CMAKE_CURRENT_SOURCE_DIR}/../gnu-lgpl-v2.1.md"
    DESTINATION license/ads
    COMPONENT license
)
install(TARGETS qtadvanceddocking
    EXPORT adsTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(EXPORT adsTargets
    FILE adsTargets.cmake
    NAMESPACE ads::
    DESTINATION lib/cmake/qtadvanceddocking
)
install(FILES qtadvanceddockingConfig.cmake "${CMAKE_CURRENT_BINARY_DIR}/qtadvanceddockingConfigVersion.cmake"
    DESTINATION lib/cmake/qtadvanceddocking
)

target_include_directories(qtadvanceddocking PUBLIC
    $<INSTALL_INTERFACE:include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

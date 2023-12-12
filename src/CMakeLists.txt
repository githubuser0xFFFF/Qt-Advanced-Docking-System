cmake_minimum_required(VERSION 3.5)
project(QtAdvancedDockingSystem LANGUAGES CXX VERSION ${VERSION_SHORT})
if (${QT_VERSION_MAJOR})
  message(STATUS "Forced to use Qt version ${QT_VERSION_MAJOR}")
  find_package(QT NAMES Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
else()
  find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
endif()
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

set(library_name "qt${QT_VERSION_MAJOR}advanceddocking")
if(BUILD_STATIC)
    add_library(${library_name} STATIC ${ads_SRCS} ${ads_HEADERS})
    target_compile_definitions( ${library_name} PUBLIC ADS_STATIC)
else()
    add_library( ${library_name} SHARED ${ads_SRCS} ${ads_HEADERS})
    target_compile_definitions( ${library_name} PRIVATE ADS_SHARED_EXPORT)
endif()

add_library(ads::${library_name} ALIAS ${library_name})

target_link_libraries(${library_name} PUBLIC Qt${QT_VERSION_MAJOR}::Core 
                                               Qt${QT_VERSION_MAJOR}::Gui 
                                               Qt${QT_VERSION_MAJOR}::Widgets)
if (UNIX AND NOT APPLE)
  if (${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
      find_package(X11 REQUIRED)
      target_link_libraries(${library_name} PUBLIC X11::xcb)
  else()
    target_link_libraries(${library_name} PUBLIC xcb)
  endif()
endif()
set_target_properties(${library_name} PROPERTIES
    AUTOMOC ON
    AUTORCC ON
    CXX_EXTENSIONS OFF
    VERSION ${VERSION_SHORT}
    EXPORT_NAME ${library_name}
    DEBUG_POSTFIX "d"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/bin"
)
if(QT_VERSION_MAJOR STREQUAL "5")
    set_target_properties(${library_name} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON)
elseif(QT_VERSION_MAJOR STREQUAL "6")
    set_target_properties(${library_name} PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON)
endif()

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    "${library_name}ConfigVersion.cmake"
    VERSION ${VERSION_SHORT}
    COMPATIBILITY SameMajorVersion
)
install(FILES ${ads_HEADERS}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${library_name}
    COMPONENT headers
)
install(FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/../LICENSE"
    "${CMAKE_CURRENT_SOURCE_DIR}/../gnu-lgpl-v2.1.md"
    DESTINATION license/ads
    COMPONENT license
)
install(TARGETS ${library_name}
    EXPORT adsTargets
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${library_name}
)

install(EXPORT adsTargets
    FILE adsTargets.cmake
    NAMESPACE ads::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${library_name}
)
install(FILES qtadvanceddockingConfig.cmake RENAME ${library_name}Config.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${library_name}
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${library_name}ConfigVersion.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${library_name}
)

target_include_directories(${library_name} PUBLIC
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

cmake_minimum_required(VERSION 3.5)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
include(GetGitRevisionDescription)
git_describe(GitTagVersion --tags)
string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GitTagVersion}")
string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GitTagVersion}")
string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GitTagVersion}")
set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
project(QtAdvancedDockingSystem LANGUAGES CXX VERSION ${VERSION_SHORT})
option(BUILD_STATIC "Build the static library" OFF)
option(BUILD_EXAMPLES "Build the examples" ON)
find_package(Qt5 5.5 COMPONENTS Core Gui Widgets REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
if(BUILD_STATIC)
    set(CMAKE_STATIC_LIBRARY_SUFFIX "_static${CMAKE_STATIC_LIBRARY_SUFFIX}")
endif()
set(ads_SRCS
    src/ads_globals.cpp
    src/DockAreaTabBar.cpp
    src/DockAreaTitleBar.cpp
    src/DockAreaWidget.cpp
    src/DockContainerWidget.cpp
    src/DockManager.cpp
    src/DockOverlay.cpp
    src/DockSplitter.cpp
    src/DockWidget.cpp
    src/DockWidgetTab.cpp
    src/DockingStateReader.cpp
    src/ElidingLabel.cpp
    src/FloatingDockContainer.cpp
    src/FloatingDragPreview.cpp
    src/IconProvider.cpp
    src/DockComponentsFactory.cpp
    src/ads.qrc
)
set(ads_INSTALL_INCLUDE 
    src/ads_globals.h
    src/DockAreaTabBar.h
    src/DockAreaTitleBar.h
    src/DockAreaTitleBar_p.h
    src/DockAreaWidget.h
    src/DockContainerWidget.h
    src/DockManager.h
    src/DockOverlay.h
    src/DockSplitter.h
    src/DockWidget.h
    src/DockWidgetTab.h
    src/DockingStateReader.h
    src/ElidingLabel.h
    src/FloatingDockContainer.h
    src/FloatingDragPreview.h
    src/IconProvider.h
    src/DockComponentsFactory.h
)
if (UNIX)
    set(ads_SRCS src/linux/FloatingWidgetTitleBar.cpp ${ads_SRCS})
    set(ads_INSTALL_INCLUDE src/linux/FloatingWidgetTitleBar.h ${ads_INSTALL_INCLUDE})
endif()
if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(ads_PlatformDir "x86")
else()
    set(ads_PlatformDir "x64")
endif()
if(BUILD_STATIC)
    add_library(qtadvanceddocking STATIC ${ads_SRCS})
    target_compile_definitions(qtadvanceddocking PUBLIC ADS_STATIC)
else()
    add_library(qtadvanceddocking SHARED ${ads_SRCS})
    target_compile_definitions(qtadvanceddocking PRIVATE ADS_SHARED_EXPORT)
endif()
target_link_libraries(qtadvanceddocking PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
set_target_properties(qtadvanceddocking PROPERTIES 
    AUTOMOC ON
    AUTORCC ON
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${VERSION_SHORT}
    EXPORT_NAME "QtAdvancedDockingSystem"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/bin"
)
include(CMakePackageConfigHelpers)
    write_basic_package_version_file("adsConfigVersion.cmake"
    VERSION ${VERSION_SHORT}
    COMPATIBILITY SameMajorVersion
)
install(FILES ${ads_INSTALL_INCLUDE}
    DESTINATION include 
    COMPONENT headers
)
install(FILES 
    "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE"
    "${CMAKE_CURRENT_SOURCE_DIR}/gnu-lgpl-v2.1.md"
    DESTINATION license 
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
    DESTINATION lib/cmake/ads
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/adsConfigVersion.cmake"
    DESTINATION lib/cmake/ads
)

target_include_directories(qtadvanceddocking PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR}/src/private>"
    $<INSTALL_INTERFACE:include>
)

if(BUILD_EXAMPLES)
    add_subdirectory(examples/simple)
    add_subdirectory(examples/sidebar)
    add_subdirectory(examples/deleteonclose)
    add_subdirectory(demo)
endif()


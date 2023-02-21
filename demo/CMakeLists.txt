cmake_minimum_required(VERSION 3.5)
project(ads_demo VERSION ${VERSION_SHORT}) 

find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} 5.5 COMPONENTS Core Gui Widgets REQUIRED)
if(WIN32 AND QT_VERSION_MAJOR LESS 6)
    find_package(Qt${QT_VERSION_MAJOR} COMPONENTS AxContainer REQUIRED)
endif()
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(ads_demo_SRCS
    main.cpp
    MainWindow.cpp
    mainwindow.ui
    StatusDialog.cpp
    StatusDialog.ui
    ImageViewer.cpp
    RenderWidget.cpp
	demo.qrc
)
add_executable(AdvancedDockingSystemDemo WIN32 ${ads_demo_SRCS})
target_include_directories(AdvancedDockingSystemDemo PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../src")
target_link_libraries(AdvancedDockingSystemDemo PUBLIC Qt${QT_VERSION_MAJOR}::Core 
                                                       Qt${QT_VERSION_MAJOR}::Gui 
                                                       Qt${QT_VERSION_MAJOR}::Widgets)
if(WIN32 AND QT_VERSION_MAJOR LESS 6)
    target_link_libraries(AdvancedDockingSystemDemo PUBLIC Qt${QT_VERSION_MAJOR}::AxContainer)
endif()
target_link_libraries(AdvancedDockingSystemDemo PRIVATE qtadvanceddocking)
set_target_properties(AdvancedDockingSystemDemo PROPERTIES 
    AUTOMOC ON
    AUTORCC ON
    AUTOUIC ON
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
    VERSION ${VERSION_SHORT}
    EXPORT_NAME "Qt Advanced Docking System Demo"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/bin"
)
#if(BUILD_STATIC)
#    target_compile_definitions(AdvancedDockingSystemDemo PRIVATE ADS_STATIC)
#endif()


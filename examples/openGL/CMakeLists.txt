cmake_minimum_required(VERSION 3.5)

project(OpenGLExample VERSION ${VERSION_SHORT})

find_package(
  QT NAMES Qt6
  COMPONENTS Core Gui Widgets Charts OpenGLWidgets
  REQUIRED)
find_package(
  Qt${QT_VERSION_MAJOR}
  COMPONENTS Core Gui Widgets Charts OpenGLWidgets
  REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(${PROJECT_NAME} WIN32 main.cpp MainWindow.cpp)

target_include_directories(${PROJECT_NAME}
                           PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../src")
target_link_libraries(${PROJECT_NAME}
                      PRIVATE qtadvanceddocking-qt${QT_VERSION_MAJOR})

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui
         Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts
         Qt${QT_VERSION_MAJOR}::OpenGLWidgets)

set_target_properties(
  ${PROJECT_NAME}
  PROPERTIES AUTOMOC ON
             AUTORCC ON
             AUTOUIC ON
             CXX_STANDARD 14
             CXX_STANDARD_REQUIRED ON
             CXX_EXTENSIONS OFF
             VERSION ${VERSION_SHORT}
             EXPORT_NAME "Qt Advanced Docking System OpenGL Example"
             ARCHIVE_OUTPUT_DIRECTORY
             "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
             LIBRARY_OUTPUT_DIRECTORY
             "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/lib"
             RUNTIME_OUTPUT_DIRECTORY
             "${CMAKE_BINARY_DIR}/${ads_PlatformDir}/bin")

# Define include directories
target_include_directories(
  ${PROJECT_NAME}
  PRIVATE $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>
          $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/examples/openGL>
          $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/${INSTALL_NAME}>)

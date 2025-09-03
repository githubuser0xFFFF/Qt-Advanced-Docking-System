cmake_minimum_required(VERSION 3.5)

project(OpenGLExample VERSION ${VERSION_SHORT})

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(
  QT NAMES Qt6 Qt5
  COMPONENTS Core
             Gui
             Widgets
             Charts
             OpenGLWidgets
             Quick
             QuickWidgets
             ShaderTools
  REQUIRED)

find_package(
  Qt${QT_VERSION_MAJOR} 5.5
  COMPONENTS Core
             Gui
             Widgets
             Charts
             OpenGLWidgets
             Quick
             QuickWidgets
             ShaderTools
  REQUIRED)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

qt_add_executable(
  ${PROJECT_NAME}
  WIN32
  main.cpp
  mainwindow.cpp
  mainwindow.h
  glwindow.cpp
  glwindow.h
  glwidget.h
  glwidget.cpp
  fbitem.cpp
  fbitem.h
  logo.cpp
  logo.h)

target_include_directories(${PROJECT_NAME}
                           PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../../src")
target_link_libraries(${PROJECT_NAME}
                      PRIVATE qtadvanceddocking-qt${QT_VERSION_MAJOR})

qt_add_qml_module(
  ${PROJECT_NAME}
  URI
  fbitem
  QML_FILES
  "test.qml"
  RESOURCE_PREFIX
  /openGL
  NO_RESOURCE_TARGET_PATH)

qt6_add_shaders(
  ${PROJECT_NAME}
  "shaders"
  PRECOMPILE
  OPTIMIZED
  PREFIX
  "/openGL"
  FILES
  "wobble.frag")

# Resources:
set(resource_files "qtlogo.png")

qt_add_resources(${PROJECT_NAME} "OpenGLExample" PREFIX "/" FILES
                 ${resource_files})

target_link_libraries(
  ${PROJECT_NAME}
  PUBLIC Qt${QT_VERSION_MAJOR}::Core
         Qt${QT_VERSION_MAJOR}::Gui
         Qt${QT_VERSION_MAJOR}::Widgets
         Qt${QT_VERSION_MAJOR}::Charts
         Qt${QT_VERSION_MAJOR}::OpenGLWidgets
         Qt${QT_VERSION_MAJOR}::Quick
         Qt${QT_VERSION_MAJOR}::QuickWidgets)

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

install(
  TARGETS ${PROJECT_NAME}
  BUNDLE DESTINATION .
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})

qt_generate_deploy_app_script(TARGET ${PROJECT_NAME} OUTPUT_SCRIPT
                              deploy_script NO_UNSUPPORTED_PLATFORM_ERROR)

install(SCRIPT ${deploy_script})

qt_generate_deploy_qml_app_script(
  TARGET
  ${PROJECT_NAME}
  OUTPUT_SCRIPT
  qml_deploy_script
  MACOS_BUNDLE_POST_BUILD
  NO_UNSUPPORTED_PLATFORM_ERROR
  DEPLOY_USER_QML_MODULES_ON_UNSUPPORTED_PLATFORM)

install(SCRIPT ${qml_deploy_script})

cmake_minimum_required(VERSION 3.16)

# [Wizard NLE fork] Test suite for fork-specific changes. Gated behind
# BUILD_TESTS in the root CMakeLists.txt so downstream consumers (FetchContent,
# distributors) don't pick it up by default.

# QT_VERSION_MAJOR isn't propagated from src/CMakeLists.txt scope; redo the
# version detection here so the test target can find Qt::Test independently.
if(DEFINED QT_VERSION_MAJOR AND QT_VERSION_MAJOR)
    find_package(QT NAMES Qt${QT_VERSION_MAJOR} COMPONENTS Core REQUIRED)
else()
    find_package(QT NAMES Qt6 Qt5 COMPONENTS Core REQUIRED)
endif()
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets Test REQUIRED)

set(library_name "qtadvanceddocking-qt${QT_VERSION_MAJOR}")

add_executable(QuadrantHitTestTest QuadrantHitTestTest.cpp)
set_target_properties(QuadrantHitTestTest PROPERTIES
    AUTOMOC ON
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)
target_link_libraries(QuadrantHitTestTest PRIVATE
    ${library_name}
    Qt${QT_VERSION_MAJOR}::Test
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Gui
    Qt${QT_VERSION_MAJOR}::Widgets
)
target_include_directories(QuadrantHitTestTest PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../src"
)

add_test(NAME QuadrantHitTestTest COMMAND QuadrantHitTestTest)

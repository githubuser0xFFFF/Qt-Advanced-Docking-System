cmake_minimum_required(VERSION 3.16)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
if (POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif (POLICY CMP0091)

# Add cmake modules path (required for Versioning.cmake)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

project(QtADS LANGUAGES CXX VERSION 5.0.0)

option(BUILD_STATIC "Build the static library" OFF)
option(BUILD_EXAMPLES "Build the examples" ON)

# Platform directory: auto-detected based on pointer size, or manually set
if(NOT ADS_PLATFORM_DIR)
    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
        set(ads_PlatformDir "x86")
    else()
        set(ads_PlatformDir "x64")
    endif()
else()
    set(ads_PlatformDir "${ADS_PLATFORM_DIR}")
endif()

add_subdirectory(src)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
    add_subdirectory(demo)
endif()


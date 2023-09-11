cmake_minimum_required(VERSION 3.5)

if (POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif (POLICY CMP0091)

# By default, the version information is extracted from the git index. However,
# we can override this behavior by explicitly setting ADS_VERSION and
# skipping the git checks. This is useful for cases where this project is being
# used independently of its original git repo (e.g. vendored in another project)
if(NOT ADS_VERSION)
    set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})
    include(GetGitRevisionDescription)
    git_describe(GitTagVersion --tags)
    string(REGEX REPLACE "^([0-9]+)\\..*" "\\1" VERSION_MAJOR "${GitTagVersion}")
    string(REGEX REPLACE "^[0-9]+\\.([0-9]+).*" "\\1" VERSION_MINOR "${GitTagVersion}")
    string(REGEX REPLACE "^[0-9]+\\.[0-9]+\\.([0-9]+).*" "\\1" VERSION_PATCH "${GitTagVersion}")
    set(VERSION_SHORT "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")
else()
    string(REGEX MATCHALL "[\.]" VERSION_DOT_MATCHES ${ADS_VERSION})
    list(LENGTH VERSION_DOT_MATCHES VERSION_DOT_COUNT)
    if(VERSION_DOT_COUNT EQUAL 2)
        set(VERSION_SHORT ${ADS_VERSION})
    else()
        message(FATAL_ERROR "ADS_VERSION must be in major.minor.patch format, e.g. 3.8.1. Got ${ADS_VERSION}")
    endif()
endif()


project(QtADS LANGUAGES CXX VERSION ${VERSION_SHORT})

option(BUILD_STATIC "Build the static library" OFF)
option(BUILD_EXAMPLES "Build the examples" ON)

if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(ads_PlatformDir "x86")
else()
    set(ads_PlatformDir "x64")
endif()

add_subdirectory(src)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
    add_subdirectory(demo)
endif()


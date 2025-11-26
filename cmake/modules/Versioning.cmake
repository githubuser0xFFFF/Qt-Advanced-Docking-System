# ============================================================
# Versioning.cmake  (Reusable Version + Resource Generator)
# ============================================================

# Freeze the directory where this module lives
# This is critical — it prevents paths from breaking when the
# function is invoked from other CMakeLists.txt files.
set(_VERSIONING_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "Versioning module directory")

# ------------------------------------------------------------
# Extract version information from Git
# ------------------------------------------------------------

# Get tag (expected: v1.2.3 or 1.2.3 or 1.2.3-12-gHASH)
execute_process(
    COMMAND git describe --tags --dirty
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_DESC_RAW
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Remove leading "v" if present
string(REGEX REPLACE "^v" "" GIT_DESC "${GIT_DESC_RAW}")

# Extract major.minor.patch
string(REGEX MATCH "^([0-9]+)\\.([0-9]+)\\.([0-9]+)" _ "${GIT_DESC}")
set(PROJECT_VERSION_MAJOR "${CMAKE_MATCH_1}")
set(PROJECT_VERSION_MINOR "${CMAKE_MATCH_2}")
set(PROJECT_VERSION_PATCH "${CMAKE_MATCH_3}")

set(PROJECT_VERSION_STRING
    "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
)

# Commit hash (full + short)
execute_process(
    COMMAND git rev-parse HEAD
    OUTPUT_VARIABLE PROJECT_GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process(
    COMMAND git rev-parse --short HEAD
    OUTPUT_VARIABLE PROJECT_GIT_HASH_SHORT
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# Export variables to parent scope
set(PROJECT_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}" PARENT_SCOPE)
set(PROJECT_VERSION_MINOR "${PROJECT_VERSION_MINOR}" PARENT_SCOPE)
set(PROJECT_VERSION_PATCH "${PROJECT_VERSION_PATCH}" PARENT_SCOPE)
set(PROJECT_VERSION_STRING "${PROJECT_VERSION_STRING}" PARENT_SCOPE)
set(PROJECT_GIT_HASH "${PROJECT_GIT_HASH}" PARENT_SCOPE)
set(PROJECT_GIT_HASH_SHORT "${PROJECT_GIT_HASH_SHORT}" PARENT_SCOPE)

# Public variable for users
set(PROJECT_AUTO_VERSION "${PROJECT_VERSION_STRING}" PARENT_SCOPE)

# ------------------------------------------------------------
# Reusable function: Attach Windows version resources to target
# ------------------------------------------------------------
function(add_windows_version_resources target)
    if(NOT WIN32)
        return()
    endif()

    if(NOT TARGET "${target}")
        message(FATAL_ERROR "add_windows_version_resources: target '${target}' not found.")
    endif()

    # Absolute path to the template (never changes)
    get_filename_component(_rc_in
        "${_VERSIONING_MODULE_DIR}/FileVersionInfo.rc.in"
        ABSOLUTE
    )

    if(NOT EXISTS "${_rc_in}")
        message(FATAL_ERROR
            "FileVersionInfo.rc.in missing at: ${_rc_in}"
        )
    endif()

    # Output in the caller's binary dir
    set(_rc_out "${CMAKE_CURRENT_BINARY_DIR}/${target}_version.rc")
    get_filename_component(_rc_out "${_rc_out}" ABSOLUTE)

    # Ensure directory exists
    file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")

    # Configure the template
    configure_file("${_rc_in}" "${_rc_out}" @ONLY)

    # Add to the target
    target_sources(${target} PRIVATE "${_rc_out}")
endfunction()

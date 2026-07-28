# ============================================================
# Versioning.cmake  (Reusable Version + Resource Generator)
# ============================================================

# Freeze the directory where this module lives
# This is critical — it prevents paths from breaking when the
# function is invoked from other CMakeLists.txt files.
set(_VERSIONING_MODULE_DIR "${CMAKE_CURRENT_LIST_DIR}" CACHE INTERNAL "Versioning module directory")

# ------------------------------------------------------------
# Function: Generates the required version header for the ADS
# library.  Should only be called by the ADS library target.
# ------------------------------------------------------------
function(generate_ads_version_header)
    # configure ads_version.h from ads_version.h.in
    if(NOT DEFINED QtADS_VERSION_MAJOR OR
       NOT DEFINED QtADS_VERSION_MINOR OR
       NOT DEFINED QtADS_VERSION_PATCH)
        message(
            FATAL_ERROR
            "ADS major, minor, or patch version variables not defined!")
    endif()

    string(TIMESTAMP QtADS_AUTO_GEN_DATE "%d.%m.%Y")

    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/ads_globals.h")
        message(
            FATAL_ERROR
            "Unable to place ads_version.h next to ads_globals.h")
    else()
        configure_file(
            "${_VERSIONING_MODULE_DIR}/ads_version.h.in"
            "${CMAKE_CURRENT_SOURCE_DIR}/ads_version.h"
            @ONLY)
    endif()
endfunction()

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

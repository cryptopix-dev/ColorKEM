# FindMinGW.cmake - Custom module to find MinGW installations on Windows
#
# This module defines:
#  MINGW_FOUND        - True if MinGW is found
#  MINGW_ROOT         - Root directory of MinGW installation
#  MINGW_BIN_DIR      - Bin directory containing gcc.exe, g++.exe
#  MINGW_INCLUDE_DIR  - Include directory
#  MINGW_LIB_DIR      - Library directory
#  MINGW_C_COMPILER   - Path to gcc.exe
#  MINGW_CXX_COMPILER - Path to g++.exe
#
# Usage:
#  find_package(MinGW REQUIRED)

if(WIN32)
    # Common MinGW installation paths
    set(_mingw_search_paths
        "C:/msys64/mingw64"
        "C:/ProgramData/mingw64/mingw64"
        "C:/mingw64"
        "C:/mingw-w64"
        "$ENV{MSYSTEM_PREFIX}"  # For MSYS2 environments
    )

    # First, try to find gcc.exe in PATH or common locations
    find_program(MINGW_C_COMPILER
        NAMES gcc.exe
        PATHS ${_mingw_search_paths}
        PATH_SUFFIXES bin
        NO_DEFAULT_PATH
    )

    if(NOT MINGW_C_COMPILER)
        find_program(MINGW_C_COMPILER NAMES gcc.exe)
    endif()

    if(MINGW_C_COMPILER)
        # Derive root from compiler path
        get_filename_component(MINGW_BIN_DIR "${MINGW_C_COMPILER}" DIRECTORY)
        get_filename_component(MINGW_ROOT "${MINGW_BIN_DIR}" DIRECTORY)

        # Set other paths
        set(MINGW_INCLUDE_DIR "${MINGW_ROOT}/include")
        set(MINGW_LIB_DIR "${MINGW_ROOT}/lib")

        # Find g++.exe in the same bin directory
        find_program(MINGW_CXX_COMPILER
            NAMES g++.exe
            PATHS ${MINGW_BIN_DIR}
            NO_DEFAULT_PATH
        )

        # Check if this looks like a valid MinGW installation
        if(EXISTS "${MINGW_INCLUDE_DIR}/stdio.h" AND EXISTS "${MINGW_LIB_DIR}/libmingw32.a")
            set(MINGW_FOUND TRUE)
        endif()
    endif()

    # If not found via compiler, try direct path search
    if(NOT MINGW_FOUND)
        foreach(_path ${_mingw_search_paths})
            if(EXISTS "${_path}/bin/gcc.exe" AND EXISTS "${_path}/include/stdio.h")
                set(MINGW_ROOT "${_path}")
                set(MINGW_BIN_DIR "${_path}/bin")
                set(MINGW_INCLUDE_DIR "${_path}/include")
                set(MINGW_LIB_DIR "${_path}/lib")
                set(MINGW_C_COMPILER "${_path}/bin/gcc.exe")
                set(MINGW_CXX_COMPILER "${_path}/bin/g++.exe")
                set(MINGW_FOUND TRUE)
                break()
            endif()
        endforeach()
    endif()

    # Check for pkg-config in MSYS2
    if(MINGW_FOUND)
        find_program(MINGW_PKG_CONFIG
            NAMES pkg-config.exe
            PATHS "${MINGW_ROOT}/../usr/bin" "C:/msys64/usr/bin"
            NO_DEFAULT_PATH
        )
    endif()

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(MinGW
        REQUIRED_VARS MINGW_ROOT MINGW_BIN_DIR MINGW_C_COMPILER MINGW_CXX_COMPILER
        VERSION_VAR MINGW_VERSION
    )

    mark_as_advanced(
        MINGW_ROOT
        MINGW_BIN_DIR
        MINGW_INCLUDE_DIR
        MINGW_LIB_DIR
        MINGW_C_COMPILER
        MINGW_CXX_COMPILER
        MINGW_PKG_CONFIG
    )

else()
    # Non-Windows systems don't need MinGW
    set(MINGW_FOUND FALSE)
endif()
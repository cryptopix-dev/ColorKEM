# MinGW toolchain file for ColorKEM Windows build

# Find MinGW installation
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(MinGW REQUIRED)

# Specify the compilers
set(CMAKE_C_COMPILER "${MINGW_C_COMPILER}" CACHE FILEPATH "C compiler")
set(CMAKE_CXX_COMPILER "${MINGW_CXX_COMPILER}" CACHE FILEPATH "C++ compiler")

# Set OpenSSL path for MinGW (assume it's in the same MinGW root)
set(OPENSSL_ROOT_DIR "${MINGW_ROOT}" CACHE PATH "OpenSSL root directory")

# Set pkg-config if found
if(MINGW_PKG_CONFIG)
    set(PKG_CONFIG_EXECUTABLE "${MINGW_PKG_CONFIG}" CACHE FILEPATH "pkg-config executable")
endif()

# Set compiler flags
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Require C++ standard")

# Windows-specific definitions
add_definitions(-D_MINGW -DWIN32 -D_WIN32)

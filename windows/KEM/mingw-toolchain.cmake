# MinGW toolchain file for ColorKEM Windows build

# Compiler paths should be passed via command line or detected from PATH
# set(CMAKE_C_COMPILER ... ) - Removed to allow auto-detection
# set(CMAKE_CXX_COMPILER ... ) - Removed to allow auto-detection

# OpenSSL path should be passed via command line or environment
# set(OPENSSL_ROOT_DIR ... ) - Removed to allow auto-detection

# Set compiler flags
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Require C++ standard")

# Windows-specific definitions
add_definitions(-D_MINGW -DWIN32 -D_WIN32)
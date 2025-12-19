# MinGW toolchain file for ColorKEM Windows build

# Compiler paths should be passed via command line or detected from PATH
# set(CMAKE_C_COMPILER ... ) - Removed to allow auto-detection
# set(CMAKE_CXX_COMPILER ... ) - Removed to allow auto-detection

# OpenSSL path should be passed via command line or environment
# set(OPENSSL_ROOT_DIR ... ) - Removed to allow auto-detection

# Set compiler flags
set(CMAKE_CXX_STANDARD 17 CACHE STRING "C++ standard")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "Require C++ standard")
# Enable GNU extensions to prevent __STRICT_ANSI__ from being defined
# This fixes the at_quick_exit and quick_exit errors with GCC 15.2.0
set(CMAKE_CXX_EXTENSIONS ON CACHE BOOL "Enable GNU extensions")

# Windows-specific definitions
add_definitions(-D_MINGW -DWIN32 -D_WIN32)

# Ensure C99 standard library functions are available
# This is needed for GCC 15.2.0 compatibility across different MinGW installations
add_compile_definitions(_GLIBCXX_USE_C99_STDLIB=1)
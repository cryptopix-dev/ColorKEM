# MinGW toolchain file for ColorKEM Windows build

# Compiler paths should be passed via command line or detected from PATH
# set(CMAKE_C_COMPILER ... ) - Removed to allow auto-detection
# set(CMAKE_CXX_COMPILER ... ) - Removed to allow auto-detection

# OpenSSL path should be passed via command line or environment
# set(OPENSSL_ROOT_DIR ... ) - Removed to allow auto-detection

# Compiler flags are set in main CMakeLists.txt

# Fix for GCC 15.2.0 __STRICT_ANSI__ issue
add_compile_options(-include ${CMAKE_CURRENT_LIST_DIR}/gcc_15_fix.h -fext-numeric-literals)

# Windows-specific definitions
add_definitions(-D_MINGW -DWIN32 -D_WIN32)

# Ensure C99 standard library functions are available
# This is needed for GCC 15.2.0 compatibility across different MinGW installations
add_compile_definitions(_GLIBCXX_USE_C99_STDLIB=1)
add_compile_options(-U__STRICT_ANSI__)
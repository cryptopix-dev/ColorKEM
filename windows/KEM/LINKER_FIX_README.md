# OpenSSL Linker Error Fix

## Problem
When building ColorKEM on Windows with MinGW and OpenSSL static libraries, you may encounter the following linker error:

```
undefined reference to `__imp__vsnprintf'
```

This error occurs when linking executables against OpenSSL's static library (`libcrypto.a`).

## Root Cause
The issue stems from a mismatch between:
1. **OpenSSL's static library** - compiled with certain expectations about the C runtime
2. **MinGW's runtime** - newer versions (especially GCC 15.2.0) that have different symbol naming conventions
3. **Cross-compiler scenarios** - When GCC is installed separately from MSYS2/MinGW (e.g., GCC in `C:\gcc-15.2.0` but OpenSSL in `C:\msys64\mingw64`)

The `__imp__vsnprintf` symbol is part of the standard C library that OpenSSL's static library expects. This function is provided by the `mingwex` library, but when GCC and OpenSSL come from different MinGW installations, the linker may not find it automatically.

## Solution
The solution involves:
1. **Finding the mingwex library** explicitly from the OpenSSL installation directory
2. **Creating a reusable CMake function** to apply consistent linking across all executables
3. **Adding linker flags** to handle potential symbol conflicts

### Implementation Details

#### 1. CMake Function for Windows Linking
A reusable function `add_windows_openssl_libs()` was created in the main CMakeLists.txt:

```cmake
# Find mingwex library once for all executables
find_library(MINGWEX_LIBRARY 
    NAMES mingwex libmingwex
    PATHS 
        ${OPENSSL_ROOT_DIR}/lib
        ${OPENSSL_ROOT_DIR}/../lib
        C:/msys64/mingw64/lib
        C:/ProgramData/mingw64/mingw64/lib
    NO_DEFAULT_PATH
)

# Function to add Windows-specific libraries to executables
function(add_windows_openssl_libs target_name)
    # Add linker flags to handle potential symbol mismatches
    target_link_options(${target_name} PRIVATE -Wl,--allow-multiple-definition)
    
    # Link OpenSSL libraries
    target_link_libraries(${target_name} PRIVATE OpenSSL::Crypto OpenSSL::SSL)
    
    # Add mingwex library (from OpenSSL's MinGW installation)
    if(MINGWEX_LIBRARY)
        target_link_libraries(${target_name} PRIVATE ${MINGWEX_LIBRARY})
    else()
        target_link_libraries(${target_name} PRIVATE mingwex)
    endif()
    
    # Add Windows system libraries
    target_link_libraries(${target_name} PRIVATE crypt32 bcrypt ws2_32 gdi32)
    
    # Add zlib if found
    if(ZLIB_FOUND)
        target_link_libraries(${target_name} PRIVATE ${ZLIB_LIBRARIES})
    endif()
endfunction()
```

#### 2. Usage in Executables
All executables now use this function:

```cmake
add_executable(benchmark_color_kem_timing benchmark_color_kem_timing.cpp)
target_link_libraries(benchmark_color_kem_timing PRIVATE clwe)

if(WIN32)
    add_windows_openssl_libs(benchmark_color_kem_timing)
endif()
```

#### 3. Test Executables
The tests/CMakeLists.txt applies this to all test executables:

```cmake
if(WIN32)
    set(TEST_EXECUTABLES test_color_value test_clwe_parameters ...)
    
    foreach(test_exe ${TEST_EXECUTABLES})
        add_windows_openssl_libs(${test_exe})
    endforeach()
endif()
```

## Why This Works
1. **Explicit mingwex search** - Searches for mingwex in OpenSSL's lib directory first, ensuring we use the compatible version
2. **Cross-compiler support** - Handles cases where GCC and OpenSSL are from different MinGW installations
3. **Linker flags** - `--allow-multiple-definition` handles cases where symbols might be defined in multiple libraries
4. **Consistent application** - The function ensures all executables get the same linking configuration
5. **Fallback mechanism** - If mingwex isn't found in specific paths, it falls back to linking by name

## Key Libraries
- **mingwex** - Provides `vsnprintf` and other standard C library functions
- **OpenSSL::Crypto & OpenSSL::SSL** - OpenSSL cryptographic libraries
- **crypt32, bcrypt, ws2_32, gdi32** - Windows system libraries for crypto and networking

## Linking Order
The order matters! Libraries should be linked in this sequence:
1. Main library (clwe)
2. legacy_stdio_definitions
3. OpenSSL::Crypto
4. Windows system libraries (crypt32, bcrypt, ws2_32)
5. Optional dependencies (zlib, webp, etc.)

## Verification
After applying this fix, the build should complete successfully without linker errors. Run:

```batch
.\build_windows.bat
```

All executables should build and link correctly.

## Alternative Solutions (Not Recommended)
1. **Use dynamic linking** - Link against OpenSSL DLLs instead of static libraries
   - Downside: Requires distributing OpenSSL DLLs with your application
   
2. **Rebuild OpenSSL** - Compile OpenSSL from source with matching compiler flags
   - Downside: Time-consuming and complex

3. **Downgrade GCC** - Use an older GCC version that doesn't have this issue
   - Downside: Loses benefits of newer compiler versions

## References
- MinGW legacy_stdio_definitions: https://sourceforge.net/p/mingw-w64/wiki2/gnu%20printf/
- OpenSSL static linking on Windows: https://wiki.openssl.org/index.php/Compilation_and_Installation

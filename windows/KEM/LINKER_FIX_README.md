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

The `__imp__vsnprintf` symbol is part of the legacy stdio implementation that OpenSSL's static library expects, but newer MinGW runtimes don't provide by default.

## Solution
Add the `mingwex` library to all executables that link against OpenSSL (directly or indirectly through the `clwe` library).

The `mingwex` library is part of the MinGW runtime and provides implementations of standard C functions like `vsnprintf` that OpenSSL's static library expects.

### Changes Made

#### 1. Main CMakeLists.txt
Added Windows-specific libraries to:
- `benchmark_color_kem_timing` executable
- `generate_key_images` executable (if WEBP is found)
- `test_key_images` executable (if WEBP is found)

```cmake
if(WIN32)
    target_link_libraries(executable_name PRIVATE 
        OpenSSL::Crypto 
        OpenSSL::SSL
        mingwex
        crypt32 
        bcrypt 
        ws2_32
        gdi32
    )
    if(ZLIB_FOUND)
        target_link_libraries(executable_name PRIVATE ${ZLIB_LIBRARIES})
    endif()
endif()
```

#### 2. tests/CMakeLists.txt
Added Windows-specific libraries to all test executables using a loop:
- All test executables (test_color_value, test_clwe_parameters, etc.)
- All fuzzing executables (if fuzzing is enabled)

## Why This Works
1. **mingwex** - Provides the missing `vsnprintf` and other standard C library functions that OpenSSL expects
2. **OpenSSL::Crypto & OpenSSL::SSL** - Explicitly links OpenSSL crypto and SSL libraries
3. **crypt32, bcrypt, ws2_32, gdi32** - Windows system libraries required by OpenSSL for cryptographic operations and networking
4. **ZLIB_LIBRARIES** - Compression library that OpenSSL may depend on

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

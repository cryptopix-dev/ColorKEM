# ColorKEM Windows Build - Cross-Compiler Fix Summary

## Issue Reported
Build fails on some devices with the error:
```
undefined reference to `__imp__vsnprintf'
```

Even after the initial mingwex fix was applied.

## Root Cause Analysis

The issue occurs in **cross-compiler scenarios** where:
- **GCC** is installed in one location (e.g., `C:\gcc-15.2.0`)
- **OpenSSL** is installed in another location (e.g., `C:\msys64\mingw64`)
- The **mingwex library** (which provides `vsnprintf`) is in OpenSSL's lib directory
- The **linker** doesn't automatically find mingwex because it's looking in GCC's lib directory

### Example Scenario
```
GCC Location:     C:\gcc-15.2.0\bin\gcc.exe
OpenSSL Location: C:\msys64\mingw64
mingwex Location: C:\msys64\mingw64\lib\libmingwex.a

Problem: GCC's linker looks in C:\gcc-15.2.0\lib but mingwex is in C:\msys64\mingw64\lib
```

## Solution Implemented

### 1. Explicit Library Search
Added code to explicitly search for mingwex in multiple locations:
- OpenSSL's lib directory (where it's most likely to be compatible)
- Common MinGW installation paths
- System-wide search as fallback

```cmake
find_library(MINGWEX_LIBRARY 
    NAMES mingwex libmingwex
    PATHS 
        ${OPENSSL_ROOT_DIR}/lib          # Search OpenSSL's lib first
        ${OPENSSL_ROOT_DIR}/../lib
        C:/msys64/mingw64/lib
        C:/ProgramData/mingw64/mingw64/lib
    NO_DEFAULT_PATH
)
```

### 2. Reusable CMake Function
Created `add_windows_openssl_libs()` function that:
- Adds linker flags to handle symbol conflicts
- Links OpenSSL libraries
- Links the found mingwex library (or falls back to linking by name)
- Links Windows system libraries
- Links zlib if available

### 3. Consistent Application
Applied the function to all executables:
- Main benchmark executable
- WEBP-related executables
- All test executables (10 tests)
- All fuzzing executables (4 fuzzers)

## Files Modified

### Main CMakeLists.txt
- Added mingwex library search (lines 140-160)
- Added `add_windows_openssl_libs()` function (lines 162-184)
- Updated benchmark executable linking (line 197)
- Updated WEBP executables linking (lines 208, 219)

### tests/CMakeLists.txt
- Updated all test executables to use the function (line 55)
- Updated all fuzzing executables to use the function (line 83)

### Documentation
- `LINKER_FIX_README.md` - Updated with cross-compiler scenario explanation
- `TROUBLESHOOTING_MINGWEX.md` - New comprehensive troubleshooting guide
- `test_build.ps1` - New test script for verifying the fix

## How It Works

1. **CMake Configuration Phase**:
   - Searches for mingwex library in OpenSSL's directory
   - Stores the full path in `MINGWEX_LIBRARY` variable
   - Reports whether it was found

2. **Linking Phase**:
   - For each executable, calls `add_windows_openssl_libs()`
   - Function adds the mingwex library using the full path
   - If not found, falls back to `-lmingwex` (linker searches default paths)

3. **Linker Flags**:
   - `--allow-multiple-definition` handles cases where symbols exist in multiple libraries
   - This is common when mixing libraries from different MinGW installations

## Benefits

1. **Cross-compiler support** - Works when GCC and OpenSSL are from different sources
2. **Automatic detection** - No manual configuration needed in most cases
3. **Clear error messages** - CMake reports if mingwex is found or not
4. **Maintainable** - Single function used across all executables
5. **Fallback mechanism** - Still attempts to link even if library isn't found in expected locations

## Testing

To test the fix on your system:

```powershell
cd windows\KEM
.\test_build.ps1
```

This will:
1. Clean the build directory
2. Run the build script
3. Report success or failure with helpful messages

## Troubleshooting

If the build still fails:

1. **Check CMake output** for mingwex library detection:
   ```
   -- Found mingwex library: C:/msys64/mingw64/lib/libmingwex.a
   ```

2. **Review the troubleshooting guide**:
   ```
   See TROUBLESHOOTING_MINGWEX.md for detailed solutions
   ```

3. **Common solutions**:
   - Install both GCC and OpenSSL from MSYS2
   - Copy mingwex library to OpenSSL's lib directory
   - Use dynamic linking instead of static linking

## Prevention

For future projects and other users:

1. **Recommend MSYS2** - Use a single MinGW distribution for all tools
2. **Document requirements** - Specify that GCC and OpenSSL should be from the same source
3. **Provide clear error messages** - CMake now warns if mingwex isn't found
4. **Include troubleshooting guide** - Help users resolve issues independently

## Technical Details

### Why mingwex is Needed
OpenSSL's static library (`libcrypto.a`) contains calls to standard C library functions like:
- `vsnprintf` - Variable argument string formatting
- `snprintf` - String formatting with size limit
- Other stdio functions

These functions are implemented in `libmingwex.a`, which is part of the MinGW runtime.

### Why Cross-Compiler Issues Occur
When you compile with GCC from one MinGW installation but link against libraries from another:
1. The compiler uses its own include files
2. The linker searches its own lib directories first
3. Libraries from the other installation may not be found
4. Symbol mismatches can occur due to different runtime versions

### The Fix
By explicitly finding and linking mingwex from OpenSSL's installation:
1. We ensure compatibility between OpenSSL and the C runtime
2. We avoid symbol mismatches
3. We make the linking process explicit and debuggable

## Conclusion

This fix handles the most common cross-compiler scenario on Windows where users have:
- Standalone GCC installation (e.g., from winlibs, MinGW-w64 builds)
- MSYS2-based OpenSSL installation
- Need to link them together

The solution is robust, maintainable, and provides clear feedback during the build process.

# Troubleshooting Guide: mingwex Library Not Found

## Problem
Even after applying the fix, you may still see:
```
undefined reference to `__imp__vsnprintf'
```

This typically happens when:
1. GCC is installed in a different location than OpenSSL
2. The mingwex library doesn't exist in the expected locations
3. There's a version mismatch between GCC and the mingwex library

## Diagnostic Steps

### Step 1: Check Your Compiler and OpenSSL Locations

Run these commands in PowerShell:

```powershell
# Find GCC location
where.exe gcc

# Find OpenSSL location (if in PATH)
where.exe openssl

# Check if mingwex exists in OpenSSL's lib directory
# Replace C:\msys64\mingw64 with your actual OpenSSL path
Test-Path "C:\msys64\mingw64\lib\libmingwex.a"
```

### Step 2: Locate mingwex Library

Search for the mingwex library on your system:

```powershell
# Search in common locations
Get-ChildItem -Path "C:\" -Filter "libmingwex.a" -Recurse -ErrorAction SilentlyContinue | Select-Object FullName
```

### Step 3: Verify Library Compatibility

If you found mingwex, check if it's compatible with your GCC version:

```powershell
# Check GCC version
gcc --version

# Check the architecture of mingwex (should be x86_64 for 64-bit)
# This requires objdump from MinGW
objdump -f "C:\path\to\libmingwex.a" | Select-String "architecture"
```

## Solutions

### Solution 1: Use Matching MinGW Installation

**Recommended**: Install both GCC and OpenSSL from the same MinGW distribution (MSYS2).

1. Install MSYS2 from https://www.msys2.org/
2. Open MSYS2 MinGW 64-bit terminal
3. Install required packages:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-openssl
   pacman -S mingw-w64-x86_64-cmake
   ```
4. Add `C:\msys64\mingw64\bin` to your system PATH
5. Rebuild the project

### Solution 2: Manually Specify mingwex Library Path

If you can't reinstall, manually specify the mingwex library location:

1. Find your mingwex library (from Step 2 above)
2. Create a file `custom_paths.bat` in the `windows/KEM` directory:
   ```batch
   @echo off
   REM Custom paths for ColorKEM build
   
   REM Set the path to mingwex library
   set MINGWEX_LIB_PATH=C:\path\to\your\libmingwex.a
   ```
3. Modify `CMakeLists.txt` to use this path (add after line 147):
   ```cmake
   # Check for custom mingwex path from environment
   if(DEFINED ENV{MINGWEX_LIB_PATH})
       set(MINGWEX_LIBRARY $ENV{MINGWEX_LIB_PATH})
       message(STATUS "Using custom mingwex library: ${MINGWEX_LIBRARY}")
   endif()
   ```

### Solution 3: Copy mingwex to OpenSSL lib Directory

If you have mingwex in one location and OpenSSL in another:

```powershell
# Copy mingwex library to OpenSSL's lib directory
Copy-Item "C:\path\to\libmingwex.a" "C:\msys64\mingw64\lib\"
```

### Solution 4: Use Dynamic Linking Instead

If static linking continues to fail, switch to dynamic linking:

1. Modify `CMakeLists.txt` line 28:
   ```cmake
   # Change from:
   set(OPENSSL_USE_STATIC_LIBS TRUE)
   
   # To:
   set(OPENSSL_USE_STATIC_LIBS FALSE)
   ```

2. Ensure OpenSSL DLLs are in your PATH or copy them to the build directory:
   ```powershell
   Copy-Item "C:\msys64\mingw64\bin\libcrypto-*.dll" "build\"
   Copy-Item "C:\msys64\mingw64\bin\libssl-*.dll" "build\"
   ```

## Verification

After applying any solution, verify the build works:

```powershell
# Clean and rebuild
.\test_build.ps1
```

## Common Error Messages and Fixes

### Error: "cannot find -lmingwex"
**Cause**: mingwex library not found in linker search paths  
**Fix**: Use Solution 2 or 3 above

### Error: "incompatible target"
**Cause**: mingwex library is for different architecture (32-bit vs 64-bit)  
**Fix**: Ensure you're using x86_64 (64-bit) versions of all libraries

### Error: "multiple definition of ___chkstk_ms"
**Cause**: Symbol conflicts between different MinGW runtimes  
**Fix**: Already handled by `-Wl,--allow-multiple-definition` flag in our fix

## Getting Help

If none of these solutions work:

1. **Collect diagnostic information**:
   ```powershell
   gcc --version > build_info.txt
   where.exe gcc >> build_info.txt
   where.exe openssl >> build_info.txt
   cmake --version >> build_info.txt
   ```

2. **Check CMake output** in the build directory:
   ```powershell
   Get-Content "build\CMakeCache.txt" | Select-String "MINGWEX"
   Get-Content "build\CMakeCache.txt" | Select-String "OPENSSL"
   ```

3. **Share the information** with the error message for further assistance

## Prevention

To avoid this issue in future projects:

1. **Use MSYS2** for all MinGW-based development on Windows
2. **Keep toolchain consistent** - install all dependencies from the same package manager
3. **Document your setup** - note which MinGW distribution you're using
4. **Use package managers** - MSYS2's pacman ensures compatibility between packages

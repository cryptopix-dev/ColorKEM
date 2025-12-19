# Quick Fix Reference - Windows Build Issues

## Error: `undefined reference to '__imp__vsnprintf'`

### Quick Solution
The build script now automatically handles this. Just run:
```batch
.\build_windows.bat
```

### What to Check
Look for this message in the CMake output:
```
-- Found mingwex library: C:/msys64/mingw64/lib/libmingwex.a
```

✅ **If you see this**: Build should succeed  
⚠️ **If you see "WARNING: mingwex library not found"**: See troubleshooting below

---

## Troubleshooting

### Option 1: Install MSYS2 (Recommended)
1. Download from https://www.msys2.org/
2. Install and open "MSYS2 MinGW 64-bit" terminal
3. Run:
   ```bash
   pacman -S mingw-w64-x86_64-gcc
   pacman -S mingw-w64-x86_64-openssl
   pacman -S mingw-w64-x86_64-cmake
   ```
4. Add `C:\msys64\mingw64\bin` to system PATH
5. Rebuild

### Option 2: Copy mingwex Library
If you have mingwex somewhere else:
```powershell
# Find it
Get-ChildItem -Path "C:\" -Filter "libmingwex.a" -Recurse -ErrorAction SilentlyContinue

# Copy to OpenSSL lib directory
Copy-Item "C:\path\to\libmingwex.a" "C:\msys64\mingw64\lib\"
```

### Option 3: Use Dynamic Linking
Edit `CMakeLists.txt` line 28:
```cmake
set(OPENSSL_USE_STATIC_LIBS FALSE)  # Change TRUE to FALSE
```

Then copy OpenSSL DLLs to build directory after building.

---

## Documentation

📖 **Detailed Explanation**: `LINKER_FIX_README.md`  
🔧 **Troubleshooting Guide**: `TROUBLESHOOTING_MINGWEX.md`  
📋 **Technical Summary**: `CROSS_COMPILER_FIX_SUMMARY.md`

---

## Test Your Build

```powershell
.\test_build.ps1
```

This will clean, build, and report success/failure.

---

## Still Having Issues?

1. Check your compiler and OpenSSL locations:
   ```powershell
   where.exe gcc
   where.exe openssl
   ```

2. Verify they're from the same MinGW distribution

3. See `TROUBLESHOOTING_MINGWEX.md` for detailed solutions

---

## Prevention

✅ Use MSYS2 for all MinGW development  
✅ Install all dependencies from the same package manager  
✅ Keep toolchain consistent (don't mix different MinGW distributions)

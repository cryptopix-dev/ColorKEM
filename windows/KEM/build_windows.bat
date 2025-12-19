@echo off
REM Build script for ColorKEM Windows using MinGW - Dynamic Configuration with Auto-Detection
setlocal enabledelayedexpansion
cd /d "%~dp0"

echo ========================================
echo ColorKEM Windows Build Script
echo ========================================
echo.

REM ----------------------------------------------------------------------------
REM 0. Load Custom Overrides (if available)
REM ----------------------------------------------------------------------------
if exist "custom_paths.bat" (
    echo [INFO] Loading custom_paths.bat...
    call custom_paths.bat
)

REM ----------------------------------------------------------------------------
REM 1. Find CMake
REM ----------------------------------------------------------------------------
echo Detecting CMake...
if defined CMAKE_EXECUTABLE (
    echo [INFO] CMake explicitly set to: !CMAKE_EXECUTABLE!
    set "CMAKE_CMD=!CMAKE_EXECUTABLE!"
) else (
    where cmake >nul 2>nul
    if !errorlevel! equ 0 (
        set "CMAKE_CMD=cmake"
        echo [INFO] CMake found in PATH.
    ) else (
        if exist "C:\Program Files\CMake\bin\cmake.exe" (
            set "CMAKE_CMD=C:\Program Files\CMake\bin\cmake.exe"
            echo [INFO] CMake found at default location.
        ) else (
            echo [ERROR] CMake not found. Please install CMake and add to PATH.
            exit /b 1
        )
    )
)

REM ----------------------------------------------------------------------------
REM 2. Find GCC and G++ (MinGW)
REM ----------------------------------------------------------------------------
echo Detecting GCC/G++...

REM GCC - Prefer MSYS2 GCC for better compatibility
if defined GCC_FULL_PATH goto :verify_gcc
if exist "C:\msys64\mingw64\bin\gcc.exe" (
    set "GCC_FULL_PATH=C:\msys64\mingw64\bin\gcc.exe"
    echo [INFO] Using MSYS2 GCC for better compatibility
    goto :verify_gcc
)
where gcc >nul 2>nul
if !errorlevel! equ 0 (
    for /f "tokens=*" %%i in ('where gcc') do (
        set "GCC_FULL_PATH=%%i"
        goto :verify_gcc
    )
)

REM Fallback: Look in common MinGW locations
set "COMMON_MINGW_PATHS=C:\ProgramData\mingw64\mingw64\bin\gcc.exe;C:\MinGW\bin\gcc.exe;C:\msys64\mingw64\bin\gcc.exe;C:\Program Files\mingw-w64\x86_64-8.1.0-posix-seh-rt_v6-rev0\mingw64\bin\gcc.exe"
for %%p in ("!COMMON_MINGW_PATHS:;=" "!") do (
    if exist %%p (
        set "GCC_FULL_PATH=%%~p"
        echo [INFO] GCC found in common path: %%~p
        goto :verify_gcc
    )
)

echo [ERROR] GCC not found in PATH or common locations.
exit /b 1

:verify_gcc
echo [INFO] C Compiler: !GCC_FULL_PATH!

REM G++
if defined GXX_FULL_PATH goto :verify_gxx
REM Prefer MSYS2 G++ if GCC is MSYS2
if "!GCC_FULL_PATH!"=="C:\msys64\mingw64\bin\gcc.exe" (
    if exist "C:\msys64\mingw64\bin\g++.exe" (
        set "GXX_FULL_PATH=C:\msys64\mingw64\bin\g++.exe"
        echo [INFO] Using MSYS2 G++ for consistency
        goto :verify_gxx
    )
)
where g++ >nul 2>nul
if !errorlevel! equ 0 (
    for /f "tokens=*" %%i in ('where g++') do (
        set "GXX_FULL_PATH=%%i"
        goto :verify_gxx
    )
)

REM Try to derive G++ from GCC path
for %%I in ("!GCC_FULL_PATH!") do set "GCC_DIR=%%~dpI"
if exist "!GCC_DIR!g++.exe" (
    set "GXX_FULL_PATH=!GCC_DIR!g++.exe"
    echo [INFO] G++ found alongside GCC: !GXX_FULL_PATH!
    goto :verify_gxx
)

echo [ERROR] G++ not found.
exit /b 1

:verify_gxx
echo [INFO] C++ Compiler: !GXX_FULL_PATH!


REM ----------------------------------------------------------------------------
REM 3. Find OpenSSL
REM ----------------------------------------------------------------------------
REM ----------------------------------------------------------------------------
REM 3. Find OpenSSL
REM ----------------------------------------------------------------------------
REM ----------------------------------------------------------------------------
REM 3. Find OpenSSL
REM ----------------------------------------------------------------------------
echo Detecting OpenSSL...

set "OPENSSL_ARG="
set "FOUND_MINGW_OPENSSL="

REM 3.1 Always check MinGW Root (Relative to detected GCC) FIRST
REM Use this if found, as it matches the compiler.
for %%I in ("!GCC_FULL_PATH!") do set "GCC_BIN=%%~dpI"
pushd "!GCC_BIN!.."
set "TRGT_MINGW_ROOT=!CD!"
popd

if exist "!TRGT_MINGW_ROOT!\include\openssl\ssl.h" (
    set "OPENSSL_ROOT_DIR=!TRGT_MINGW_ROOT!"
    echo [INFO] Found OpenSSL in MinGW Root: !OPENSSL_ROOT_DIR!
    set "FOUND_MINGW_OPENSSL=1"
)

REM 3.2 If not found in MinGW, check environment variable or custom overrides
if not defined FOUND_MINGW_OPENSSL (
    if defined OPENSSL_ROOT_DIR (
        echo [INFO] Using explicitly set OpenSSL Root: !OPENSSL_ROOT_DIR!
    ) else (
        REM 3.3 Check Common Paths / Where
        call :find_fallback_openssl
    )
)

if not defined OPENSSL_ROOT_DIR (
    echo [WARNING] OpenSSL headers not found.
    goto :openssl_done
)

REM ----------------------------------------------------------------------------
REM Detect OpenSSL Libraries
REM ----------------------------------------------------------------------------
echo Detecting OpenSSL Libraries...

REM Normalize ROOT to Backslashes for file checks
set "ROOT_BS=!OPENSSL_ROOT_DIR:/=\!"
set "LIB_DIR_BS=!ROOT_BS!\lib"

set "OPENSSL_CRYPTO_LIBRARY="
set "OPENSSL_SSL_LIBRARY="

REM 1. Check for MinGW style .dll.a or .a (PREFERRED)
if exist "!LIB_DIR_BS!\libcrypto.dll.a" set "OPENSSL_CRYPTO_LIBRARY=!LIB_DIR_BS!\libcrypto.dll.a"
if not defined OPENSSL_CRYPTO_LIBRARY if exist "!LIB_DIR_BS!\libcrypto.a" set "OPENSSL_CRYPTO_LIBRARY=!LIB_DIR_BS!\libcrypto.a"

if exist "!LIB_DIR_BS!\libssl.dll.a" set "OPENSSL_SSL_LIBRARY=!LIB_DIR_BS!\libssl.dll.a"
if not defined OPENSSL_SSL_LIBRARY if exist "!LIB_DIR_BS!\libssl.a" set "OPENSSL_SSL_LIBRARY=!LIB_DIR_BS!\libssl.a"

REM 2. Check for VC++ style .lib (Backup)
if not defined OPENSSL_CRYPTO_LIBRARY if exist "!LIB_DIR_BS!\libcrypto.lib" set "OPENSSL_CRYPTO_LIBRARY=!LIB_DIR_BS!\libcrypto.lib"
if not defined OPENSSL_SSL_LIBRARY if exist "!LIB_DIR_BS!\libssl.lib" set "OPENSSL_SSL_LIBRARY=!LIB_DIR_BS!\libssl.lib"

REM Prepare CMake Arguments (Use Forward Slashes)
set "OPENSSL_ROOT_FS=!ROOT_BS:\=/!"
set "OPENSSL_INCLUDE_FS=!OPENSSL_ROOT_FS!/include"

set "OPENSSL_ARG=-DOPENSSL_ROOT_DIR="!OPENSSL_ROOT_FS!" -DOPENSSL_INCLUDE_DIR="!OPENSSL_INCLUDE_FS!""

if defined OPENSSL_CRYPTO_LIBRARY (
    set "LIB_CRYPTO_FS=!OPENSSL_CRYPTO_LIBRARY:\=/!"
    echo [INFO] Found Crypto Lib: !LIB_CRYPTO_FS!
    set "OPENSSL_ARG=!OPENSSL_ARG! -DOPENSSL_CRYPTO_LIBRARY="!LIB_CRYPTO_FS!""
)
if defined OPENSSL_SSL_LIBRARY (
    set "LIB_SSL_FS=!OPENSSL_SSL_LIBRARY:\=/!"
    echo [INFO] Found SSL Lib: !LIB_SSL_FS!
    set "OPENSSL_ARG=!OPENSSL_ARG! -DOPENSSL_SSL_LIBRARY="!LIB_SSL_FS!""
)

goto :openssl_done

:find_fallback_openssl
    REM Check Common Windows Locations
    set "COMMON_OPENSSL=C:\msys64\mingw64;C:\Program Files\OpenSSL-Win64;C:\Program Files\OpenSSL;C:\OpenSSL-Win64"
    for %%p in ("!COMMON_OPENSSL:;=" "!") do (
        if exist "%%~p\include\openssl\ssl.h" (
            set "OPENSSL_ROOT_DIR=%%~p"
            echo [INFO] OpenSSL found in common path: %%~p
            exit /b
        )
    )
    
    REM Last Resort: 'where openssl'
    where openssl >nul 2>nul
    if !errorlevel! equ 0 (
        for /f "tokens=*" %%i in ('where openssl') do (
            set "OPENSSL_BIN_PATH=%%i"
            REM Derive root
             for %%I in ("!OPENSSL_BIN_PATH!") do set "OPENSSL_BIN_DIR=%%~dpI"
            pushd "!OPENSSL_BIN_DIR!.."
            set "OPENSSL_ROOT_DIR=!CD!"
            popd
            echo [INFO] OpenSSL detected (via PATH): !OPENSSL_ROOT_DIR!
            exit /b
        )
    )
    exit /b

:openssl_done

REM ----------------------------------------------------------------------------
REM 4. Build Process
REM ----------------------------------------------------------------------------

if exist "build" (
    echo Cleaning build directory...
    rmdir /s /q build
)
mkdir build
cd build

REM Normalize Compile Paths too
set "GCC_FULL_PATH=!GCC_FULL_PATH:\=/!"
set "GXX_FULL_PATH=!GXX_FULL_PATH:\=/!"

echo Configuring project with CMake...
echo CMake Command: "!CMAKE_CMD!" .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER="!GCC_FULL_PATH!" -DCMAKE_CXX_COMPILER="!GXX_FULL_PATH!" !OPENSSL_ARG!

"!CMAKE_CMD!" .. -G "MinGW Makefiles" ^
    -DCMAKE_C_COMPILER="!GCC_FULL_PATH!" ^
    -DCMAKE_CXX_COMPILER="!GXX_FULL_PATH!" ^
    -DCMAKE_TOOLCHAIN_FILE="..\mingw-toolchain.cmake" ^
    !OPENSSL_ARG!



if %errorlevel% neq 0 (
    echo CMake configuration failed.
    exit /b 1
)

echo Building project...
echo Building project (Sequential)...
"%CMAKE_CMD%" --build . --parallel 1
if %errorlevel% neq 0 (
    echo Build failed.
    exit /b 1
)

cd ..
echo.
echo Build completed successfully!
echo Executables include: benchmark_color_kem_timing.exe
echo.

if exist "build\benchmark_color_kem_timing.exe" (
    echo Running quick verification...
    build\benchmark_color_kem_timing.exe
)

endlocal
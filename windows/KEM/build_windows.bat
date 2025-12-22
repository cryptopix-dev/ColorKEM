@echo off
REM Build script for ColorKEM Windows using MinGW
setlocal enabledelayedexpansion

echo ========================================
echo ColorKEM Windows Build Script
echo ========================================
echo.

REM Check for required tools
echo Checking for required build tools...
set "requiredTools=cmake gcc openssl"
for %%t in (%requiredTools%) do (
    where %%t >nul 2>nul
    if !errorlevel! equ 0 (
        echo [OK] %%t found
    ) else (
        echo [ERROR] Required tool '%%t' is not installed or not in PATH.
        echo Please install it and ensure it's in your PATH.
        exit /b 1
    )
)

echo All required tools are available.
echo.

REM Detect MinGW installation
echo Detecting MinGW installation...
set "MINGW_FOUND=0"

REM Common MinGW paths to check
set "mingw_paths=C:\msys64\mingw64 C:\ProgramData\mingw64\mingw64 C:\mingw64"

for %%p in (%mingw_paths%) do (
    if exist "%%p\bin\gcc.exe" (
        echo Found MinGW at %%p
        set "MINGW_ROOT=%%p"
        set "MINGW_FOUND=1"
        goto :found_mingw
    )
)

REM Check if gcc is already in PATH
where gcc >nul 2>nul
if %errorlevel% equ 0 (
    for /f "tokens=*" %%i in ('where gcc') do set "GCC_PATH=%%i"
    for %%i in ("%GCC_PATH%") do set "MINGW_BIN_DIR=%%~dpi"
    for %%i in ("%MINGW_BIN_DIR%.") do set "MINGW_ROOT=%%~dpi"
    set "MINGW_ROOT=%MINGW_ROOT:~0,-1%"
    if exist "%MINGW_ROOT%\include\stdio.h" (
        echo Found MinGW via PATH at %MINGW_ROOT%
        set "MINGW_FOUND=1"
        goto :found_mingw
    )
)

if %MINGW_FOUND% equ 0 (
    echo ERROR: MinGW installation not found. Please install MinGW or MSYS2.
    echo Checked paths: %mingw_paths%
    exit /b 1
)

:found_mingw
REM Set compiler environment variables for CMake
set "CC=%MINGW_ROOT%\bin\gcc.exe"
set "CXX=%MINGW_ROOT%\bin\g++.exe"
set "PATH=%MINGW_ROOT%\bin;%PATH%"

REM Try to find pkg-config
if exist "C:\msys64\usr\bin\pkg-config.exe" (
    set "PKG_CONFIG_EXECUTABLE=C:\msys64\usr\bin\pkg-config.exe"
    set "PKG_CONFIG_PATH=%MINGW_ROOT%\lib\pkgconfig;%PKG_CONFIG_PATH%"
) else if exist "%MINGW_ROOT%\..\usr\bin\pkg-config.exe" (
    set "PKG_CONFIG_EXECUTABLE=%MINGW_ROOT%\..\usr\bin\pkg-config.exe"
    set "PKG_CONFIG_PATH=%MINGW_ROOT%\lib\pkgconfig;%PKG_CONFIG_PATH%"
)

REM Set OpenSSL root (assume it's in MinGW)
set "OPENSSL_ROOT_DIR=%MINGW_ROOT%"

REM Export environment variables for subprocesses (optional, commented out)
REM setx CC "%MINGW_ROOT%\bin\gcc.exe"
REM setx CXX "%MINGW_ROOT%\bin\g++.exe"

REM Clean build directory to avoid stale CMake cache issues
if exist "build" (
    echo Cleaning build directory...
    rmdir /s /q build
)

REM Create build directory
mkdir build

REM Change to build directory
cd build

REM Run CMake
echo Configuring project with CMake...
cmake .. -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE="mingw-toolchain.cmake" -DPKG_CONFIG_EXECUTABLE="C:/msys64/usr/bin/pkg-config.exe"
if %errorlevel% neq 0 (
    echo CMake configuration failed.
    exit /b 1
)

REM Build the project
echo Building project...
cmake --build .
if %errorlevel% neq 0 (
    echo Build failed.
    exit /b 1
)

REM Return to original directory
cd ..

echo.
echo Build completed successfully!
echo Executables are available in the build/ directory:
echo   - benchmark_color_kem_timing: Benchmark executable
echo   - key_sizes_test: Key sizes test
echo   - avx_test: AVX support test
echo   - test_compression: Compression test
echo   - test_compression_kem: Compression KEM test
if exist "build\generate_key_images.exe" (
    echo   - generate_key_images: Key image generator
    echo   - test_key_images: Key image test
)
echo.

REM Check if we can run the main benchmark
if exist "build\benchmark_color_kem_timing.exe" (
    echo Running quick verification test...
    cd build
    benchmark_color_kem_timing.exe
    if %errorlevel% equ 0 (
        echo Verification test passed!
    ) else (
        echo Warning: Verification test failed.
    )
    cd ..
) else (
    echo Warning: benchmark_color_kem_timing.exe not found.
)

endlocal
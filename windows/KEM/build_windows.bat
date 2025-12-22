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

REM Set compiler environment variables for CMake
set "CC=C:/msys64/mingw64/bin/gcc.exe"
set "CXX=C:/msys64/mingw64/bin/g++.exe"
set "PATH=C:/msys64/mingw64/bin;%PATH%"
set "PKG_CONFIG_EXECUTABLE=C:/msys64/usr/bin/pkg-config.exe"
set "PKG_CONFIG_PATH=C:/msys64/mingw64/lib/pkgconfig;%PKG_CONFIG_PATH%"
set "OPENSSL_ROOT_DIR=C:/msys64/mingw64/opt"

REM Export environment variables for subprocesses
setx CC "C:/msys64/mingw64/bin/gcc.exe"
setx CXX "C:/msys64/mingw64/bin/g++.exe"

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
cmake .. -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DPKG_CONFIG_EXECUTABLE="C:/msys64/usr/bin/pkg-config.exe"
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
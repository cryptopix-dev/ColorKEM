@echo off
REM Build script for ColorKEM Windows using MinGW with tests
setlocal enabledelayedexpansion

echo ========================================
echo ColorKEM Windows Build Tests Script
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

REM Set compiler environment variables for CMake
set "CC=C:/ProgramData/mingw64/mingw64/bin/gcc.exe"
set "CXX=C:/ProgramData/mingw64/mingw64/bin/g++.exe"
set "PATH=C:/ProgramData/mingw64/mingw64/bin;%PATH%"
set "PKG_CONFIG_EXECUTABLE=C:/msys64/usr/bin/pkg-config.exe"
set "PKG_CONFIG_PATH=C:/msys64/mingw64/lib/pkgconfig;%PKG_CONFIG_PATH%"
set "OPENSSL_ROOT_DIR=C:/ProgramData/mingw64/mingw64/opt"

REM Export environment variables for subprocesses
setx CC "C:/ProgramData/mingw64/mingw64/bin/gcc.exe"
setx CXX "C:/ProgramData/mingw64/mingw64/bin/g++.exe"

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
cmake .. -G "MinGW Makefiles" -DCMAKE_TOOLCHAIN_FILE="mingw-toolchain.cmake" -DPKG_CONFIG_EXECUTABLE="C:/msys64/usr/bin/pkg-config.exe" -DBUILD_TESTS=ON
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
echo Build with tests completed successfully!
echo Executables are available in the build/ directory.

endlocal
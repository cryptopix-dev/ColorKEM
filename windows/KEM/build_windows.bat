@echo off
REM Build script for ColorKEM Windows using MinGW - Dynamic Configuration
setlocal enabledelayedexpansion

echo ========================================
echo ColorKEM Windows Build Script
echo ========================================
echo.

REM Check for required tools
echo Checking for required build tools...
set "requiredTools=cmake gcc g++ openssl"
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

REM dynamically find compilers
for /f "tokens=*" %%i in ('where gcc') do set "GCC_FULL_PATH=%%i" & goto :found_gcc
:found_gcc
for /f "tokens=*" %%i in ('where g++') do set "GXX_FULL_PATH=%%i" & goto :found_gxx
:found_gxx

echo [INFO] C Compiler: !GCC_FULL_PATH!
echo [INFO] C++ Compiler: !GXX_FULL_PATH!

REM Derive MinGW root from GCC path for OpenSSL detection
for %%I in ("!GCC_FULL_PATH!") do set "GCC_BIN=%%~dpI"
REM Remove trailing backslash if present to be safe, though pushd handles it
pushd "!GCC_BIN!.."
set "MINGW_ROOT=!CD!"
popd
echo [INFO] Derived MinGW Root: !MINGW_ROOT!

REM Try to find pkg-config
set "PKG_CONFIG_ARG="
where pkg-config >nul 2>nul
if !errorlevel! equ 0 (
    for /f "tokens=*" %%i in ('where pkg-config') do set "PKG_CONFIG_PATH_EXEC=%%i" & goto :found_pkg
)
:found_pkg
if defined PKG_CONFIG_PATH_EXEC (
    echo [INFO] pkg-config: !PKG_CONFIG_PATH_EXEC!
    set "PKG_CONFIG_ARG=-DPKG_CONFIG_EXECUTABLE="!PKG_CONFIG_PATH_EXEC!""
)

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
REM We pass the compilers explicitly to ensure consistency
REM We also pass OPENSSL_ROOT_DIR based on the compiler location to ensure compat
cmake .. -G "MinGW Makefiles" ^
    -DCMAKE_C_COMPILER="!GCC_FULL_PATH!" ^
    -DCMAKE_CXX_COMPILER="!GXX_FULL_PATH!" ^
    -DCMAKE_TOOLCHAIN_FILE="..\mingw-toolchain.cmake" ^
    -DOPENSSL_ROOT_DIR="!MINGW_ROOT!" ^
    !PKG_CONFIG_ARG!

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
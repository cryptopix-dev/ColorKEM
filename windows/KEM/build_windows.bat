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
if exist "build\test_image_keys.exe" (
    echo   - test_image_keys: Image keys test
)
if exist "build\test_path_config.exe" (
    echo   - test_path_config: Path configuration system test
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
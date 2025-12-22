@echo off
setlocal enabledelayedexpansion

:: ColorKEM Test Runner for Windows
:: This script runs all available tests

:: Set MinGW PATH for DLLs
set "PATH=C:/ProgramData/mingw64/mingw64/bin;%PATH%"

:: Test results
set TOTAL_TESTS=0
set PASSED_TESTS=0
set FAILED_TESTS=0

:: Main script starts here
echo ========================================
echo ColorKEM Verification Test
echo ========================================
echo.

:: Check if build directory exists
if not exist "build" (
    echo Build directory not found. Please run build_windows.bat first.
    exit /b 1
)

echo.
echo Running verification test...
echo ---------------------------

:: Test 1: Parameters
if exist "build\tests\test_clwe_parameters.exe" (
    echo Running Parameters Test...
    build\tests\test_clwe_parameters.exe
    if %errorlevel% equ 0 (
        echo PASSED
        set /a PASSED_TESTS+=1
    ) else (
        echo FAILED
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)

:: Test 2: Color Value
if exist "build\tests\test_color_value.exe" (
    echo Running Color Value Test...
    build\tests\test_color_value.exe
    if %errorlevel% equ 0 (
        echo PASSED
        set /a PASSED_TESTS+=1
    ) else (
        echo FAILED
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)

:: Test 3: Color KEM
if exist "build\tests\test_color_kem.exe" (
    echo Running Color KEM Test...
    build\tests\test_color_kem.exe
    if %errorlevel% equ 0 (
        echo PASSED
        set /a PASSED_TESTS+=1
    ) else (
        echo FAILED
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)

:: Test 4: Serialization
if exist "build\tests\test_serialization.exe" (
    echo Running Serialization Test...
    build\tests\test_serialization.exe
    if %errorlevel% equ 0 (
        echo PASSED
        set /a PASSED_TESTS+=1
    ) else (
        echo FAILED
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)

:: Test 5: Utils
if exist "build\tests\test_utils.exe" (
    echo Running Utils Test...
    build\tests\test_utils.exe
    if %errorlevel% equ 0 (
        echo PASSED
        set /a PASSED_TESTS+=1
    ) else (
        echo FAILED
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)

:: Test 6: NTT Engine
if exist "build\tests\test_ntt_engine.exe" (
    echo Running NTT Engine Test...
    build\tests\test_ntt_engine.exe
    if %errorlevel% equ 0 (
        echo PASSED
        set /a PASSED_TESTS+=1
    ) else (
        echo FAILED
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)

:: Test 7: Sampling
if exist "build\tests\test_sampling.exe" (
    echo Running Sampling Test...
    build\tests\test_sampling.exe
    if %errorlevel% equ 0 (
        echo PASSED
        set /a PASSED_TESTS+=1
    ) else (
        echo FAILED
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)

:: Test 8: Integration KEM
if exist "build\tests\test_integration_kem.exe" (
    echo Running Integration KEM Test...
    build\tests\test_integration_kem.exe
    if %errorlevel% equ 0 (
        echo PASSED
        set /a PASSED_TESTS+=1
    ) else (
        echo FAILED
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)

:: Test 9: Known Answer Tests
if exist "build\tests\test_known_answer_tests.exe" (
    echo Running KAT Test...
    build\tests\test_known_answer_tests.exe
    if %errorlevel% equ 0 (
        echo PASSED
        set /a PASSED_TESTS+=1
    ) else (
        echo FAILED
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)

:: Test 10: Performance Metrics
if exist "build\tests\test_performance_metrics.exe" (
    echo Running Performance Metrics Test...
    build\tests\test_performance_metrics.exe
    if %errorlevel% equ 0 (
        echo PASSED
        set /a PASSED_TESTS+=1
    ) else (
        echo FAILED
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)

:: Test 11: Key Images (if WebP available)
if exist "build\test_key_images.exe" (
    echo Running Key Images Test...
    build\test_key_images.exe
    if %errorlevel% equ 0 (
        echo PASSED
        set /a PASSED_TESTS+=1
    ) else (
        echo FAILED
        set /a FAILED_TESTS+=1
    )
    set /a TOTAL_TESTS+=1
)

echo.
echo ========================================
echo Test Summary
=======================================
echo Total Tests: !TOTAL_TESTS!
echo Passed: !PASSED_TESTS!
echo Failed: !FAILED_TESTS!

if !FAILED_TESTS! equ 0 (
    echo All tests passed!
    exit /b 0
) else (
    echo Some tests failed!
    exit /b 1
)
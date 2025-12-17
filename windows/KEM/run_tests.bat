@echo off
echo ========================================
ColorKEM Comprehensive Test Suite
=======================================
echo.
echo.

set TotalTests=3
set PassedTests=0
set FailedTests=0

if not exist "build" (
    echo Build directory not found. Please run build_windows.bat first.
    exit /b 1
)

echo.
echo Running test suites...
echo ----------------------

echo Running ColorKEM comprehensive test...
call build\test_color_kem.exe
if %errorlevel% equ 0 (
    echo PASSED
    set /a PassedTests+=1
) else (
    echo FAILED
    set /a FailedTests+=1
)

echo.
echo Running Image Keys test...
call build\test_image_keys.exe
if %errorlevel% equ 0 (
    echo PASSED
    set /a PassedTests+=1
) else (
    echo FAILED
    set /a FailedTests+=1
)

echo.
echo Running Path Configuration test...
call build\test_path_config.exe
if %errorlevel% equ 0 (
    echo PASSED
    set /a PassedTests+=1
) else (
    echo FAILED
    set /a FailedTests+=1
)

echo.
echo ========================================
echo Test Summary
echo =======================================
echo Total Tests: %TotalTests%
echo Passed: %PassedTests%
echo Failed: %FailedTests%

if %FailedTests% equ 0 (
    echo All tests passed!
    exit /b 0
) else (
    echo Some tests failed!
    exit /b 1
)
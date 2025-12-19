# Test Build Script
# This script tests if the mingwex library fix works correctly

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Testing ColorKEM Build with mingwex Fix" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Clean build directory
if (Test-Path "build") {
    Write-Host "Cleaning build directory..." -ForegroundColor Yellow
    Remove-Item -Recurse -Force build
}

# Run the build
Write-Host "Running build_windows.bat..." -ForegroundColor Yellow
& .\build_windows.bat

if ($LASTEXITCODE -eq 0) {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "BUILD SUCCESSFUL!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "The mingwex library fix is working correctly." -ForegroundColor Green
} else {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Red
    Write-Host "BUILD FAILED!" -ForegroundColor Red
    Write-Host "========================================" -ForegroundColor Red
    Write-Host ""
    Write-Host "Please check the error messages above." -ForegroundColor Red
    Write-Host "The mingwex library may not be found in the expected locations." -ForegroundColor Red
    Write-Host ""
    Write-Host "Troubleshooting steps:" -ForegroundColor Yellow
    Write-Host "1. Check if mingwex library exists in OpenSSL lib directory" -ForegroundColor Yellow
    Write-Host "2. Verify GCC and OpenSSL are from compatible MinGW installations" -ForegroundColor Yellow
    Write-Host "3. Review LINKER_FIX_README.md for more details" -ForegroundColor Yellow
}

@echo off
REM ColorKEM Configuration System Test Script
REM This script demonstrates the dynamic path configuration system

echo ==========================================
echo ColorKEM Dynamic Path Configuration Test
echo ==========================================
echo.

REM Create test directories
echo Creating test directories...
if not exist "test_config_output" mkdir test_config_output
if not exist "test_config_input" mkdir test_config_input
if not exist "test_config_temp" mkdir test_config_temp
if not exist "test_config_logs" mkdir test_config_logs
echo Test directories created.
echo.

REM Test 1: Default configuration
echo Test 1: Default Configuration
echo ================================
echo Testing default paths (current directory)...
echo Note: This test requires the application to be built first
echo Command: generate_key_images.exe --env testing
echo.

REM Test 2: Environment variable configuration
echo Test 2: Environment Variable Configuration
echo ===========================================
echo Setting environment variables...
set COLOR_KEM_KEY_OUTPUT_DIR=test_config_output
set COLOR_KEM_KEY_INPUT_DIR=test_config_input
set COLOR_KEM_TEMP_DIR=test_config_temp
set COLOR_KEM_LOG_DIR=test_config_logs
set COLOR_KEM_ENV=development
echo Environment variables set.
echo Testing with environment variables...
echo Command: generate_key_images.exe
echo.

REM Test 3: Command-line configuration
echo Test 3: Command-Line Configuration
echo ===================================
echo Testing command-line path override...
echo Command: generate_key_images.exe --key-output-dir custom_output --env production
echo.

REM Test 4: Configuration file test
echo Test 4: Configuration File Test
echo ================================
echo Creating test configuration file...
echo [test_config] > test_config.ini
echo key_output_dir = file_config_output >> test_config.ini
echo key_input_dir = file_config_input >> test_config.ini
echo temp_dir = file_config_temp >> test_config.ini
echo log_dir = file_config_logs >> test_config.ini
echo Configuration file created.
_key_images.exe --echo Command: generateconfig test_config.ini --env test_config
echo.

REM Test 5: Path validation test
echo Test 5: Path Validation Test
echo ==============================
echo Testing path validation and directory creation...
echo Creating deep nested path test...
set COLOR_KEM_KEY_OUTPUT_DIR=nested\deep\path\test
echo Command: generate_key_images.exe --key-output-dir nested\deep\path\test --validate
echo.

REM Test 6: Environment presets test
echo Test 6: Environment Presets Test
echo =================================
echo Testing different environment presets...
echo.
echo Testing development environment:
echo Command: generate_key_images.exe --env development
echo.
echo Testing production environment:
echo Command: generate_key_images.exe --env production
echo.
echo Testing testing environment:
echo Command: generate_key_images.exe --env testing
echo.
echo Testing CI environment:
echo Command: generate_key_images.exe --env ci
echo.

REM Summary
echo ==========================================
echo Configuration Test Summary
echo ==========================================
echo.
echo The configuration system supports:
echo 1. Default paths (current directory)
echo 2. Environment variables (COLOR_KEM_*)
echo 3. Command-line arguments (--key-output-dir, etc.)
echo 4. Configuration files (INI format)
echo 5. Environment presets (development, production, testing, ci)
echo 6. Path validation and automatic directory creation
echo 7. Environment variable expansion (%%USERPROFILE%%, etc.)
echo.
echo Configuration Methods Demonstrated:
echo - Environment variables: COLOR_KEM_KEY_OUTPUT_DIR, etc.
echo - Command line: --key-output-dir, --config, --env
echo - Preset environments: development, production, testing, ci
echo.
echo Files Created:
echo - test_config.ini (configuration file example)
echo - test_config_output/ (environment variable test)
echo - file_config_output/ (configuration file test)
echo - nested/deep/path/test/ (validation test)
echo.
echo For more information, see PATH_CONFIGURATION.md
echo.

REM Cleanup (optional)
echo Cleaning up test files...
if exist test_config_output rmdir /s /q test_config_output
if exist test_config_input rmdir /s /q test_config_input
if exist test_config_temp rmdir /s /q test_config_temp
if exist test_config_logs rmdir /s /q test_config_logs
if exist file_config_output rmdir /s /q file_config_output
if exist file_config_input rmdir /s /q file_config_input
if exist file_config_temp rmdir /s /q file_config_temp
if exist file_config_logs rmdir /s /q file_config_logs
if exist nested rmdir /s /q nested
if exist test_config.ini del test_config.ini
echo Test files cleaned up.
echo.

echo Configuration system test completed!
echo To run actual tests, build the application first and then execute the commands shown above.
pause
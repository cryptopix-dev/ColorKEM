# ColorKEM Dynamic Path Configuration System

The ColorKEM implementation now includes a comprehensive dynamic path configuration system that allows flexible deployment across different environments without hardcoded paths.

## Overview

The configuration system provides:
- **Runtime path configuration** for file operations
- **Environment variable support** for system integration
- **Configuration file support** for persistent settings
- **Command-line argument support** for dynamic overrides
- **Environment presets** for common deployment scenarios
- **Cross-platform compatibility** with Windows path handling

## Features

### 1. Configuration Sources (Priority Order)
1. **Command-line arguments** (highest priority)
2. **Environment variables**
3. **Configuration files**
4. **Default paths** (lowest priority)

### 2. Path Categories
- `key_output_dir` - Directory for saving generated keys
- `key_input_dir` - Directory for loading keys
- `temp_dir` - Temporary file directory
- `log_dir` - Log file directory
- `lib_dir` - Library directory
- `include_dir` - Include directory

### 3. Environment Presets
- `development` - Local development paths
- `production` - User profile based paths
- `testing` - Test-specific paths
- `ci` - Continuous integration paths

## Configuration Methods

### Method 1: Configuration File
Create a configuration file (e.g., `color_kem_config.ini`):

```ini
[production]
key_output_dir = %USERPROFILE%\ColorKEM\keys\output
key_input_dir = %USERPROFILE%\ColorKEM\keys\input
temp_dir = %USERPROFILE%\ColorKEM\temp
log_dir = %USERPROFILE%\ColorKEM\logs
```

Load the configuration:
```cpp
auto& config = clwe::config::PathConfig::getInstance();
config.loadFromFile("color_kem_config.ini");
```

### Method 2: Environment Variables
Set environment variables before running the application:

```cmd
set COLOR_KEM_KEY_OUTPUT_DIR=C:\ColorKEM\keys
set COLOR_KEM_KEY_INPUT_DIR=C:\ColorKEM\keys
set COLOR_KEM_TEMP_DIR=C:\ColorKEM\temp
set COLOR_KEM_LOG_DIR=C:\ColorKEM\logs
set COLOR_KEM_ENV=production
```

Available environment variables:
- `COLOR_KEM_KEY_OUTPUT_DIR`
- `COLOR_KEM_KEY_INPUT_DIR`
- `COLOR_KEM_TEMP_DIR`
- `COLOR_KEM_LOG_DIR`
- `COLOR_KEM_LIB_DIR`
- `COLOR_KEM_INCLUDE_DIR`
- `COLOR_KEM_ENV`

### Method 3: Command-Line Arguments
Pass configuration via command line:

```cmd
generate_key_images.exe --key-output-dir C:\ColorKEM\keys --env production
test_key_images.exe --key-input-dir C:\ColorKEM\keys --config color_kem_config.ini
```

Available command-line options:
- `--key-output-dir <path>` - Set output directory
- `--key-input-dir <path>` - Set input directory
- `--temp-dir <path>` - Set temporary directory
- `--log-dir <path>` - Set log directory
- `--config <file>` - Load configuration from file
- `--env <environment>` - Set environment preset

### Method 4: Programmatic Configuration
Configure paths directly in code:

```cpp
auto& config = clwe::config::PathConfig::getInstance();
config.setKeyOutputDirectory("C:\\ColorKEM\\keys");
config.setEnvironment("production");
config.createDirectories(); // Create directories if they don't exist
```

## Usage Examples

### Example 1: Development Environment
```cmd
set COLOR_KEM_ENV=development
generate_key_images.exe
```
Uses local `./debug_output`, `./debug_input`, etc.

### Example 2: Production Environment
```cmd
set COLOR_KEM_ENV=production
generate_key_images.exe --key-output-dir D:\SecureKeys
```
Uses user profile paths with custom output directory.

### Example 3: Custom Configuration File
Create `my_config.ini`:
```ini
[my_deployment]
key_output_dir = E:\ApplicationData\Keys
temp_dir = E:\ApplicationData\Temp
```

Run with custom config:
```cmd
generate_key_images.exe --config my_config.ini --env my_deployment
```

### Example 4: CI/CD Environment
```cmd
set COLOR_KEM_ENV=ci
generate_key_images.exe
```
Uses CI-specific paths `./ci_keys`, `./ci_temp`, etc.

## Environment Variable Expansion

The system supports environment variable expansion using `%VAR%` syntax:

```ini
[key_expansion]
key_output_dir = %USERPROFILE%\ColorKEM\keys
temp_dir = %TEMP%\ColorKEM
log_dir = %APPDATA%\ColorKEM\logs
```

Supported Windows environment variables:
- `USERPROFILE` - User profile directory
- `TEMP` or `TMP` - Temporary directory
- `APPDATA` - Application data directory
- `PROGRAMFILES` - Program files directory
- `SYSTEMROOT` - Windows system directory

## Path Validation and Creation

The configuration system automatically:
1. **Validates paths** - Checks if directories exist or can be created
2. **Creates directories** - Automatically creates missing directories
3. **Expands environment variables** - Resolves `%VAR%` patterns
4. **Normalizes paths** - Ensures proper path formatting

```cpp
auto& config = clwe::config::PathConfig::getInstance();
config.setDefaultPaths();

if (!config.validatePaths()) {
    std::cerr << "Invalid configuration!" << std::endl;
    return 1;
}

config.createDirectories(); // Create all necessary directories
```

## Default Paths

When no configuration is provided, the system uses these defaults:

- **Key directories**: Current working directory (`.`)
- **Temporary directory**: System temp directory + `ColorKEM\`
- **Log directory**: Current working directory + `logs\`
- **Library directory**: Application directory + `lib\`
- **Include directory**: Application directory + `include\`

## Best Practices

### 1. Environment Separation
Use different paths for different environments:
- Development: `./debug_*`
- Testing: `./test_*`
- Production: `%USERPROFILE%\ColorKEM\*`

### 2. Security Considerations
- Use secure directories for key storage
- Set appropriate file permissions
- Avoid world-writable directories for keys

### 3. Backup and Recovery
- Configure log directories for audit trails
- Use version control for configuration files
- Document environment-specific settings

### 4. Deployment Automation
- Use environment variables in deployment scripts
- Include configuration validation in CI/CD
- Test configuration in staging environments

## Troubleshooting

### Common Issues

1. **Directory Creation Fails**
   - Check write permissions
   - Verify path validity
   - Ensure parent directories exist

2. **Environment Variables Not Found**
   - Verify variable names (case-sensitive)
   - Check if variables are set in correct scope
   - Restart application after setting variables

3. **Configuration File Not Loaded**
   - Verify file path and permissions
   - Check file format (INI style)
   - Ensure proper section headers

4. **Path Conflicts**
   - Use absolute paths when possible
   - Check for special characters in paths
   - Verify path length limits

### Debug Configuration
Enable verbose logging to troubleshoot configuration issues:

```cpp
auto& config = clwe::config::PathConfig::getInstance();
config.setLogDirectory("./config_logs");
config.validatePaths(); // This will create log files with detailed information
```

## Integration with Existing Code

The configuration system is designed to be non-intrusive. Existing code can be gradually migrated:

```cpp
// Old way (hardcoded)
std::string output_dir = ".";
std::string filename = output_dir + "/public_key.webp";

// New way (dynamic)
auto& config = clwe::config::PathConfig::getInstance();
std::string filename = config.getKeyFilePath("public_key.webp", true);
```

Both approaches work, but the new dynamic approach provides much more flexibility for different deployment scenarios.

## Performance Considerations

- Configuration is loaded once at application startup
- Path resolution is cached for performance
- Environment variable expansion is done once
- Directory validation can be skipped for known-good configurations

## Security Notes

- Configuration files should have appropriate access controls
- Environment variables should be set securely
- Key directories should have restricted access
- Log files may contain sensitive information and should be protected

This dynamic path configuration system makes the ColorKEM implementation much more flexible and suitable for various deployment scenarios while maintaining security and performance.
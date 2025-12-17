# ColorKEM CMake Configuration Template
# This file provides flexible build-time configuration options

# Set default values
set(DEFAULT_COLOR_KEM_OUTPUT_DIR "${CMAKE_BINARY_DIR}/output" CACHE PATH "Default output directory for generated keys")
set(DEFAULT_COLOR_KEM_INPUT_DIR "${CMAKE_BINARY_DIR}/input" CACHE PATH "Default input directory for loading keys")
set(DEFAULT_COLOR_KEM_TEMP_DIR "${CMAKE_BINARY_DIR}/temp" CACHE PATH "Default temporary directory")
set(DEFAULT_COLOR_KEM_LOG_DIR "${CMAKE_BINARY_DIR}/logs" CACHE PATH "Default log directory")

# Environment-specific configurations
set(COLOR_KEM_ENVIRONMENT "development" CACHE STRING "Build environment: development, production, testing, ci")
set_property(CACHE COLOR_KEM_ENVIRONMENT PROPERTY STRINGS "development" "production" "testing" "ci")

# Platform-specific settings
if(WIN32)
    set(DEFAULT_OPENSSL_ROOT_DIR "C:/ProgramData/mingw64/mingw64/opt" CACHE PATH "OpenSSL root directory")
    set(DEFAULT_WEBP_INCLUDE_DIR "C:/ProgramData/mingw64/mingw64/include" CACHE PATH "WebP include directory")
    set(DEFAULT_WEBP_LIBRARY_DIR "C:/ProgramData/mingw64/mingw64/lib" CACHE PATH "WebP library directory")
elseif(UNIX)
    set(DEFAULT_OPENSSL_ROOT_DIR "/usr" CACHE PATH "OpenSSL root directory")
    set(DEFAULT_WEBP_INCLUDE_DIR "/usr/include" CACHE PATH "WebP include directory")
    set(DEFAULT_WEBP_LIBRARY_DIR "/usr/lib" CACHE PATH "WebP library directory")
endif()

# Allow environment variable overrides
if(DEFINED ENV{COLOR_KEM_OUTPUT_DIR})
    set(COLOR_KEM_OUTPUT_DIR $ENV{COLOR_KEM_OUTPUT_DIR} CACHE PATH "Output directory (from environment)")
endif()

if(DEFINED ENV{COLOR_KEM_INPUT_DIR})
    set(COLOR_KEM_INPUT_DIR $ENV{COLOR_KEM_INPUT_DIR} CACHE PATH "Input directory (from environment)")
endif()

if(DEFINED ENV{COLOR_KEM_TEMP_DIR})
    set(COLOR_KEM_TEMP_DIR $ENV{COLOR_KEM_TEMP_DIR} CACHE PATH "Temp directory (from environment)")
endif()

if(DEFINED ENV{COLOR_KEM_LOG_DIR})
    set(COLOR_KEM_LOG_DIR $ENV{COLOR_KEM_LOG_DIR} CACHE PATH "Log directory (from environment)")
endif()

# Apply environment presets
if(COLOR_KEM_ENVIRONMENT STREQUAL "development")
    set(COLOR_KEM_OUTPUT_DIR "${CMAKE_SOURCE_DIR}/debug_output" CACHE PATH "Development output directory")
    set(COLOR_KEM_INPUT_DIR "${CMAKE_SOURCE_DIR}/debug_input" CACHE PATH "Development input directory")
    set(COLOR_KEM_TEMP_DIR "${CMAKE_SOURCE_DIR}/debug_temp" CACHE PATH "Development temp directory")
    set(COLOR_KEM_LOG_DIR "${CMAKE_SOURCE_DIR}/debug_logs" CACHE PATH "Development log directory")
elseif(COLOR_KEM_ENVIRONMENT STREQUAL "production")
    if(WIN32)
        set(COLOR_KEM_OUTPUT_DIR "$ENV{USERPROFILE}/ColorKEM/keys/output" CACHE PATH "Production output directory")
        set(COLOR_KEM_INPUT_DIR "$ENV{USERPROFILE}/ColorKEM/keys/input" CACHE PATH "Production input directory")
        set(COLOR_KEM_TEMP_DIR "$ENV{USERPROFILE}/ColorKEM/temp" CACHE PATH "Production temp directory")
        set(COLOR_KEM_LOG_DIR "$ENV{USERPROFILE}/ColorKEM/logs" CACHE PATH "Production log directory")
    else()
        set(COLOR_KEM_OUTPUT_DIR "$ENV{HOME}/.colorkem/keys/output" CACHE PATH "Production output directory")
        set(COLOR_KEM_INPUT_DIR "$ENV{HOME}/.colorkem/keys/input" CACHE PATH "Production input directory")
        set(COLOR_KEM_TEMP_DIR "$ENV{HOME}/.colorkem/temp" CACHE PATH "Production temp directory")
        set(COLOR_KEM_LOG_DIR "$ENV{HOME}/.colorkem/logs" CACHE PATH "Production log directory")
    endif()
elseif(COLOR_KEM_ENVIRONMENT STREQUAL "testing")
    set(COLOR_KEM_OUTPUT_DIR "${CMAKE_BINARY_DIR}/test_output" CACHE PATH "Testing output directory")
    set(COLOR_KEM_INPUT_DIR "${CMAKE_BINARY_DIR}/test_input" CACHE PATH "Testing input directory")
    set(COLOR_KEM_TEMP_DIR "${CMAKE_BINARY_DIR}/test_temp" CACHE PATH "Testing temp directory")
    set(COLOR_KEM_LOG_DIR "${CMAKE_BINARY_DIR}/test_logs" CACHE PATH "Testing log directory")
elseif(COLOR_KEM_ENVIRONMENT STREQUAL "ci")
    set(COLOR_KEM_OUTPUT_DIR "${CMAKE_BINARY_DIR}/ci_keys" CACHE PATH "CI output directory")
    set(COLOR_KEM_INPUT_DIR "${CMAKE_BINARY_DIR}/ci_keys" CACHE PATH "CI input directory")
    set(COLOR_KEM_TEMP_DIR "${CMAKE_BINARY_DIR}/ci_temp" CACHE PATH "CI temp directory")
    set(COLOR_KEM_LOG_DIR "${CMAKE_BINARY_DIR}/ci_logs" CACHE PATH "CI log directory")
endif()

# Set defaults if not already set
if(NOT COLOR_KEM_OUTPUT_DIR)
    set(COLOR_KEM_OUTPUT_DIR "${DEFAULT_COLOR_KEM_OUTPUT_DIR}" CACHE PATH "Output directory")
endif()

if(NOT COLOR_KEM_INPUT_DIR)
    set(COLOR_KEM_INPUT_DIR "${DEFAULT_COLOR_KEM_INPUT_DIR}" CACHE PATH "Input directory")
endif()

if(NOT COLOR_KEM_TEMP_DIR)
    set(COLOR_KEM_TEMP_DIR "${DEFAULT_COLOR_KEM_TEMP_DIR}" CACHE PATH "Temp directory")
endif()

if(NOT COLOR_KEM_LOG_DIR)
    set(COLOR_KEM_LOG_DIR "${DEFAULT_COLOR_KEM_LOG_DIR}" CACHE PATH "Log directory")
endif()

# Create directories
file(MAKE_DIRECTORY "${COLOR_KEM_OUTPUT_DIR}")
file(MAKE_DIRECTORY "${COLOR_KEM_INPUT_DIR}")
file(MAKE_DIRECTORY "${COLOR_KEM_TEMP_DIR}")
file(MAKE_DIRECTORY "${COLOR_KEM_LOG_DIR}")

# Generate configuration header
configure_file(
    "${CMAKE_SOURCE_DIR}/cmake/config.h.in"
    "${CMAKE_BINARY_DIR}/generated_config.h"
    @ONLY
)

# Print configuration summary
message(STATUS "ColorKEM Configuration Summary:")
message(STATUS "  Environment: ${COLOR_KEM_ENVIRONMENT}")
message(STATUS "  Output Directory: ${COLOR_KEM_OUTPUT_DIR}")
message(STATUS "  Input Directory: ${COLOR_KEM_INPUT_DIR}")
message(STATUS "  Temp Directory: ${COLOR_KEM_TEMP_DIR}")
message(STATUS "  Log Directory: ${COLOR_KEM_LOG_DIR}")
message(STATUS "  OpenSSL Root: ${DEFAULT_OPENSL_ROOT_DIR}")
message(STATUS "  WebP Include: ${DEFAULT_WEBP_INCLUDE_DIR}")
message(STATUS "  WebP Library: ${DEFAULT_WEBP_LIBRARY_DIR}")

# Export variables for use in other CMake files
set(COLOR_KEM_OUTPUT_DIR "${COLOR_KEM_OUTPUT_DIR}" PARENT_SCOPE)
set(COLOR_KEM_INPUT_DIR "${COLOR_KEM_INPUT_DIR}" PARENT_SCOPE)
set(COLOR_KEM_TEMP_DIR "${COLOR_KEM_TEMP_DIR}" PARENT_SCOPE)
set(COLOR_KEM_LOG_DIR "${COLOR_KEM_LOG_DIR}" PARENT_SCOPE)
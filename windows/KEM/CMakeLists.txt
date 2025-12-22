cmake_minimum_required(VERSION 3.16)
project(ColorKEM VERSION 1.0.0 LANGUAGES C CXX)

# Compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Multi-architecture SIMD detection and configuration
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)

# Windows-specific settings
if(WIN32)
    add_definitions(-D_MINGW -DWIN32 -D_WIN32)
endif()

# Architecture-specific SIMD detection
if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)|(x86_64)|(X86_64)")
    # x86/x64 architecture - check for AVX support
    check_cxx_compiler_flag("-mavx2" AVX2_SUPPORTED)
    check_cxx_compiler_flag("-mfma" FMA_SUPPORTED)

    if(AVX2_SUPPORTED)
        add_compile_definitions(HAVE_AVX2)
        add_compile_options(-mavx2)
        if(FMA_SUPPORTED)
            add_compile_options(-mfma)
        endif()
        message(STATUS "x86_64: AVX2 support detected and enabled")

        # AVX-512 detection skipped for stability
        message(STATUS "x86_64: AVX-512 detection skipped (using AVX2)")
        # check_cxx_compiler_flag("-mavx512f" AVX512F_SUPPORTED)
        # ...
    else()
        message(WARNING "x86_64: AVX2 not supported, using scalar fallback")
        add_compile_definitions(NO_AVX_SUPPORT)
    endif()

elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "(aarch64)|(arm64)|(ARM64)")
    # ARM64 architecture - check for NEON support
    check_cxx_compiler_flag("-march=armv8-a+simd" NEON_SUPPORTED)
    if(NEON_SUPPORTED)
        add_compile_definitions(HAVE_NEON)
        add_compile_options(-march=armv8-a+simd)
        message(STATUS "ARM64: NEON SIMD support detected and enabled")
    else()
        message(WARNING "ARM64: NEON not supported, using scalar fallback")
        add_compile_definitions(NO_SIMD_SUPPORT)
    endif()

else()
    # Unknown architecture
    message(STATUS "Unknown architecture ${CMAKE_SYSTEM_PROCESSOR}, using scalar fallback")
    add_compile_definitions(NO_SIMD_SUPPORT)
endif()

# Cross-compilation support
if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compilation detected - SIMD features will be determined at runtime")
    add_compile_definitions(CROSS_COMPILING)
endif()

# Dependencies - Use MinGW's built-in OpenSSL
set(OPENSSL_ROOT_DIR "C:/msys64/mingw64")
set(OPENSSL_USE_STATIC_LIBS TRUE)
set(OPENSSL_INCLUDE_DIR "C:/msys64/mingw64/include")
set(OPENSSL_CRYPTO_LIBRARY "C:/msys64/mingw64/lib/libcrypto.a")
set(OPENSSL_SSL_LIBRARY "C:/msys64/mingw64/lib/libssl.a")

# Automatically detect OpenSSL root based on compiler location to avoid mismatches
if(WIN32 AND MINGW AND NOT DEFINED OPENSSL_ROOT_DIR)
    # Ensure we have the full path to the compiler
    if(NOT IS_ABSOLUTE "${CMAKE_C_COMPILER}")
       find_program(FULL_C_COMPILER_PATH ${CMAKE_C_COMPILER})
    else()
       set(FULL_C_COMPILER_PATH ${CMAKE_C_COMPILER})
    endif()

    if(FULL_C_COMPILER_PATH)
        get_filename_component(COMPILER_DIR ${FULL_C_COMPILER_PATH} DIRECTORY)
        get_filename_component(MINGW_TOOLCHAIN_ROOT ${COMPILER_DIR} DIRECTORY)
        
        # Check standard locations relative to the compiler toolchain
        # This prevents picking up 'ProgramData' libs when using 'MSYS2' compiler, and vice-versa
        if(EXISTS "${MINGW_TOOLCHAIN_ROOT}/include/openssl/ssl.h" OR EXISTS "${MINGW_TOOLCHAIN_ROOT}/lib/libcrypto.a")
            message(STATUS "Auto-detected OpenSSL Root from Compiler Path: ${MINGW_TOOLCHAIN_ROOT}")
            set(OPENSSL_ROOT_DIR ${MINGW_TOOLCHAIN_ROOT})
        endif()
    endif()
endif()

# Find zlib manually for OpenSSL
find_path(ZLIB_INCLUDE_DIR zlib.h PATHS C:/msys64/mingw64/include)
find_library(ZLIB_LIBRARY libz.a PATHS C:/msys64/mingw64/lib)

if(ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
    set(ZLIB_FOUND TRUE)
    set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
else()
    set(ZLIB_FOUND FALSE)
endif()

# Find OpenSSL - should work with MinGW's built-in version
find_package(OpenSSL REQUIRED)

# Find WebP manually - prefer static libraries
find_path(WEBP_INCLUDE_DIR webp/types.h PATHS C:/msys64/mingw64/include)
find_library(WEBP_LIBRARY libwebp.a PATHS C:/msys64/mingw64/lib)
find_library(WEBPDEMUX_LIBRARY libwebpdemux.a PATHS C:/msys64/mingw64/lib)
find_library(WEBPMUX_LIBRARY libwebpmux.a PATHS C:/msys64/mingw64/lib)
find_library(SHARPYUV_LIBRARY libsharpyuv.a PATHS C:/msys64/mingw64/lib)

if(WEBP_INCLUDE_DIR AND WEBP_LIBRARY)
    set(WEBP_FOUND TRUE)
    set(WEBP_LIBRARIES ${WEBP_LIBRARY} ${WEBPDEMUX_LIBRARY} ${WEBPMUX_LIBRARY} ${SHARPYUV_LIBRARY})
else()
    set(WEBP_FOUND FALSE)
endif()

# Add OpenSSL include directories explicitly for Windows
if(WIN32)
    include_directories(${OPENSSL_INCLUDE_DIR})
    if(WEBP_FOUND)
        include_directories(${WEBP_INCLUDE_DIR})
    endif()
endif()

# Google Test - Use system MinGW version if available, otherwise download
find_package(GTest QUIET)
if(NOT GTest_FOUND)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
      DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    # For Windows: Prevent overriding the parent project's compiler/linker on Windows
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # Set compiler for subproject before making it available
    set(CMAKE_C_COMPILER "C:/msys64/mingw64/bin/gcc.exe" CACHE FILEPATH "C compiler" FORCE)
    set(CMAKE_CXX_COMPILER "C:/msys64/mingw64/bin/g++.exe" CACHE FILEPATH "C++ compiler" FORCE)

    # Suppress CMake deprecation warnings from googletest
    set(CMAKE_POLICY_VERSION_MINIMUM 3.16)
    FetchContent_MakeAvailable(googletest)
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core)

# Library target
add_library(clwe STATIC
    src/core/parameters.cpp
    src/core/ntt_engine.cpp
    src/core/ntt_scalar.cpp
    src/core/color_value.cpp
    src/core/color_ntt_engine.cpp
    src/core/color_kem.cpp
    src/core/color_integration.cpp
    src/core/cpu_features.cpp
    src/core/shake_sampler.cpp
    src/core/sampling.cpp
    src/core/utils.cpp
    src/core/performance_metrics.cpp
    src/core/tiny_sha3.c
)

target_link_libraries(clwe PRIVATE OpenSSL::Crypto)
if(ZLIB_FOUND)
    target_link_libraries(clwe PRIVATE ${ZLIB_LIBRARIES})
endif()

# Windows Cryptography API for secure random
if(WIN32)
    target_link_libraries(clwe PRIVATE crypt32 bcrypt ws2_32)
    
    # Find mingwex library once for all executables
    # This library provides vsnprintf and other standard C functions needed by OpenSSL
    find_library(MINGWEX_LIBRARY 
        NAMES mingwex libmingwex
        PATHS 
            ${OPENSSL_ROOT_DIR}/lib
            ${OPENSSL_ROOT_DIR}/../lib
            C:/msys64/mingw64/lib
            C:/ProgramData/mingw64/mingw64/lib
        NO_DEFAULT_PATH
    )
    
    # If not found in specific paths, try system search
    if(NOT MINGWEX_LIBRARY)
        find_library(MINGWEX_LIBRARY NAMES mingwex libmingwex)
    endif()
    
    if(MINGWEX_LIBRARY)
        message(STATUS "Found mingwex library: ${MINGWEX_LIBRARY}")
    else()
        message(WARNING "mingwex library not found in standard locations, will attempt to link by name")
    endif()
    
    # Function to add Windows-specific libraries to executables
    function(add_windows_openssl_libs target_name)
        # Add linker flags to handle potential symbol mismatches
        target_link_options(${target_name} PRIVATE -Wl,--allow-multiple-definition)
        
        # Link OpenSSL libraries
        target_link_libraries(${target_name} PRIVATE OpenSSL::Crypto OpenSSL::SSL)
        
        # Add mingwex library
        if(MINGWEX_LIBRARY)
            target_link_libraries(${target_name} PRIVATE ${MINGWEX_LIBRARY})
        else()
            target_link_libraries(${target_name} PRIVATE mingwex)
        endif()
        
        # Add Windows system libraries
        target_link_libraries(${target_name} PRIVATE crypt32 bcrypt ws2_32 gdi32)
        
        # Add zlib if found
        if(ZLIB_FOUND)
            target_link_libraries(${target_name} PRIVATE ${ZLIB_LIBRARIES})
        endif()
    endfunction()
endif()

# KEM demonstration - commented out as demo_kem.cpp is missing
# add_executable(demo_kem demo_kem.cpp)
# target_link_libraries(demo_kem PRIVATE clwe)

# Color KEM timing benchmark
add_executable(benchmark_color_kem_timing benchmark_color_kem_timing.cpp)
target_link_libraries(benchmark_color_kem_timing PRIVATE clwe)

if(WEBP_FOUND)
    # Key image generator executable
    add_executable(generate_key_images generate_key_images.cpp)
    target_link_libraries(generate_key_images PRIVATE clwe ${WEBP_LIBRARIES})
    
    # Windows-specific libraries for OpenSSL static linking
    if(WIN32)
        add_windows_openssl_libs(generate_key_images)
    endif()

    # Key image test executable
    add_executable(test_key_images test_key_images.cpp)
    target_link_libraries(test_key_images PRIVATE clwe ${WEBP_LIBRARIES})
    
    # Windows-specific libraries for OpenSSL static linking
    if(WIN32)
        add_windows_openssl_libs(test_key_images)
    endif()

    # Add test for key images
    add_test(NAME TestKeyImages COMMAND test_key_images)
endif()

# Tests
enable_testing()

# Add tests subdirectory
add_subdirectory(tests)

# Add comprehensive test target - run all tests including unit tests
add_custom_target(run_all_tests
    COMMAND ${CMAKE_COMMAND} -E echo "Running basic verification test..."
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running ColorKEM verification and unit tests"
)

# Add benchmark as a test
add_test(NAME BenchmarkTest COMMAND benchmark_color_kem_timing)

# Installation
install(TARGETS clwe
    EXPORT ColorKEMTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    INCLUDES DESTINATION include
)

install(DIRECTORY src/include/clwe/
    DESTINATION include/clwe
    FILES_MATCHING PATTERN "*.hpp"
)

# Export targets
install(EXPORT ColorKEMTargets
    FILE ColorKEMTargets.cmake
    NAMESPACE ColorKEM::
    DESTINATION lib/cmake/ColorKEM
)

# Python bindings (optional)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Fuzzing support (libFuzzer)
option(ENABLE_FUZZING "Enable fuzzing with libFuzzer" OFF)
if(ENABLE_FUZZING)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        message(STATUS "Enabling libFuzzer support")
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=fuzzer")
        set(CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fsanitize=fuzzer")
    else()
        message(WARNING "libFuzzer requires Clang compiler. Fuzzing disabled.")
        set(ENABLE_FUZZING OFF)
    endif()
endif()
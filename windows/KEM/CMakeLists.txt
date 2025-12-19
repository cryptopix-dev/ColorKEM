cmake_minimum_required(VERSION 3.16)
project(ColorKEM VERSION 1.0.0 LANGUAGES C CXX)

# Toolchain will be set via external toolchain file

# Compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Enable GNU extensions (uses -std=gnu++17 instead of -std=c++17)
# This prevents __STRICT_ANSI__ from being defined (fixing the at_quick_exit issue)
# and enables GNU extensions like 'Q' suffix for high-precision floats (fixing the googletest issue).
set(CMAKE_CXX_EXTENSIONS ON)

# Compiler flags will be set via toolchain file

# Static linking settings will be set via toolchain file

# Windows-specific settings
if(WIN32)
    add_definitions(-D_MINGW -DWIN32 -D_WIN32)
endif()

# SIMD optimization flags will be set via toolchain file

# Dependencies - Use MinGW's built-in OpenSSL
# OPENSSL_ROOT_DIR should be passed from command line
# set(OPENSSL_ROOT_DIR "C:/ProgramData/mingw64/mingw64/opt")
set(OPENSSL_USE_STATIC_LIBS TRUE)

# Find zlib manually for OpenSSL
find_path(ZLIB_INCLUDE_DIR zlib.h PATHS C:/ProgramData/mingw64/mingw64/include C:/msys64/mingw64/include)
find_library(ZLIB_LIBRARY libz.a PATHS C:/ProgramData/mingw64/mingw64/lib C:/msys64/mingw64/lib)

if(ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
    set(ZLIB_FOUND TRUE)
    set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
else()
    set(ZLIB_FOUND FALSE)
endif()

# Find OpenSSL - should work with MinGW's built-in version
find_package(OpenSSL REQUIRED)

# Find WebP manually - prefer static libraries
find_path(WEBP_INCLUDE_DIR webp/types.h PATHS C:/ProgramData/mingw64/mingw64/include C:/msys64/mingw64/include)
find_library(WEBP_LIBRARY libwebp.a PATHS C:/ProgramData/mingw64/mingw64/lib C:/msys64/mingw64/lib)
find_library(WEBPDEMUX_LIBRARY libwebpdemux.a PATHS C:/ProgramData/mingw64/mingw64/lib C:/msys64/mingw64/lib)
find_library(WEBPMUX_LIBRARY libwebpmux.a PATHS C:/ProgramData/mingw64/mingw64/lib C:/msys64/mingw64/lib)
find_library(SHARPYUV_LIBRARY libsharpyuv.a PATHS C:/ProgramData/mingw64/mingw64/lib C:/msys64/mingw64/lib)

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
    # CMAKE_C_COMPILER and CMAKE_CXX_COMPILER are passed from the main project
    # set(CMAKE_C_COMPILER "C:/ProgramData/mingw64/mingw64/bin/gcc.exe" CACHE FILEPATH "C compiler" FORCE)
    # set(CMAKE_CXX_COMPILER "C:/ProgramData/mingw64/mingw64/bin/g++.exe" CACHE FILEPATH "C++ compiler" FORCE)

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
)

target_link_libraries(clwe PRIVATE OpenSSL::Crypto)
if(ZLIB_FOUND)
    target_link_libraries(clwe PRIVATE ${ZLIB_LIBRARIES})
endif()

# Windows Cryptography API for secure random
if(WIN32)
    target_link_libraries(clwe PRIVATE crypt32 bcrypt)
endif()

# KEM demonstration - commented out as demo_kem.cpp is missing
# add_executable(demo_kem demo_kem.cpp)
# target_link_libraries(demo_kem PRIVATE clwe_macos)

# Color KEM timing benchmark
add_executable(benchmark_color_kem_timing benchmark_color_kem_timing.cpp)
target_link_libraries(benchmark_color_kem_timing PRIVATE clwe)


if(WEBP_FOUND)
    # Key image generator executable
    add_executable(generate_key_images generate_key_images.cpp)
    target_link_libraries(generate_key_images PRIVATE clwe ${WEBP_LIBRARIES})

    # Key image test executable
    add_executable(test_key_images test_key_images.cpp)
    target_link_libraries(test_key_images PRIVATE clwe ${WEBP_LIBRARIES})

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
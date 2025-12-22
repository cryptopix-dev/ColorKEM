cmake_minimum_required(VERSION 3.16)

project(ColorKEM
    VERSION 1.0.0
    LANGUAGES C CXX
)

# Include custom CMake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# ============================================================
# Compiler settings
# ============================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable GNU extensions
# - Fixes __STRICT_ANSI__ issues
# - Enables GNU-specific features needed by gtest
set(CMAKE_CXX_EXTENSIONS ON)

# ============================================================
# Windows-specific definitions
# ============================================================

if(WIN32)
    add_definitions(-D_MINGW -DWIN32 -D_WIN32)
endif()

# ============================================================
# OpenSSL (static, MinGW)
# ============================================================

set(OPENSSL_USE_STATIC_LIBS TRUE)

# Auto-detect OpenSSL root from compiler path (MinGW safety)
if(WIN32 AND MINGW AND NOT DEFINED OPENSSL_ROOT_DIR)

    if(NOT IS_ABSOLUTE "${CMAKE_C_COMPILER}")
        find_program(FULL_C_COMPILER_PATH ${CMAKE_C_COMPILER})
    else()
        set(FULL_C_COMPILER_PATH ${CMAKE_C_COMPILER})
    endif()

    if(FULL_C_COMPILER_PATH)
        get_filename_component(COMPILER_DIR ${FULL_C_COMPILER_PATH} DIRECTORY)
        get_filename_component(MINGW_TOOLCHAIN_ROOT ${COMPILER_DIR} DIRECTORY)

        if(EXISTS "${MINGW_TOOLCHAIN_ROOT}/include/openssl/ssl.h"
           OR EXISTS "${MINGW_TOOLCHAIN_ROOT}/lib/libcrypto.a")
            message(STATUS "Auto-detected OpenSSL Root: ${MINGW_TOOLCHAIN_ROOT}")
            set(OPENSSL_ROOT_DIR ${MINGW_TOOLCHAIN_ROOT})
        endif()
    endif()
endif()

# ============================================================
# Zlib (manual static lookup)
# ============================================================

# Try to find MinGW for additional search paths
find_package(MinGW QUIET)

find_path(ZLIB_INCLUDE_DIR zlib.h
    PATHS
        ${MINGW_INCLUDE_DIR}
        C:/ProgramData/mingw64/mingw64/include
        C:/msys64/mingw64/include
        /usr/include
        /usr/local/include
)

find_library(ZLIB_LIBRARY libz.a z
    PATHS
        ${MINGW_LIB_DIR}
        C:/ProgramData/mingw64/mingw64/lib
        C:/msys64/mingw64/lib
        /usr/lib
        /usr/local/lib
)

if(ZLIB_INCLUDE_DIR AND ZLIB_LIBRARY)
    set(ZLIB_FOUND TRUE)
    set(ZLIB_LIBRARIES ${ZLIB_LIBRARY})
else()
    set(ZLIB_FOUND FALSE)
endif()

# ============================================================
# OpenSSL package
# ============================================================

find_package(OpenSSL REQUIRED)

# ============================================================
# WebP (static)
# ============================================================

find_path(WEBP_INCLUDE_DIR webp/types.h
    PATHS
        ${MINGW_INCLUDE_DIR}
        C:/ProgramData/mingw64/mingw64/include
        C:/msys64/mingw64/include
        /usr/include
        /usr/local/include
)

find_library(WEBP_LIBRARY       libwebp.a webp       PATHS ${MINGW_LIB_DIR} C:/ProgramData/mingw64/mingw64/lib C:/msys64/mingw64/lib /usr/lib /usr/local/lib)
find_library(WEBPDEMUX_LIBRARY  libwebpdemux.a webpdemux  PATHS ${MINGW_LIB_DIR} C:/ProgramData/mingw64/mingw64/lib C:/msys64/mingw64/lib /usr/lib /usr/local/lib)
find_library(WEBPMUX_LIBRARY    libwebpmux.a webpmux    PATHS ${MINGW_LIB_DIR} C:/ProgramData/mingw64/mingw64/lib C:/msys64/mingw64/lib /usr/lib /usr/local/lib)
find_library(SHARPYUV_LIBRARY   libsharpyuv.a sharpyuv   PATHS ${MINGW_LIB_DIR} C:/ProgramData/mingw64/mingw64/lib C:/msys64/mingw64/lib /usr/lib /usr/local/lib)

if(WEBP_INCLUDE_DIR AND WEBP_LIBRARY)
    set(WEBP_FOUND TRUE)
    set(WEBP_LIBRARIES
        ${WEBP_LIBRARY}
        ${WEBPDEMUX_LIBRARY}
        ${WEBPMUX_LIBRARY}
        ${SHARPYUV_LIBRARY}
    )
else()
    set(WEBP_FOUND FALSE)
endif()

# ============================================================
# Include directories
# ============================================================

include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src/core
)

if(WIN32)
    include_directories(${OPENSSL_INCLUDE_DIR})
    if(WEBP_FOUND)
        include_directories(${WEBP_INCLUDE_DIR})
    endif()
endif()

# ============================================================
# GoogleTest
# ============================================================

find_package(GTest QUIET)

if(NOT GTest_FOUND)
    include(FetchContent)

    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    set(CMAKE_POLICY_VERSION_MINIMUM 3.16)

    FetchContent_MakeAvailable(googletest)
endif()

# ============================================================
# Core Library
# ============================================================

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

# ============================================================
# Windows OpenSSL static helper
# ============================================================

if(WIN32)

    target_link_libraries(clwe PRIVATE crypt32 bcrypt ws2_32)

    find_library(MINGWEX_LIBRARY
        NAMES mingwex libmingwex
        PATHS
            ${MINGW_LIB_DIR}
            ${OPENSSL_ROOT_DIR}/lib
            ${OPENSSL_ROOT_DIR}/../lib
            C:/msys64/mingw64/lib
            C:/ProgramData/mingw64/mingw64/lib
        NO_DEFAULT_PATH
    )

    if(NOT MINGWEX_LIBRARY)
        find_library(MINGWEX_LIBRARY NAMES mingwex libmingwex)
    endif()

    function(add_windows_openssl_libs target)
        target_link_options(${target} PRIVATE -Wl,--allow-multiple-definition)
        target_link_libraries(${target} PRIVATE OpenSSL::Crypto OpenSSL::SSL)

        if(MINGWEX_LIBRARY)
            target_link_libraries(${target} PRIVATE ${MINGWEX_LIBRARY})
        else()
            target_link_libraries(${target} PRIVATE mingwex)
        endif()

        target_link_libraries(${target} PRIVATE crypt32 bcrypt ws2_32 gdi32)

        if(ZLIB_FOUND)
            target_link_libraries(${target} PRIVATE ${ZLIB_LIBRARIES})
        endif()
    endfunction()

endif()

# ============================================================
# Benchmarks
# ============================================================

add_executable(benchmark_color_kem_timing benchmark_color_kem_timing.cpp)
target_link_libraries(benchmark_color_kem_timing PRIVATE clwe)

if(WIN32)
    add_windows_openssl_libs(benchmark_color_kem_timing)
endif()

# ============================================================
# WebP Tools
# ============================================================

if(WEBP_FOUND)

    add_executable(generate_key_images generate_key_images.cpp)
    target_link_libraries(generate_key_images PRIVATE clwe ${WEBP_LIBRARIES})

    add_executable(test_key_images test_key_images.cpp)
    target_link_libraries(test_key_images PRIVATE clwe ${WEBP_LIBRARIES})

    if(WIN32)
        add_windows_openssl_libs(generate_key_images)
        add_windows_openssl_libs(test_key_images)
    endif()

    add_test(NAME TestKeyImages COMMAND test_key_images)

endif()

# ============================================================
# Testing
# ============================================================

enable_testing()
add_subdirectory(tests)

add_test(NAME BenchmarkTest COMMAND benchmark_color_kem_timing)

add_custom_target(run_all_tests
    COMMAND ${CMAKE_COMMAND} -E echo "Running ColorKEM tests..."
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

# ============================================================
# Installation
# ============================================================

install(TARGETS clwe
    EXPORT ColorKEMTargets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    INCLUDES DESTINATION include
)

install(DIRECTORY src/include/clwe/
    DESTINATION include/clwe
    FILES_MATCHING PATTERN "*.hpp"
)

install(EXPORT ColorKEMTargets
    FILE ColorKEMTargets.cmake
    NAMESPACE ColorKEM::
    DESTINATION lib/cmake/ColorKEM
)

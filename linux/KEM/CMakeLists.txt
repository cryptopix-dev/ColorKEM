cmake_minimum_required(VERSION 3.16)
project(clwe_linux VERSION 1.0.0 LANGUAGES C CXX)

# Compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

# Multi-architecture SIMD detection and configuration
include(CheckCXXCompilerFlag)
include(CheckCXXSourceCompiles)

# Architecture-specific SIMD detection
if(CMAKE_SYSTEM_PROCESSOR MATCHES "(x86)|(X86)|(amd64)|(AMD64)|(x86_64)|(X86_64)")
    # x86/x64 architecture - check for AVX support
    check_cxx_compiler_flag("-mavx2" AVX2_SUPPORTED)
    check_cxx_compiler_flag("-mfma" FMA_SUPPORTED)

    if(AVX2_SUPPORTED)
        add_compile_definitions(HAVE_AVX2)
        add_definitions(-DHAVE_AVX2)
        set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mavx2")
        if(FMA_SUPPORTED)
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mfma")
        endif()
        message(STATUS "x86_64: AVX2 support detected and enabled")

        # AVX-512 detection with comprehensive instruction set
        check_cxx_compiler_flag("-mavx512f" AVX512F_SUPPORTED)
        check_cxx_compiler_flag("-mavx512dq" AVX512DQ_SUPPORTED)
        check_cxx_compiler_flag("-mavx512bw" AVX512BW_SUPPORTED)
        check_cxx_compiler_flag("-mavx512vl" AVX512VL_SUPPORTED)

        if(AVX512F_SUPPORTED)
            add_compile_definitions(HAVE_AVX512)
            set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mavx512f")
            message(STATUS "x86_64: AVX-512F support detected and enabled")

            if(AVX512DQ_SUPPORTED)
                set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mavx512dq")
                add_compile_definitions(HAVE_AVX512DQ)
                message(STATUS "x86_64: AVX-512DQ support detected and enabled")
            endif()

            if(AVX512BW_SUPPORTED)
                set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mavx512bw")
                add_compile_definitions(HAVE_AVX512BW)
                message(STATUS "x86_64: AVX-512BW support detected and enabled")
            endif()

            if(AVX512VL_SUPPORTED)
                set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -mavx512vl")
                add_compile_definitions(HAVE_AVX512VL)
                message(STATUS "x86_64: AVX-512VL support detected and enabled")
            endif()
        else()
            message(STATUS "x86_64: AVX-512 not supported, using AVX2")
        endif()
    else()
        message(WARNING "x86_64: AVX2 not supported, using scalar fallback")
        add_compile_definitions(NO_AVX_SUPPORT)
    endif()

elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "(aarch64)|(arm64)|(ARM64)")
    # ARM64 architecture - check for NEON support
    check_cxx_compiler_flag("-march=armv8-a+simd" NEON_SUPPORTED)
    if(NEON_SUPPORTED)
        add_compile_definitions(HAVE_NEON)
        message(STATUS "ARM64: NEON SIMD support detected and enabled")
    else()
        message(WARNING "ARM64: NEON not supported, using scalar fallback")
        add_compile_definitions(NO_SIMD_SUPPORT)
    endif()

elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "(riscv64)|(riscv)")
    # RISC-V architecture - check for vector extensions
    check_cxx_compiler_flag("-march=rv64gcv" RVV_SUPPORTED)
    if(RVV_SUPPORTED)
        add_compile_definitions(HAVE_RVV)
        message(STATUS "RISC-V: Vector extensions detected and enabled")
    else()
        message(WARNING "RISC-V: Vector extensions not supported, using scalar fallback")
        add_compile_definitions(NO_SIMD_SUPPORT)
    endif()

elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "(ppc64)|(powerpc64)")
    # PowerPC architecture - check for VSX/AltiVec
    check_cxx_compiler_flag("-mvsx" VSX_SUPPORTED)
    check_cxx_compiler_flag("-maltivec" ALTIVEC_SUPPORTED)
    if(VSX_SUPPORTED OR ALTIVEC_SUPPORTED)
        add_compile_definitions(HAVE_VSX)
        if(VSX_SUPPORTED)
            message(STATUS "PowerPC: VSX support detected and enabled")
        endif()
        if(ALTIVEC_SUPPORTED)
            add_compile_definitions(HAVE_ALTIVEC)
            message(STATUS "PowerPC: AltiVec support detected and enabled")
        endif()
    else()
        message(WARNING "PowerPC: VSX/AltiVec not supported, using scalar fallback")
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

# Dependencies
find_package(OpenSSL REQUIRED)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/core)

# Library target
set(BASE_SOURCES
    src/core/parameters.cpp
    src/core/ntt_engine.cpp
    src/core/ntt_scalar.cpp
    src/core/color_value.cpp
    src/core/color_ntt_engine.cpp
    src/core/color_kem.cpp
    src/core/cpu_features.cpp
    src/core/shake_sampler.cpp
    src/core/sampling.cpp
    src/core/utils.cpp
    src/core/performance_metrics.cpp
    src/core/performance_metrics_linux.cpp
)

if(AVX2_SUPPORTED)
    list(APPEND BASE_SOURCES src/core/polynomial.cpp src/core/ntt_avx.cpp src/core/ring_operations.cpp)
endif()

if(NEON_SUPPORTED)
    list(APPEND BASE_SOURCES src/core/ntt_neon.cpp)
endif()

if(RVV_SUPPORTED)
    list(APPEND BASE_SOURCES src/core/ntt_rvv.cpp)
endif()

if(VSX_SUPPORTED)
    list(APPEND BASE_SOURCES src/core/ntt_vsx.cpp)
endif()

add_library(clwe_linux STATIC ${BASE_SOURCES})

if(NEON_SUPPORTED)
    target_compile_options(clwe_linux PRIVATE -march=armv8-a+simd)
endif()

if(RVV_SUPPORTED)
    target_compile_options(clwe_linux PRIVATE -march=rv64gcv)
endif()

if(VSX_SUPPORTED)
    target_compile_options(clwe_linux PRIVATE -mvsx)
endif()

if(ALTIVEC_SUPPORTED)
    target_compile_options(clwe_linux PRIVATE -maltivec)
endif()

target_link_libraries(clwe_linux PRIVATE OpenSSL::Crypto)

# KEM demonstration - commented out as demo_kem.cpp is missing
# add_executable(demo_kem demo_kem.cpp)
# if(NEON_SUPPORTED)
#     target_compile_options(demo_kem PRIVATE -march=armv8-a+simd)
# endif()
#
# if(RVV_SUPPORTED)
#     target_compile_options(demo_kem PRIVATE -march=rv64gcv)
# endif()
#
# if(VSX_SUPPORTED)
#     target_compile_options(demo_kem PRIVATE -mvsx)
# endif()
#
# if(ALTIVEC_SUPPORTED)
#     target_compile_options(demo_kem PRIVATE -maltivec)
# endif()
#
# target_link_libraries(demo_kem PRIVATE clwe_linux)

# Color KEM timing benchmark
add_executable(benchmark_color_kem_timing benchmark_color_kem_timing.cpp)
if(NEON_SUPPORTED)
    target_compile_options(benchmark_color_kem_timing PRIVATE -march=armv8-a+simd)
endif()

if(RVV_SUPPORTED)
    target_compile_options(benchmark_color_kem_timing PRIVATE -march=rv64gcv)
endif()

if(VSX_SUPPORTED)
    target_compile_options(benchmark_color_kem_timing PRIVATE -mvsx)
endif()

if(ALTIVEC_SUPPORTED)
    target_compile_options(benchmark_color_kem_timing PRIVATE -maltivec)
endif()

target_link_libraries(benchmark_color_kem_timing PRIVATE clwe_linux)

# Demo with timing - commented out as demo_with_timing.cpp is missing
# add_executable(demo_with_timing demo_with_timing.cpp)
# if(NEON_SUPPORTED)
#     target_compile_options(demo_with_timing PRIVATE -march=armv8-a+simd)
# endif()
#
# if(RVV_SUPPORTED)
#     target_compile_options(demo_with_timing PRIVATE -march=rv64gcv)
# endif()
#
# if(VSX_SUPPORTED)
#     target_compile_options(demo_with_timing PRIVATE -mvsx)
# endif()
#
# if(ALTIVEC_SUPPORTED)
#     target_compile_options(demo_with_timing PRIVATE -maltivec)
# endif()
#
# target_link_libraries(demo_with_timing PRIVATE clwe_linux)

# Main executable
# add_executable(clwe_main src/main.cpp)
# target_link_libraries(clwe_main PRIVATE clwe_avx)

# Tests
enable_testing()
add_subdirectory(tests)

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


# Python bindings (optional)
option(BUILD_PYTHON_BINDINGS "Build Python bindings" OFF)
if(BUILD_PYTHON_BINDINGS)
    add_subdirectory(python)
endif()

# Installation
install(TARGETS clwe_linux
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
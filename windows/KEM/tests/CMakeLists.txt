# Tests CMakeLists.txt

# Include directories for tests
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/include/clwe)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src/core)

# Test executables
add_executable(test_color_value test_color_value.cpp)
target_link_libraries(test_color_value PRIVATE clwe gtest_main)

add_executable(test_clwe_parameters test_clwe_parameters.cpp)
target_link_libraries(test_clwe_parameters PRIVATE clwe gtest_main)

add_executable(test_color_kem test_color_kem.cpp)
target_link_libraries(test_color_kem PRIVATE clwe gtest_main)

add_executable(test_serialization test_serialization.cpp)
target_link_libraries(test_serialization PRIVATE clwe gtest_main)

add_executable(test_utils test_utils.cpp)
target_link_libraries(test_utils PRIVATE clwe gtest_main)

add_executable(test_ntt_engine test_ntt_engine.cpp)
target_link_libraries(test_ntt_engine PRIVATE clwe gtest_main)

add_executable(test_sampling test_sampling.cpp)
target_link_libraries(test_sampling PRIVATE clwe gtest_main)

add_executable(test_integration_kem test_integration_kem.cpp)
target_link_libraries(test_integration_kem PRIVATE clwe gtest_main)

add_executable(test_known_answer_tests test_known_answer_tests.cpp)
target_link_libraries(test_known_answer_tests PRIVATE clwe gtest_main)

add_executable(test_performance_metrics test_performance_metrics.cpp)
target_link_libraries(test_performance_metrics PRIVATE clwe gtest_main)

# Windows-specific libraries for OpenSSL static linking
if(WIN32)
    set(TEST_EXECUTABLES 
        test_color_value 
        test_clwe_parameters 
        test_color_kem 
        test_serialization 
        test_utils 
        test_ntt_engine 
        test_sampling 
        test_integration_kem 
        test_known_answer_tests 
        test_performance_metrics
    )
    
    foreach(test_exe ${TEST_EXECUTABLES})
        target_link_libraries(${test_exe} PRIVATE OpenSSL::Crypto OpenSSL::SSL mingwex crypt32 bcrypt ws2_32 gdi32)
    endforeach()
endif()

# Fuzzing targets (only if fuzzing is enabled)
if(ENABLE_FUZZING)
    add_executable(fuzz_input_validation fuzz/fuzz_input_validation.cpp)
    target_link_libraries(fuzz_input_validation PRIVATE clwe)

    add_executable(fuzz_serialization fuzz/fuzz_serialization.cpp)
    target_link_libraries(fuzz_serialization PRIVATE clwe)

    add_executable(fuzz_kem_operations fuzz/fuzz_kem_operations.cpp)
    target_link_libraries(fuzz_kem_operations PRIVATE clwe)

    add_executable(fuzz_edge_cases fuzz/fuzz_edge_cases.cpp)
    target_link_libraries(fuzz_edge_cases PRIVATE clwe)
    
    # Windows-specific libraries for fuzzing targets
    if(WIN32)
        set(FUZZ_EXECUTABLES 
            fuzz_input_validation 
            fuzz_serialization 
            fuzz_kem_operations 
            fuzz_edge_cases
        )
        
        foreach(fuzz_exe ${FUZZ_EXECUTABLES})
            target_link_libraries(${fuzz_exe} PRIVATE OpenSSL::Crypto OpenSSL::SSL mingwex crypt32 bcrypt ws2_32 gdi32)
        endforeach()
    endif()
endif()

# Register tests
add_test(NAME ColorValueTests COMMAND test_color_value)
add_test(NAME CLWEParametersTests COMMAND test_clwe_parameters)
add_test(NAME ColorKEMTests COMMAND test_color_kem)
add_test(NAME SerializationTests COMMAND test_serialization)
add_test(NAME UtilsTests COMMAND test_utils)
add_test(NAME NTTEngineTests COMMAND test_ntt_engine)
add_test(NAME SamplingTests COMMAND test_sampling)
add_test(NAME IntegrationKEMTests COMMAND test_integration_kem)
add_test(NAME KnownAnswerTests COMMAND test_known_answer_tests)
add_test(NAME PerformanceMetricsTests COMMAND test_performance_metrics)
idf_component_register(SRCS "benchmark_color_kem_timing.cpp"
                            "../src/core/color_integration.cpp"
                            "../src/core/color_kem.cpp"
                            "../src/core/color_ntt_engine.cpp"
                            "../src/core/color_value.cpp"
                            "../src/core/cpu_features.cpp"
                            "../src/core/ntt_engine.cpp"
                            "../src/core/ntt_scalar.cpp"
                            "../src/core/parameters.cpp"
                            "../src/core/performance_metrics.cpp"
                            "../src/core/sampling.cpp"
                            "../src/core/shake_sampler.cpp"
                            "../src/core/tiny_sha3.c"
                            "../src/core/utils.cpp"
                       INCLUDE_DIRS "." "../src/include")

# Enable testing if configured
if(CONFIG_ENABLE_TESTS)
    idf_component_register(
        TEST_SRC
            "../tests/test_clwe_parameters.cpp"
            "../tests/test_color_kem.cpp"
            "../tests/test_color_value.cpp"
            "../tests/test_integration_kem.cpp"
            "../tests/test_known_answer_tests.cpp"
            "../tests/test_ntt_engine.cpp"
            "../tests/test_performance_metrics.cpp"
            "../tests/test_sampling.cpp"
            "../tests/test_serialization.cpp"
            "../tests/test_utils.cpp"
        REQUIRES
            unity
    )
endif()

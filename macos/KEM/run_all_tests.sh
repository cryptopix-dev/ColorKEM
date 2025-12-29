#!/bin/bash

# ColorKEM Test Runner for macOS
# This script runs all available tests

# Test results
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Main script starts here
echo "========================================"
echo "ColorKEM Verification Test"
echo "========================================"
echo ""

if [ ! -d "build" ]; then
    echo "Build directory not found. Building project..."
    ./build_macos.sh
    if [ $? -ne 0 ]; then
        echo "Build failed!"
        exit 1
    fi
fi

echo ""
echo "Running verification test..."
echo "---------------------------"

# Test 1: Parameters
if [ -f "./build/tests/test_clwe_parameters" ]; then
    echo "Running Parameters Test..."
    ./build/tests/test_clwe_parameters
    if [ $? -eq 0 ]; then
        echo "PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
fi

# Test 2: Color Value
if [ -f "./build/tests/test_color_value" ]; then
    echo "Running Color Value Test..."
    ./build/tests/test_color_value
    if [ $? -eq 0 ]; then
        echo "PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
fi

# Test 3: Color KEM
if [ -f "./build/tests/test_color_kem" ]; then
    echo "Running Color KEM Test..."
    ./build/tests/test_color_kem
    if [ $? -eq 0 ]; then
        echo "PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
fi

# Test 4: Serialization
if [ -f "./build/tests/test_serialization" ]; then
    echo "Running Serialization Test..."
    ./build/tests/test_serialization
    if [ $? -eq 0 ]; then
        echo "PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
fi

# Test 5: Utils
if [ -f "./build/tests/test_utils" ]; then
    echo "Running Utils Test..."
    ./build/tests/test_utils
    if [ $? -eq 0 ]; then
        echo "PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
fi

# Test 6: NTT Engine
if [ -f "./build/tests/test_ntt_engine" ]; then
    echo "Running NTT Engine Test..."
    ./build/tests/test_ntt_engine
    if [ $? -eq 0 ]; then
        echo "PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
fi

# Test 7: Sampling
if [ -f "./build/tests/test_sampling" ]; then
    echo "Running Sampling Test..."
    ./build/tests/test_sampling
    if [ $? -eq 0 ]; then
        echo "PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
fi

# Test 8: Integration KEM
if [ -f "./build/tests/test_integration_kem" ]; then
    echo "Running Integration KEM Test..."
    ./build/tests/test_integration_kem
    if [ $? -eq 0 ]; then
        echo "PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
fi

# Test 9: Known Answer Tests
if [ -f "./build/tests/test_known_answer_tests" ]; then
    echo "Running KAT Test..."
    ./build/tests/test_known_answer_tests
    if [ $? -eq 0 ]; then
        echo "PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
fi

# Test 10: Performance Metrics
if [ -f "./build/tests/test_performance_metrics" ]; then
    echo "Running Performance Metrics Test..."
    ./build/tests/test_performance_metrics
    if [ $? -eq 0 ]; then
        echo "PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
fi

# Test 11: Key Images (if WebP available)
if [ -f "./build/test_key_images" ]; then
    echo "Running Key Images Test..."
    ./build/test_key_images
    if [ $? -eq 0 ]; then
        echo "PASSED"
        PASSED_TESTS=$((PASSED_TESTS + 1))
    else
        echo "FAILED"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
fi

echo ""
echo "========================================"
echo "Test Summary"
echo "========================================"
echo "Total Tests: $TOTAL_TESTS"
echo "Passed: $PASSED_TESTS"
echo "Failed: $FAILED_TESTS"

if [ $FAILED_TESTS -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed!"
    exit 1
fi
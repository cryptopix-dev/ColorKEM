#!/bin/bash

echo "========================================"
echo "ColorKEM Comprehensive Test Suite"
echo "========================================"
echo ""
echo ""

TOTAL_TESTS=1
PASSED_TESTS=0
FAILED_TESTS=0

if [ ! -d "build" ]; then
    echo "Build directory not found. Building project..."
    ./build_macos.sh
    if [ $? -ne 0 ]; then
        echo "Build failed!"
        exit 1
    fi
fi

echo ""
echo "Running test suites..."
echo "----------------------"

echo "Running ColorKEM comprehensive test..."
./build/tests/test_color_kem
if [ $? -eq 0 ]; then
    echo "PASSED"
    PASSED_TESTS=$((PASSED_TESTS + 1))
else
    echo "FAILED"
    FAILED_TESTS=$((FAILED_TESTS + 1))
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
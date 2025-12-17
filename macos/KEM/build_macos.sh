#!/bin/bash

# build_macos.sh - Automated build script for macOS cryptopix-clwe
# Run from macos/ directory

set -e  # Exit on any error

echo "Starting automated build for cryptopix-clwe on macOS..."

# Architecture detection
if [[ "$(uname -m)" == "x86_64" ]]; then
    echo "Detected Intel architecture (x86_64)"
    ARCH="intel"
elif [[ "$(uname -m)" == "arm64" ]]; then
    echo "Detected Apple Silicon architecture (arm64)"
    ARCH="apple_silicon"
else
    echo "Unknown architecture: $(uname -m)"
    ARCH="unknown"
fi

# Create build directory
mkdir -p build
cd build

# Configure with CMake
echo "Configuring build with CMake..."
if ! cmake ..; then
    echo "CMake configuration failed."
    exit 1
fi

# Build the project
echo "Building the project..."
if ! make -j$(sysctl -n hw.ncpu); then
    echo "Build failed."
    exit 1
fi

cd ..

echo "Build completed successfully!"
echo "Library: build/libclwe_macos.a"
echo "Demo executable: build/demo_kem"
echo "Benchmark executable: build/benchmark_color_kem_timing"
echo "Architecture: $ARCH"

exit 0
#!/bin/bash

# build_linux.sh - Automated build script for cryptopix-clwe Linux standalone project
# Run from linux/ directory

set -e  # Exit on any error

echo "Starting automated build for cryptopix-clwe on Linux..."

# Function to check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Detect Linux distribution
if [ -f /etc/os-release ]; then
    . /etc/os-release
    DISTRO=$ID
    DISTRO_LIKE=$ID_LIKE
else
    echo "Cannot detect Linux distribution. Exiting."
    exit 1
fi

echo "Detected distribution: $DISTRO"

# Function to install packages based on distro
install_packages() {
    case $DISTRO in
        ubuntu|debian|pop|elementary|linuxmint)
            echo "Installing packages for Ubuntu/Debian-based system..."
            sudo apt update
            sudo apt install -y cmake build-essential libssl-dev git
            ;;
        centos|rhel|fedora)
            echo "Installing packages for CentOS/RHEL/Fedora-based system..."
            if command_exists dnf; then
                sudo dnf install -y cmake gcc-c++ openssl-devel git
            elif command_exists yum; then
                sudo yum install -y cmake gcc-c++ openssl-devel git
            else
                echo "Neither dnf nor yum found. Please install dependencies manually."
                exit 1
            fi
            ;;
        arch|manjaro)
            echo "Installing packages for Arch-based system..."
            sudo pacman -Syu --noconfirm cmake gcc openssl git
            ;;
        opensuse*|sles)
            echo "Installing packages for openSUSE-based system..."
            sudo zypper install -y cmake gcc-c++ libopenssl-devel git
            ;;
        *)
            echo "Unsupported distribution: $DISTRO. Please install dependencies manually: cmake, build-essential/gcc-c++, libssl-dev/openssl-devel, git"
            exit 1
            ;;
    esac
}

# Check and install dependencies
MISSING_DEPS=()
if ! command_exists cmake; then
    MISSING_DEPS+=("cmake")
fi
if ! command_exists gcc || ! command_exists g++; then
    MISSING_DEPS+=("gcc/g++")
fi
if ! command_exists git; then
    MISSING_DEPS+=("git")
fi
# OpenSSL check is trickier, assume it's installed if libssl-dev is there, but we'll install anyway if missing

if [ ${#MISSING_DEPS[@]} -gt 0 ]; then
    echo "Missing dependencies: ${MISSING_DEPS[*]}"
    install_packages
else
    echo "All basic dependencies appear to be installed."
fi

# Ensure OpenSSL dev headers are available
if ! pkg-config --exists openssl 2>/dev/null && ! [ -f /usr/include/openssl/ssl.h ]; then
    echo "OpenSSL development headers not found. Installing..."
    install_packages
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
if ! make -j$(nproc); then
    echo "Build failed."
    exit 1
fi

cd ..

echo "Build completed successfully!"
echo "Library: build/libclwe_linux.a"
echo "Demo executable: build/demo_kem"
echo "Benchmark executable: build/benchmark_color_kem_timing"
echo "Demo with timing: build/demo_with_timing"

exit 0
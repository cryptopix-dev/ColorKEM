# ColorKEM for Linux

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-blue.svg)](https://cmake.org/)

**Platform**: Linux (Ubuntu, Debian, CentOS, Fedora, Arch, openSUSE)
**Version**: 1.0.0
**Authors**: CRYPTOPIX (OPC) PVT LTD Development team

This is the Linux-specific implementation of ColorKEM, optimized for Linux distributions with support for various architectures and SIMD instructions.

## 🚀 Build Instructions

### Prerequisites

- **C++ Compiler**: GCC 9+, Clang 10+
- **CMake**: Version 3.16 or higher
- **OpenSSL**: Version 1.1.1 or higher (development headers)
- **Git**: For cloning dependencies

### Automated Build

The easiest way to build ColorKEM on Linux is using the provided build script:

```bash
cd ColorKEM/linux/KEM
chmod +x build_linux.sh
./build_linux.sh
```

This script will:
- Detect your Linux distribution
- Install required dependencies automatically
- Create a `build/` directory
- Configure the project with CMake
- Build the library and executables

### Manual Build

If you prefer to build manually:

```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install cmake g++ libssl-dev build-essential git

# For Fedora/RHEL/CentOS
sudo dnf install cmake gcc-c++ openssl-devel git

# Build
mkdir build
cd build
cmake ..
make -j$(nproc)
```

### Build Outputs

After successful build:
- **Library**: `build/libclwe_linux.a`
- **Demo executable**: `build/demo_kem`
- **Benchmark executable**: `build/benchmark_color_kem_timing`
- **Test executables**: Various test binaries

## 💡 Usage

### C++ Integration

```cpp
#include "clwe/color_kem.hpp"

int main() {
    // Initialize parameters
    clwe::CLWEParameters params(512);

    // Create KEM instance
    clwe::ColorKEM kem(params);

    // Generate keypair
    auto [public_key, private_key] = kem.keygen();

    // Encapsulate shared secret
    auto [ciphertext, shared_secret] = kem.encapsulate(public_key);

    // Decapsulate shared secret
    auto recovered_secret = kem.decapsulate(private_key, ciphertext);

    return (shared_secret == recovered_secret) ? 0 : 1;
}
```

### Linking

When compiling your application, link against the static library:

```bash
g++ -std=c++17 your_app.cpp -Lbuild -lclwe_linux -lssl -lcrypto -pthread -o your_app
```

## 🏗️ Platform-Specific Details

### Supported Distributions

- **Ubuntu/Debian-based**: Ubuntu 18.04+, Debian 10+, Pop!_OS, Linux Mint
- **Red Hat-based**: CentOS 7+, RHEL 7+, Fedora 30+
- **Arch-based**: Arch Linux, Manjaro
- **openSUSE-based**: openSUSE Leap 15+, SUSE Linux Enterprise

### Architecture Support

- **x86-64**: Full support with AVX2/AVX512 SIMD optimizations
- **ARM64**: NEON optimizations (when available)
- **ARMv7**: NEON optimizations (when available)

### SIMD Optimizations

The Linux build automatically detects and enables SIMD instructions:
- **AVX2**: For Intel/AMD processors with AVX2 support
- **AVX512**: For latest Intel processors (F/VL/BW/DQ subsets)
- **NEON**: For ARM processors

### Performance Notes

- Uses `getauxval()` or CPUID for feature detection
- Fallback to scalar implementations when SIMD is unavailable
- Memory alignment optimized for cache performance

### Security Features

- Constant-time operations to prevent timing attacks
- Secure memory wiping of sensitive data
- Cryptographically secure random number generation via OpenSSL

## 🧪 Testing

Run the test suite:

```bash
cd build
ctest --output-on-failure
```

Or use the convenience script (if available):

```bash
cd ColorKEM/linux/KEM
./run_all_tests.sh
```

## 📞 Support

For Linux-specific issues:
- Check the main [ColorKEM repository](https://github.com/cryptopix-dev/ColorKEM)
- Report issues on [GitHub Issues](https://github.com/cryptopix-dev/ColorKEM/issues)
- Email: support@cryptopix.in

---

**ColorKEM Linux**: High-performance post-quantum cryptography for Linux systems.
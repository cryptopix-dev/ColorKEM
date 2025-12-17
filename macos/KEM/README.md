# ColorKEM for macOS

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-blue.svg)](https://cmake.org/)

**Platform**: macOS (10.15+)
**Version**: 1.0.0
**Authors**: CRYPTOPIX (OPC) PVT LTD Development team

This is the macOS-specific implementation of ColorKEM, optimized for both Intel and Apple Silicon Macs with native performance optimizations.

## 🚀 Build Instructions

### Prerequisites

- **macOS**: Version 10.15 (Catalina) or higher
- **Xcode**: Command Line Tools (install with `xcode-select --install`)
- **CMake**: Version 3.16 or higher
- **OpenSSL**: Via Homebrew
- **Homebrew**: For dependency management

### Automated Build

Use the provided build script for macOS:

```bash
cd ColorKEM/macos/KEM
chmod +x build_macos.sh
./build_macos.sh
```

This script will:
- Detect your Mac architecture (Intel or Apple Silicon)
- Create a `build/` directory
- Configure the project with CMake
- Build the library and executables using available CPU cores

### Manual Build

If you prefer manual installation:

```bash
# Install dependencies via Homebrew
brew install cmake openssl

# Build
mkdir build
cd build
cmake ..
make -j$(sysctl -n hw.ncpu)
```

### Build Outputs

After successful build:
- **Library**: `build/libclwe_macos.a`
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

When compiling your application:

```bash
clang++ -std=c++17 your_app.cpp -Lbuild -lclwe_macos -lssl -lcrypto -framework Security -o your_app
```

## 🏗️ Platform-Specific Details

### Supported macOS Versions

- **macOS Catalina (10.15)**: Minimum supported version
- **macOS Big Sur (11.0+)**: Full support
- **macOS Monterey (12.0+)**: Full support
- **macOS Ventura (13.0+)**: Full support

### Architecture Support

- **Intel (x86-64)**: AVX2/AVX512 optimizations
- **Apple Silicon (ARM64/M1/M2/M3)**: NEON optimizations with Rosetta 2 compatibility

### SIMD Optimizations

- **Intel Macs**: AVX2/AVX512F for vectorized operations
- **Apple Silicon**: NEON instructions for ARM64 performance
- **Automatic Detection**: Uses sysctl and CPU feature detection

### Performance Features

- **Multi-core Building**: Uses `sysctl -n hw.ncpu` for optimal parallel compilation
- **Memory Management**: Optimized for macOS memory subsystem
- **Security Framework**: Integration with macOS Security framework for entropy

### Security Considerations

- **Constant-Time Operations**: Timing attack resistance
- **Secure Memory**: Uses macOS secure memory allocation when available
- **Random Number Generation**: Leverages OpenSSL with macOS entropy sources

## 🧪 Testing

Execute the test suite:

```bash
cd build
ctest --output-on-failure
```

Or use the convenience script:

```bash
cd ColorKEM/macos/KEM
./run_all_tests.sh
```

## 📞 Support

For macOS-specific issues:
- Check the main [ColorKEM repository](https://github.com/cryptopix-dev/ColorKEM)
- Report issues on [GitHub Issues](https://github.com/cryptopix-dev/ColorKEM/issues)
- Email: support@cryptopix.in

---

**ColorKEM macOS**: Native post-quantum cryptography for macOS systems.
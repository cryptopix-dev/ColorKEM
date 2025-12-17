# ColorKEM: Post-Quantum Key Encapsulation Mechanism Library

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-blue.svg)](https://cmake.org/)
[![Version](https://img.shields.io/badge/Version-1.0.0-green.svg)](https://github.com/cryptopix-dev/ColorKEM)

**Project Name**: ColorKEM
**Version**: 1.0.0
**Authors**: CRYPTOPIX (OPC) PVT LTD Development team
**Email**: support@cryptopix.in
**Copyright**: ©2025 CRYPTOPIX (OPC) PVT LTD
**GitHub Repository**: https://github.com/cryptopix-dev/ColorKEM

ColorKEM is a high-performance, production-ready implementation of a lattice-based Key Encapsulation Mechanism (KEM) using CLWE (Color Lattice Weil Encoding) for quantum-resistant cryptography. This library provides secure key establishment with innovative color-based polynomial encoding for enhanced security and efficiency.

## 🚀 Key Features

- **Lattice-Based Security**: CLWE-based KEM resistant to quantum attacks
- **Multiple Security Levels**: Configurable parameters for different security requirements
- **Color Integration**: Novel polynomial encoding using color representations for improved security
- **High Performance**: SIMD optimizations (AVX2, AVX512, NEON) for accelerated computations
- **Enterprise Ready**: Comprehensive key management and security features
- **Cross-Platform**: Linux, macOS, and Windows support
- **Python Bindings**: Easy integration with Python applications
- **Comprehensive Testing**: Full test suite with Known Answer Tests (KAT) vectors

## 📋 Table of Contents

- [Installation](#installation)
- [Quick Start](#quick-start)
- [Project Structure](#project-structure)
- [Usage Examples](#usage-examples)
- [Testing](#testing)
- [Build Requirements](#build-requirements)
- [Contributing](#contributing)
- [License](#license)
- [Contact](#contact)

## 🔧 Installation

### Prerequisites

- **C++ Compiler**: GCC 9+, Clang 10+, or MSVC 2019+
- **CMake**: Version 3.16 or higher
- **OpenSSL**: Version 1.1.1 or higher
- **Git**: For cloning dependencies

### Platform-Specific Installation

#### Linux
```bash
# Install dependencies (Ubuntu/Debian)
sudo apt-get update
sudo apt-get install cmake g++ libssl-dev build-essential

# For Fedora/RHEL/CentOS
sudo dnf install cmake gcc-c++ openssl-devel

# Clone and build
git clone https://github.com/cryptopix-dev/ColorKEM.git
cd ColorKEM
./linux/build_linux.sh
```

#### macOS
```bash
# Install dependencies
brew install cmake openssl

# Clone and build
git clone https://github.com/cryptopix-dev/ColorKEM.git
cd ColorKEM
./macos/build_macos.sh
```

#### Windows
```bash
# Install dependencies (using vcpkg)
git clone https://github.com/cryptopix-dev/ColorKEM.git
cd ColorKEM
./windows/build_windows.bat
```

## 📁 Project Structure

```
ColorKEM/
├── linux/                  # Linux-specific implementation
│   ├── build/              # Build artifacts (excluded from Git)
│   ├── src/                # Source code
│   ├── include/            # Header files
│   ├── tests/              # Test suite
│   ├── python/             # Python bindings
│   ├── build_linux.sh      # Linux build script
│   └── README.md           # Linux-specific documentation
├── macos/                  # macOS-specific implementation
│   └── ...                 # Similar structure
├── windows/                # Windows-specific implementation
│   └── ...                 # Similar structure
└── README.md               # This file
```

## 🚀 Quick Start

```cpp
#include "clwe/parameters.hpp"
#include "clwe/color_kem.hpp"

int main() {
    // Initialize parameters
    clwe::CLWEParameters params(512);  // Example security level

    // Create KEM instance
    clwe::ColorKEM kem(params);

    // Generate keypair
    auto [public_key, private_key] = kem.keygen();

    // Encapsulate shared secret
    auto [ciphertext, shared_secret] = kem.encapsulate(public_key);

    // Decapsulate shared secret
    auto recovered_secret = kem.decapsulate(private_key, ciphertext);

    // Verify shared secrets match
    return (shared_secret == recovered_secret) ? 0 : 1;
}
```

## 💡 Usage Examples

### Basic Key Generation and Encapsulation

```cpp
#include "clwe/color_kem.hpp"

int main() {
    // Initialize with custom parameters
    clwe::CLWEParameters params(768);  // Higher security

    // Generate keys
    clwe::ColorKEM kem(params);
    auto [pub_key, priv_key] = kem.keygen();

    // Encapsulate
    auto [ct, ss_enc] = kem.encapsulate(pub_key);

    // Decapsulate
    auto ss_dec = kem.decapsulate(priv_key, ct);

    // Shared secrets should be identical
    assert(ss_enc == ss_dec);

    std::cout << "KEM operation successful" << std::endl;
    return 0;
}
```

### Python Bindings Usage

```python
import pycolorkem

# Create parameters
params = pycolorkem.CLWEParameters(512)

# Create KEM instance
kem = pycolorkem.ColorKEM(params)

# Generate key pair
public_key, private_key = kem.keygen()

# Encapsulate
ciphertext, shared_secret = kem.encapsulate(public_key)

# Decapsulate
recovered_secret = kem.decapsulate(private_key, ciphertext)

# Verify
assert shared_secret == recovered_secret
print("Python KEM test passed")
```

## 🧪 Testing

### Running Tests

```bash
# Linux
cd linux
./run_all_tests.sh

# macOS
cd macos
./run_all_tests.sh

# Windows
cd windows
run_all_tests.bat
```

### Test Coverage

- **100+ individual tests** across multiple categories
- **Known Answer Tests (KAT)** for compliance verification
- **Performance benchmarks** for different security levels
- **Fuzz testing** for robustness
- **Cross-platform validation** ensuring consistency

## 🏗️ Build Requirements

### Minimum Requirements

- **OS**: Ubuntu 18.04+, macOS 10.15+, Windows 10+
- **Compiler**: C++17 compliant compiler (GCC 7.0+, Clang 10+, MSVC 2019+)
- **Memory**: 4GB RAM minimum, 8GB recommended
- **Storage**: 500MB for source and build artifacts

### Dependencies

- **OpenSSL**: 1.1.1+ (for cryptographic primitives)
- **CMake**: 3.16+ (build system)
- **GoogleTest**: Automatically downloaded via FetchContent

### SIMD Support

- **x86-64**: AVX2, AVX512F/VL/BW/DQ (automatic detection)
- **ARM64**: NEON (automatic detection)
- **ARMv7**: NEON (automatic detection)

## 🤝 Contributing

We welcome contributions to ColorKEM! Please see our [Contributing Guide](CONTRIBUTING.md) for details.

### Development Setup

```bash
# Fork and clone
git clone https://github.com/cryptopix-dev/ColorKEM.git
cd ColorKEM

# Create feature branch
git checkout -b feature/your-feature

# Build with tests
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

# Run tests
ctest --output-on-failure
```

## 📄 License

ColorKEM is licensed under the Apache License 2.0. See [LICENSE](LICENSE) for details.

```
Copyright 2025 CRYPTOPIX (OPC) PVT LTD

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
```

## 📞 Contact

- **Project Homepage**: https://github.com/cryptopix-dev/ColorKEM
- **Issues**: https://github.com/cryptopix-dev/ColorKEM/issues
- **Discussions**: https://github.com/cryptopix-dev/ColorKEM/discussions
- **Email**: support@cryptopix.in

### Security

For security-related issues, please email support@cryptopix.in instead of creating public issues.

## 🔐 Cryptographic Implementation Notes

- **Lattice-Based Security**: CLWE-based KEM designed to resist quantum attacks
- **Color-Based Encoding**: Innovative polynomial representation using color spaces for enhanced security
- **Constant-Time Operations**: Side-channel resistant implementations
- **Secure Randomness**: Cryptographically secure random number generation
- **Memory Safety**: Explicit zeroization of sensitive data

## 🙏 Acknowledgments

- OpenSSL project for cryptographic primitives
- GoogleTest framework for testing infrastructure

---

**ColorKEM**: Securing key exchange in the quantum era.
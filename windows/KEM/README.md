# ColorKEM for Windows

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-blue.svg)](https://cmake.org/)

**Platform**: Windows (10+)
**Version**: 1.0.0
**Authors**: CRYPTOPIX (OPC) PVT LTD Development team

This is the Windows-specific implementation of ColorKEM, optimized for Windows systems with support for Visual Studio and MinGW toolchains.

## 🚀 Build Instructions

### Prerequisites

- **Windows**: Version 10 or higher
- **Visual Studio**: 2019 or later (Community Edition is sufficient)
- **CMake**: Version 3.16 or higher
- **Git**: For cloning dependencies
- **OpenSSL**: Pre-built binaries or vcpkg

### Automated Build (Recommended)

Use the provided batch script:

```batch
cd ColorKEM\windows\KEM
build_windows.bat
```

This script will:
- Create a `build/` directory
- Configure the project with CMake using Visual Studio generator
- Build the library and executables

### Manual Build with Visual Studio

```batch
# Create build directory
mkdir build
cd build

# Configure with CMake (Visual Studio)
cmake .. -G "Visual Studio 16 2019" -A x64

# Build
cmake --build . --config Release
```

### Manual Build with MinGW

```batch
# Install MinGW and MSYS2 if not present
# Install dependencies via pacman
pacman -S mingw-w64-x86_64-cmake mingw-w64-x86_64-gcc mingw-w64-x86_64-openssl

# Configure
mkdir build
cd build
cmake .. -G "MinGW Makefiles"

# Build
mingw32-make -j%NUMBER_OF_PROCESSORS%
```

### Build Outputs

After successful build:
- **Library**: `build/Release/clwe_windows.lib` (or `.a` for MinGW)
- **Demo executable**: `build/Release/demo_kem.exe`
- **Benchmark executable**: `build/Release/benchmark_color_kem_timing.exe`
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

### Linking with Visual Studio

Add to your project:
- Include directories: `ColorKEM/windows/KEM/src/include`
- Library directories: `build/Release`
- Link libraries: `clwe_windows.lib`, `libssl.lib`, `libcrypto.lib`

### Linking with MinGW

```bash
g++ -std=c++17 your_app.cpp -Lbuild -lclwe_windows -lssl -lcrypto -o your_app.exe
```

## 🏗️ Platform-Specific Details

### Supported Windows Versions

- **Windows 10**: Full support
- **Windows 11**: Full support
- **Windows Server 2019+**: Full support

### Toolchain Support

- **Visual Studio 2019/2022**: Preferred toolchain with MSVC
- **MinGW-w64**: Alternative GCC-based toolchain
- **Clang**: Via Visual Studio or MinGW

### Architecture Support

- **x64 (AMD64)**: Primary target with AVX2/AVX512 optimizations
- **x86 (32-bit)**: Limited support (deprecated)
- **ARM64**: Experimental support via Visual Studio

### SIMD Optimizations

- **AVX2**: Automatic detection and usage on supported processors
- **AVX512**: For latest Intel processors (when available)
- **Fallback**: Scalar implementations for older systems

### Performance Features

- **Multi-threaded Building**: Uses `%NUMBER_OF_PROCESSORS%` for parallel compilation
- **Memory Management**: Optimized for Windows memory subsystem
- **Security**: Integration with Windows Cryptography API for entropy

### Security Considerations

- **Constant-Time Operations**: Timing attack resistance
- **Secure Memory**: Uses Windows secure memory allocation
- **Random Generation**: Leverages OpenSSL with Windows entropy sources

## 🧪 Testing

Run the test suite:

```batch
cd build
ctest --output-on-failure -C Release
```

Or use the convenience script:

```batch
cd ColorKEM\windows\KEM
run_all_tests.bat
```

## 📞 Support

For Windows-specific issues:
- Check the main [ColorKEM repository](https://github.com/cryptopix-dev/ColorKEM)
- Report issues on [GitHub Issues](https://github.com/cryptopix-dev/ColorKEM/issues)
- Email: support@cryptopix.in

---

**ColorKEM Windows**: Post-quantum cryptography for Windows platforms.
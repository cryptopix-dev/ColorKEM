# ColorKEM Installation Guide

## What Others Need to Install

This guide provides comprehensive installation instructions for users who want to build and use the ColorKEM library on their systems.

---

## 📋 System Requirements

### Minimum Requirements
- **Operating System**: Windows 10+, Ubuntu 18.04+, macOS 10.15+, or ESP32-S3
- **RAM**: 4GB minimum, 8GB recommended
- **Storage**: 500MB for source code and build artifacts
- **Internet Connection**: Required for downloading dependencies

---

## 🔧 Required Software

### All Platforms

#### 1. **CMake** (Version 3.16 or higher)
CMake is the build system used to configure and build ColorKEM.

#### 2. **Git**
Required for cloning the repository and dependencies.

#### 3. **OpenSSL** (Version 1.1.1 or higher)
Used for cryptographic primitives (SHA-3, SHAKE, etc.).

#### 4. **C++17 Compatible Compiler**
- **Linux**: GCC 7.0+ or Clang 10+
- **macOS**: Clang 10+ (comes with Xcode Command Line Tools)
- **Windows**: GCC 9+ (via MinGW-w64)

---

## 🪟 Windows Installation

### Step 1: Install MinGW-w64 (GCC Compiler)

**Option A: Using MSYS2 (Recommended)**

1. Download MSYS2 from [https://www.msys2.org/](https://www.msys2.org/)
2. Install MSYS2 to `C:\msys64` (default location)
3. Open MSYS2 MINGW64 terminal and run:
   ```bash
   pacman -Syu
   pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-cmake mingw-w64-x86_64-openssl git
   ```
4. Add MinGW to your system PATH:
   - Add `C:\msys64\mingw64\bin` to your Windows PATH environment variable

**Option B: Standalone MinGW-w64**

1. Download MinGW-w64 from [https://winlibs.com/](https://winlibs.com/) or [https://github.com/niXman/mingw-builds-binaries/releases](https://github.com/niXman/mingw-builds-binaries/releases)
2. Extract to `C:\mingw64` or `C:\ProgramData\mingw64`
3. Add `C:\mingw64\bin` to your Windows PATH

### Step 2: Install CMake

1. Download CMake from [https://cmake.org/download/](https://cmake.org/download/)
2. Install using the Windows installer
3. During installation, select "Add CMake to system PATH"

### Step 3: Install OpenSSL

**If using MSYS2**: OpenSSL is already installed in Step 1.

**If using standalone MinGW**:
1. Download OpenSSL for Windows from [https://slproweb.com/products/Win32OpenSSL.html](https://slproweb.com/products/Win32OpenSSL.html)
2. Install to `C:\Program Files\OpenSSL-Win64`
3. The build script will auto-detect this location

### Step 4: Install Git

1. Download Git from [https://git-scm.com/download/win](https://git-scm.com/download/win)
2. Install with default options

### Step 5: Verify Installation

Open Command Prompt or PowerShell and verify:
```cmd
gcc --version
g++ --version
cmake --version
git --version
```

### Step 6: Build ColorKEM

```cmd
git clone https://github.com/cryptopix-dev/ColorKEM.git
cd ColorKEM\windows\KEM
build_windows.bat
```

The build script will:
- Auto-detect GCC, G++, CMake, and OpenSSL
- Configure the project
- Build the library and executables
- Run verification tests

---

## 🐧 Linux Installation

### Ubuntu/Debian-based Systems

```bash
# Update package list
sudo apt update

# Install all required dependencies
sudo apt install -y cmake g++ libssl-dev build-essential git

# Clone and build
git clone https://github.com/cryptopix-dev/ColorKEM.git
cd ColorKEM/linux/KEM
chmod +x build_linux.sh
./build_linux.sh
```

### Fedora/RHEL/CentOS

```bash
# Install dependencies
sudo dnf install -y cmake gcc-c++ openssl-devel git

# Clone and build
git clone https://github.com/cryptopix-dev/ColorKEM.git
cd ColorKEM/linux/KEM
chmod +x build_linux.sh
./build_linux.sh
```

### Arch Linux/Manjaro

```bash
# Install dependencies
sudo pacman -Syu --noconfirm cmake gcc openssl git

# Clone and build
git clone https://github.com/cryptopix-dev/ColorKEM.git
cd ColorKEM/linux/KEM
chmod +x build_linux.sh
./build_linux.sh
```

### openSUSE/SLES

```bash
# Install dependencies
sudo zypper install -y cmake gcc-c++ libopenssl-devel git

# Clone and build
git clone https://github.com/cryptopix-dev/ColorKEM.git
cd ColorKEM/linux/KEM
chmod +x build_linux.sh
./build_linux.sh
```

---

## 🍎 macOS Installation

### Step 1: Install Xcode Command Line Tools

```bash
xcode-select --install
```

### Step 2: Install Homebrew (if not already installed)

```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

### Step 3: Install Dependencies

```bash
brew install cmake openssl git
```

### Step 4: Build ColorKEM

```bash
git clone https://github.com/cryptopix-dev/ColorKEM.git
cd ColorKEM/macos/KEM
chmod +x build_macos.sh
./build_macos.sh
```

---

## 📦 Optional Dependencies

### WebP Library (for Key Image Generation)

The WebP library is optional and only required if you want to use the key image generation features.

**Windows (MSYS2)**:
```bash
pacman -S mingw-w64-x86_64-libwebp
```

**Linux (Ubuntu/Debian)**:
```bash
sudo apt install -y libwebp-dev
```

**macOS**:
```bash
brew install webp
```

### Google Test (for Testing)

Google Test is automatically downloaded during the build process via CMake's FetchContent. No manual installation is required.

---

## 🔍 Verification

After building, verify the installation by running:

**Windows**:
```cmd
cd build
benchmark_color_kem_timing.exe
```

**Linux/macOS**:
```bash
cd build
./benchmark_color_kem_timing
```

You should see output showing successful key generation, encapsulation, and decapsulation operations.

---

## 🧪 Running Tests

**Windows**:
```cmd
cd windows\KEM
build_tests.bat
```

**Linux/macOS**:
```bash
cd build
ctest --output-on-failure
```

---

## 🚨 Troubleshooting

### Windows Issues

**Problem**: CMake cannot find GCC/G++
- **Solution**: Ensure MinGW bin directory is in your PATH
- Verify with: `where gcc` and `where g++`

**Problem**: OpenSSL not found
- **Solution**: The build script auto-detects OpenSSL. If it fails:
  1. Install OpenSSL via MSYS2: `pacman -S mingw-w64-x86_64-openssl`
  2. Or create `custom_paths.bat` in `windows/KEM/` with:
     ```bat
     set OPENSSL_ROOT_DIR=C:\path\to\openssl
     ```

**Problem**: Linker errors with `vsnprintf`
- **Solution**: Ensure you're using MinGW-w64, not older MinGW versions
- Update MinGW to the latest version

### Linux Issues

**Problem**: OpenSSL headers not found
- **Solution**: Install development headers:
  - Ubuntu/Debian: `sudo apt install libssl-dev`
  - Fedora/RHEL: `sudo dnf install openssl-devel`

**Problem**: Compiler version too old
- **Solution**: Update GCC to version 7.0 or higher
  - Ubuntu: `sudo apt install gcc-9 g++-9`

### macOS Issues

**Problem**: OpenSSL not found by CMake
- **Solution**: Set OpenSSL path explicitly:
  ```bash
  export OPENSSL_ROOT_DIR=$(brew --prefix openssl)
  ```

---

## 📚 Additional Resources

- **GitHub Repository**: [https://github.com/cryptopix-dev/ColorKEM](https://github.com/cryptopix-dev/ColorKEM)
- **Documentation**: See `README.md` in the repository root
- **Support**: support@cryptopix.in
- **Issues**: [https://github.com/cryptopix-dev/ColorKEM/issues](https://github.com/cryptopix-dev/ColorKEM/issues)

---

## 📝 Quick Reference

### Minimum Software Versions

| Software | Minimum Version | Recommended Version |
|----------|----------------|---------------------|
| CMake    | 3.16           | 3.20+              |
| GCC      | 7.0            | 9.0+               |
| Clang    | 10.0           | 12.0+              |
| OpenSSL  | 1.1.1          | 3.0+               |
| Git      | 2.0            | 2.30+              |

### Platform-Specific Package Names

| Dependency | Ubuntu/Debian | Fedora/RHEL | Arch | macOS (Homebrew) | Windows (MSYS2) |
|------------|---------------|-------------|------|------------------|-----------------|
| CMake      | cmake         | cmake       | cmake | cmake           | mingw-w64-x86_64-cmake |
| GCC/G++    | build-essential | gcc-c++   | gcc   | (Xcode CLT)     | mingw-w64-x86_64-gcc |
| OpenSSL    | libssl-dev    | openssl-devel | openssl | openssl      | mingw-w64-x86_64-openssl |
| Git        | git           | git         | git   | git             | git |

---

## ✅ Summary Checklist

Before building ColorKEM, ensure you have:

- [ ] C++17 compatible compiler (GCC 7+, Clang 10+, or MSVC 2019+)
- [ ] CMake 3.16 or higher
- [ ] OpenSSL 1.1.1 or higher (with development headers)
- [ ] Git
- [ ] 4GB+ RAM
- [ ] 500MB+ free disk space
- [ ] Internet connection (for downloading Google Test)

---

**Copyright © 2025 CRYPTOPIX (OPC) PVT LTD**  
**License**: Apache 2.0

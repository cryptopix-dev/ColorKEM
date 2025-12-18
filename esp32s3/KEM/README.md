# ColorKEM for ESP32-S3

[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](https://opensource.org/licenses/Apache-2.0)
[![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![ESP-IDF](https://img.shields.io/badge/ESP--IDF-4.4+-blue.svg)](https://docs.espressif.com/projects/esp-idf/en/latest/)

**Platform**: ESP32-S3
**Version**: 1.0.0
**Authors**: CRYPTOPIX (OPC) PVT LTD Development team

This is the ESP32-S3-specific implementation of ColorKEM, optimized for embedded IoT applications with ESP-IDF framework.

## 🚀 Build Instructions

### Prerequisites

- **ESP-IDF**: Version 4.4 or higher
- **ESP32-S3 Development Board**: With sufficient flash and RAM
- **CMake**: Version 3.16 or higher (included with ESP-IDF)

### Setup ESP-IDF

Follow the [ESP-IDF Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html) to install and configure ESP-IDF.

### Build

```bash
cd ColorKEM/iot/esp32s3
idf.py set-target esp32s3
idf.py build
```

### Flash and Monitor

```bash
idf.py flash
idf.py monitor
```

## 💡 Usage

### ESP-IDF Component Integration

Add this component to your ESP-IDF project by copying the `KEM` directory to your project's `components/` folder.

```cpp
#include "clwe/color_kem.hpp"

extern "C" void app_main() {
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

    // Check success
    if (shared_secret == recovered_secret) {
        ESP_LOGI(TAG, "KEM operation successful");
    }
}
```

## 🏗️ Platform-Specific Details

### Supported ESP32-S3 Features

- **ESP32-S3**: Full support with Xtensa LX7 core
- **Memory**: Optimized for limited RAM (up to 512KB)
- **Flash**: Support for external flash storage

### Performance Optimizations

- **Scalar Operations**: Optimized for ESP32-S3 without SIMD extensions
- **Memory Management**: Efficient use of ESP32 memory subsystems
- **Crypto Acceleration**: Integration with ESP32 hardware crypto accelerators

### Security Considerations

- **Constant-Time Operations**: Timing attack resistance
- **Secure Memory**: Uses ESP32 secure memory features
- **Random Number Generation**: Leverages ESP-IDF entropy sources

## 🧪 Testing

Run the test suite:

```bash
idf.py test
```

## 📞 Support

For ESP32-S3-specific issues:
- Check the main [ColorKEM repository](https://github.com/cryptopix-dev/ColorKEM)
- Report issues on [GitHub Issues](https://github.com/cryptopix-dev/ColorKEM/issues)
- Email: support@cryptopix.in

---

**ColorKEM ESP32-S3**: Post-quantum cryptography for IoT devices.
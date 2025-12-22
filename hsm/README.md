# ColorKEM HSM Integration

This directory contains the Hardware Security Module (HSM) integration for ColorKEM, providing secure key storage and cryptographic operations.

## Overview

The HSM integration provides:
- Abstract HSM interface for pluggable backends
- Software HSM simulation for development/testing
- PKCS#11 compatibility layer
- Optional HSM-backed operations in ColorKEM
- Configuration system for enabling/disabling HSM usage

## Architecture

### Core Components

- **`hsm_interface.hpp`**: Abstract base class defining HSM operations
- **`software_hsm.hpp/.cpp`**: Software simulation of HSM functionality
- **`pkcs11_layer.hpp`**: Basic PKCS#11 structures and constants
- **`hsm_config.hpp/.cpp`**: Configuration management for HSM backends

### Integration Points

The HSM integration is optional and does not break existing software-only operation. When enabled:

- Private keys can be stored securely in the HSM
- Cryptographic operations can be performed in hardware
- Fallback to software operation is available

## Usage

### Basic Setup

```cpp
#include "hsm/hsm_config.hpp"
#include "hsm/software_hsm.hpp"

// Configure for software simulation
auto config = clwe::hsm::HSMConfig::softwareSimulation();
clwe::hsm::set_global_hsm_config(config);

// Create ColorKEM with HSM support
clwe::CLWEParameters params(768);
clwe::ColorKEM kem(params);  // Uses HSM from global config

// Or explicitly
clwe::ColorKEM kem(params, config);
```

### HSM Operations

```cpp
// Create HSM instance
auto hsm = clwe::hsm::create_software_hsm();

// Initialize
auto result = hsm->initialize();
if (!result.success) {
    // Handle error
}

// Generate key
auto key_result = hsm->generate_key(clwe::hsm::KeyType::SECRET_KEY, 32);
if (key_result.success) {
    // Use key_result.data
}

// Import existing key
std::vector<uint8_t> key_data = {1, 2, 3, 4};
auto import_result = hsm->import_key(clwe::hsm::KeyType::SECRET_KEY, key_data);
```

## Configuration

### Backend Types

- **`NONE`**: No HSM, software-only operation
- **`SOFTWARE_SIM`**: Software simulation for development
- **`HARDWARE`**: Real hardware HSM (not implemented)
- **`PKCS11`**: PKCS#11 compatible HSM (basic structure)

### Configuration Options

```cpp
clwe::hsm::HSMConfig config;
config.backend = clwe::hsm::HSMBackend::SOFTWARE_SIM;
config.enable_key_caching = true;
config.require_hsm_for_private_keys = false;  // Allow fallback
```

## Building

To enable HSM support, define `ENABLE_HSM` during compilation:

```bash
g++ -DENABLE_HSM -I/path/to/hsm/include ...
```

## Testing

Run the integration tests:

```bash
cd hsm/src/hsm
g++ -DENABLE_HSM -I../../../hsm/include -I../../../linux/KEM/src/include \
    test_hsm_integration.cpp software_hsm.cpp hsm_config.cpp -o test_hsm
./test_hsm
```

## Security Considerations

- The software HSM simulation is for development only
- Real HSMs should be used for production deployments
- Private keys are sensitive and should be protected
- HSM operations may have performance implications

## Future Extensions

- Hardware HSM driver implementations
- Full PKCS#11 compliance
- Support for additional cryptographic mechanisms
- Key lifecycle management
- Audit logging

## License

This HSM integration follows the same license as ColorKEM.
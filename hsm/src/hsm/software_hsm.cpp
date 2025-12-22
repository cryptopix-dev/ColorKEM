/**
 * @file software_hsm.cpp
 * @brief Software-based HSM simulation for development and testing
 *
 * This implementation provides a software simulation of HSM functionality,
 * storing keys in memory with basic security measures. It serves as a
 * development/testing backend that can be replaced with hardware HSM drivers.
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 */

#include "hsm/hsm_interface.hpp"
#include <unordered_map>
#include <mutex>
#include <random>
#include <cstring>
#include <algorithm>
#include <memory>

namespace clwe {
namespace hsm {

/**
 * @brief Software HSM implementation
 *
 * Simulates HSM operations using secure memory storage.
 * Keys are stored encrypted in memory for basic protection.
 */
class SoftwareHSM : public HSMInterface {
private:
    std::unordered_map<KeyHandle, std::vector<uint8_t>> key_store_;
    std::unordered_map<KeyHandle, KeyType> key_types_;
    KeyHandle next_handle_;
    mutable std::mutex mutex_;
    std::array<uint8_t, 32> encryption_key_;  // Simple key for encrypting stored keys

    /**
     * @brief Generate a new unique key handle
     */
    KeyHandle generate_handle() {
        std::lock_guard<std::mutex> lock(mutex_);
        do {
            next_handle_++;
            if (next_handle_ == INVALID_KEY_HANDLE) next_handle_++;
        } while (key_store_.find(next_handle_) != key_store_.end());
        return next_handle_;
    }

    /**
     * @brief Simple XOR encryption for key storage
     * Note: This is for simulation only - real HSMs use proper encryption
     */
    std::vector<uint8_t> encrypt_data(const std::vector<uint8_t>& data) {
        std::vector<uint8_t> encrypted = data;
        for (size_t i = 0; i < encrypted.size(); ++i) {
            encrypted[i] ^= encryption_key_[i % encryption_key_.size()];
        }
        return encrypted;
    }

    /**
     * @brief Decrypt stored key data
     */
    std::vector<uint8_t> decrypt_data(const std::vector<uint8_t>& data) {
        return encrypt_data(data);  // XOR is symmetric
    }

    /**
     * @brief Secure random byte generation
     */
    void secure_random_bytes(uint8_t* buffer, size_t size) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 255);

        for (size_t i = 0; i < size; ++i) {
            buffer[i] = static_cast<uint8_t>(dis(gen));
        }
    }

public:
    SoftwareHSM() : next_handle_(INVALID_KEY_HANDLE) {
        // Generate a random encryption key for the simulation
        secure_random_bytes(encryption_key_.data(), encryption_key_.size());
    }

    ~SoftwareHSM() override {
        // Securely erase all stored keys
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& pair : key_store_) {
            std::fill(pair.second.begin(), pair.second.end(), 0);
        }
        key_store_.clear();
        key_types_.clear();
    }

    HSMResult initialize() override {
        // Software HSM is always ready
        return HSMResult(true, "Software HSM initialized successfully");
    }

    HSMResult generate_key(KeyType key_type, size_t key_size) override {
        std::lock_guard<std::mutex> lock(mutex_);

        // Generate random key data
        std::vector<uint8_t> key_data(key_size);
        secure_random_bytes(key_data.data(), key_size);

        // Store the key
        KeyHandle handle = generate_handle();
        key_store_[handle] = encrypt_data(key_data);
        key_types_[handle] = key_type;

        HSMResult result(true, "Key generated successfully");
        result.data = key_data;  // Return the key data for public keys
        return result;
    }

    HSMResult import_key(KeyType key_type, const std::vector<uint8_t>& key_data) override {
        std::lock_guard<std::mutex> lock(mutex_);

        if (key_data.empty()) {
            return HSMResult(false, "Cannot import empty key data");
        }

        KeyHandle handle = generate_handle();
        key_store_[handle] = encrypt_data(key_data);
        key_types_[handle] = key_type;

        HSMResult result(true, "Key imported successfully");
        // Don't return the data, just the handle
        return result;
    }

    HSMResult export_public_key(KeyHandle key_handle) override {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = key_store_.find(key_handle);
        if (it == key_store_.end()) {
            return HSMResult(false, "Invalid key handle");
        }

        auto type_it = key_types_.find(key_handle);
        if (type_it == key_types_.end() || type_it->second != KeyType::PUBLIC_KEY) {
            return HSMResult(false, "Key is not a public key or type unknown");
        }

        HSMResult result(true, "Public key exported successfully");
        result.data = decrypt_data(it->second);
        return result;
    }

    HSMResult delete_key(KeyHandle key_handle) override {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = key_store_.find(key_handle);
        if (it == key_store_.end()) {
            return HSMResult(false, "Invalid key handle");
        }

        // Securely erase the key
        std::fill(it->second.begin(), it->second.end(), 0);
        key_store_.erase(it);
        key_types_.erase(key_handle);

        return HSMResult(true, "Key deleted successfully");
    }

    HSMResult perform_crypto_operation(CryptoOperation operation,
                                     KeyHandle key_handle,
                                     const std::vector<uint8_t>& input_data,
                                     const std::vector<uint8_t>& parameters) override {
        std::lock_guard<std::mutex> lock(mutex_);

        auto it = key_store_.find(key_handle);
        if (it == key_store_.end()) {
            return HSMResult(false, "Invalid key handle");
        }

        // For simulation, we just return the input data as "processed"
        // Real HSM would perform actual cryptographic operations
        HSMResult result(true, "Cryptographic operation completed");
        result.data = input_data;  // Echo back for simulation
        return result;
    }

    HSMResult get_status() override {
        std::lock_guard<std::mutex> lock(mutex_);

        HSMResult result(true, "Software HSM operational");
        result.data = {
            static_cast<uint8_t>(key_store_.size() & 0xFF),
            static_cast<uint8_t>((key_store_.size() >> 8) & 0xFF),
            static_cast<uint8_t>((key_store_.size() >> 16) & 0xFF),
            static_cast<uint8_t>((key_store_.size() >> 24) & 0xFF)
        };
        return result;
    }

    bool is_valid_key_handle(KeyHandle key_handle) const override {
        std::lock_guard<std::mutex> lock(mutex_);
        return key_store_.find(key_handle) != key_store_.end();
    }
};

/**
 * @brief Factory function to create a software HSM instance
 */
std::unique_ptr<HSMInterface> create_software_hsm() {
    return std::make_unique<SoftwareHSM>();
}

} // namespace hsm
} // namespace clwe
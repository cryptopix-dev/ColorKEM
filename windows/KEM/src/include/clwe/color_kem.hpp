#ifndef COLOR_KEM_HPP
#define COLOR_KEM_HPP

#include "clwe.hpp"
#include "color_value.hpp"
#include "color_ntt_engine.hpp"
#include <vector>
#include <array>
#include <memory>

namespace clwe {

// Forward declarations
class ColorNTTEngine;
class ColorKEM;

// Key structures for Color KEM
struct ColorPublicKey {
    std::array<uint8_t, 32> seed;
    std::vector<uint8_t> public_data;
    CLWEParameters params;
    uint8_t format_version = 0x01;       // Format version (1 = original, 2 = compressed)
    bool use_compression = false;        // Flag indicating if compression is used
    std::vector<uint8_t> metadata;       // Additional metadata for key management

    ColorPublicKey() = default;
    ColorPublicKey(const std::array<uint8_t, 32>& s, const std::vector<uint8_t>& pd, const CLWEParameters& p,
                   bool compressed = false, const std::vector<uint8_t>& meta = {})
        : seed(s), public_data(pd), params(p), use_compression(compressed), metadata(meta) {}

    std::vector<uint8_t> serialize() const;
    static ColorPublicKey deserialize(const std::vector<uint8_t>& data, const CLWEParameters& params);
};

struct ColorPrivateKey {
    std::vector<uint8_t> secret_data;
    CLWEParameters params;
    uint8_t format_version = 0x01;       // Format version (1 = original, 2 = compressed)
    bool use_compression = false;        // Flag indicating if compression is used
    std::vector<uint8_t> metadata;       // Additional metadata for key management

    ColorPrivateKey() = default;
    ColorPrivateKey(const std::vector<uint8_t>& sd, const CLWEParameters& p,
                    bool compressed = false, const std::vector<uint8_t>& meta = {})
        : secret_data(sd), params(p), use_compression(compressed), metadata(meta) {}

    std::vector<uint8_t> serialize() const;
    static ColorPrivateKey deserialize(const std::vector<uint8_t>& data, const CLWEParameters& params);
};

struct ColorCiphertext {
    std::vector<uint8_t> ciphertext_data;
    std::vector<uint8_t> shared_secret_hint;
    CLWEParameters params;
    uint8_t format_version = 0x01;       // Format version (1 = original, 2 = compressed)
    bool use_compression = false;        // Flag indicating if compression is used
    std::vector<uint8_t> metadata;       // Additional metadata for ciphertext management

    ColorCiphertext() = default;
    ColorCiphertext(const std::vector<uint8_t>& cd, const std::vector<uint8_t>& ssh, const CLWEParameters& p,
                    bool compressed = false, const std::vector<uint8_t>& meta = {})
        : ciphertext_data(cd), shared_secret_hint(ssh), params(p), use_compression(compressed), metadata(meta) {}

    std::vector<uint8_t> serialize() const;
    static ColorCiphertext deserialize(const std::vector<uint8_t>& data);
};

// Color-based KEM implementation
class ColorKEM {
private:
    CLWEParameters params_;
    std::unique_ptr<ColorNTTEngine> color_ntt_engine_;

    // Helper methods
    std::vector<std::vector<ColorValue>> generate_matrix_A(const std::array<uint8_t, 32>& seed) const;
    std::vector<ColorValue> generate_error_vector(uint32_t eta) const;
    std::vector<ColorValue> generate_secret_key(uint32_t eta) const;
    // Deterministic versions for KATs
    std::vector<ColorValue> generate_error_vector_deterministic(uint32_t eta, const std::array<uint8_t, 32>& seed) const;
    std::vector<ColorValue> generate_secret_key_deterministic(uint32_t eta, const std::array<uint8_t, 32>& seed) const;
    std::vector<ColorValue> generate_public_key(const std::vector<ColorValue>& secret_key,
                                               const std::vector<std::vector<ColorValue>>& matrix_A,
                                               const std::vector<ColorValue>& error_vector) const;
    std::vector<ColorValue> matrix_vector_mul(const std::vector<std::vector<ColorValue>>& matrix,
                                             const std::vector<ColorValue>& vector) const;
    std::vector<ColorValue> matrix_transpose_vector_mul(const std::vector<std::vector<ColorValue>>& matrix,
                                                        const std::vector<ColorValue>& vector) const;
    ColorValue decrypt_message(const std::vector<ColorValue>& secret_key,
                              const std::vector<ColorValue>& ciphertext) const;
    ColorValue generate_shared_secret() const;
    std::vector<uint8_t> encode_color_secret(const ColorValue& secret) const;
    ColorValue decode_color_secret(const std::vector<uint8_t>& encoded) const;
    std::vector<uint8_t> color_secret_to_bytes(const ColorValue& secret);
    ColorValue bytes_to_color_secret(const std::vector<uint8_t>& bytes);
    std::vector<ColorValue> encrypt_message(const std::vector<std::vector<ColorValue>>& matrix_A,
                                           const std::vector<ColorValue>& public_key,
                                           const ColorValue& message) const;
    std::vector<ColorValue> encrypt_message_deterministic(const std::vector<std::vector<ColorValue>>& matrix_A,
                                                         const std::vector<ColorValue>& public_key,
                                                         const ColorValue& message,
                                                         const std::array<uint8_t, 32>& r_seed,
                                                         const std::array<uint8_t, 32>& e1_seed,
                                                         const std::array<uint8_t, 32>& e2_seed) const;

public:
    ColorKEM(const CLWEParameters& params);
    ~ColorKEM();

    // Disable copy and assignment
    ColorKEM(const ColorKEM&) = delete;
    ColorKEM& operator=(const ColorKEM&) = delete;

    // Key generation
    std::pair<ColorPublicKey, ColorPrivateKey> keygen();

    // Optimized key generation with compression
    std::pair<ColorPublicKey, ColorPrivateKey> generate_keypair_optimized();

    // Deterministic key generation (for KATs)
    std::pair<ColorPublicKey, ColorPrivateKey> keygen_deterministic(const std::array<uint8_t, 32>& matrix_seed,
                                                                    const std::array<uint8_t, 32>& secret_seed,
                                                                    const std::array<uint8_t, 32>& error_seed);

    // Encapsulation: returns (ciphertext, shared_secret)
    std::pair<ColorCiphertext, ColorValue> encapsulate(const ColorPublicKey& public_key);

    // Deterministic encapsulation (for KATs)
    std::pair<ColorCiphertext, ColorValue> encapsulate_deterministic(const ColorPublicKey& public_key,
                                                                    const std::array<uint8_t, 32>& r_seed,
                                                                    const std::array<uint8_t, 32>& e1_seed,
                                                                    const std::array<uint8_t, 32>& e2_seed,
                                                                    const ColorValue& shared_secret);

    // Decapsulation
    ColorValue decapsulate(const ColorPublicKey& public_key,
                          const ColorPrivateKey& private_key,
                          const ColorCiphertext& ciphertext);

    // Key verification
    bool verify_keypair(const ColorPublicKey& public_key, const ColorPrivateKey& private_key) const;

    // Hash ciphertext for FO transform
    ColorValue hash_ciphertext(const ColorCiphertext& ciphertext) const;

    // Getters
    const CLWEParameters& params() const { return params_; }
};

} // namespace clwe

#endif // COLOR_KEM_HPP
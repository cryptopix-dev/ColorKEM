#ifndef COLOR_KEM_HPP
#define COLOR_KEM_HPP

#include "color_value.hpp"
#include "color_ntt_engine.hpp"
#include "clwe/clwe.hpp"
#include <vector>
#include <array>

namespace clwe {

// Key structures for Color KEM
struct ColorPublicKey {
    std::array<uint8_t, 32> seed;
    std::vector<uint8_t> public_data;
    CLWEParameters params;

    std::vector<uint8_t> serialize() const;
    static ColorPublicKey deserialize(const std::vector<uint8_t>& data, const CLWEParameters& params);
};

struct ColorPrivateKey {
    std::vector<uint8_t> secret_data;
    CLWEParameters params;

    std::vector<uint8_t> serialize() const;
    static ColorPrivateKey deserialize(const std::vector<uint8_t>& data, const CLWEParameters& params);
};

struct ColorCiphertext {
    std::vector<uint8_t> ciphertext_data;
    std::vector<uint8_t> shared_secret_hint;
    CLWEParameters params;

    std::vector<uint8_t> serialize() const;
    static ColorCiphertext deserialize(const std::vector<uint8_t>& data);
};

class ColorKEM {
private:
    CLWEParameters params_;
    std::unique_ptr<ColorNTTEngine> color_ntt_engine_;

    std::vector<std::vector<std::vector<ColorValue>>> generate_matrix_A(const std::array<uint8_t, 32>& seed) const;
    std::vector<std::vector<ColorValue>> generate_secret_key(uint32_t eta) const;
    std::vector<std::vector<ColorValue>> generate_error_vector(uint32_t eta) const;
    std::vector<std::vector<ColorValue>> generate_error_vector_deterministic(uint32_t eta, const std::array<uint8_t, 32>& seed) const;
    std::vector<std::vector<ColorValue>> generate_secret_key_deterministic(uint32_t eta, const std::array<uint8_t, 32>& seed) const;
    std::vector<std::vector<ColorValue>> generate_public_key(const std::vector<std::vector<ColorValue>>& secret_key,
                                                const std::vector<std::vector<std::vector<ColorValue>>>& matrix_A,
                                                const std::vector<std::vector<ColorValue>>& error_vector) const;
    std::vector<std::vector<ColorValue>> matrix_vector_mul(const std::vector<std::vector<std::vector<ColorValue>>>& matrix,
                                             const std::vector<std::vector<ColorValue>>& vector) const;
    std::vector<std::vector<ColorValue>> matrix_transpose_vector_mul(const std::vector<std::vector<std::vector<ColorValue>>>& matrix,
                                                       const std::vector<std::vector<ColorValue>>& vector) const;
    ColorValue decrypt_message(const std::vector<std::vector<ColorValue>>& secret_key,
                              const std::vector<std::vector<ColorValue>>& ciphertext) const;
    ColorValue generate_shared_secret() const;
    std::vector<uint8_t> encode_color_secret(const ColorValue& secret) const;
    ColorValue decode_color_secret(const std::vector<uint8_t>& encoded) const;
    std::vector<std::vector<ColorValue>> encrypt_message(const std::vector<std::vector<std::vector<ColorValue>>>& matrix_A,
                                           const std::vector<std::vector<ColorValue>>& public_key,
                                           const ColorValue& message) const;
    std::vector<std::vector<ColorValue>> encrypt_message_deterministic(const std::vector<std::vector<std::vector<ColorValue>>>& matrix_A,
                                                                const std::vector<std::vector<ColorValue>>& public_key,
                                                                const ColorValue& message,
                                                                const std::array<uint8_t, 32>& r_seed,
                                                                const std::array<uint8_t, 32>& e1_seed,
                                                                const std::array<uint8_t, 32>& e2_seed) const;

public:
    ColorKEM(const CLWEParameters& params = CLWEParameters());
    ~ColorKEM();

    ColorKEM(const ColorKEM&) = delete;
    ColorKEM& operator=(const ColorKEM&) = delete;

    std::pair<ColorPublicKey, ColorPrivateKey> keygen();
    std::pair<ColorPublicKey, ColorPrivateKey> keygen_deterministic(const std::array<uint8_t, 32>& matrix_seed,
                                                                   const std::array<uint8_t, 32>& secret_seed,
                                                                   const std::array<uint8_t, 32>& error_seed);

    std::pair<ColorCiphertext, ColorValue> encapsulate(const ColorPublicKey& public_key);
    std::pair<ColorCiphertext, ColorValue> encapsulate_deterministic(const ColorPublicKey& public_key,
                                                                     const std::array<uint8_t, 32>& r_seed,
                                                                     const std::array<uint8_t, 32>& e1_seed,
                                                                     const std::array<uint8_t, 32>& e2_seed,
                                                                     const ColorValue& shared_secret);

    ColorValue decapsulate(const ColorPublicKey& public_key,
                           const ColorPrivateKey& private_key,
                           const ColorCiphertext& ciphertext);

    bool verify_keypair(const ColorPublicKey& public_key, const ColorPrivateKey& private_key) const;

private:
    ColorValue hash_ciphertext(const ColorCiphertext& ciphertext) const;

    const CLWEParameters& params() const { return params_; }

    static std::vector<uint8_t> color_secret_to_bytes(const ColorValue& secret);
    static ColorValue bytes_to_color_secret(const std::vector<uint8_t>& bytes);
};

} // namespace clwe

#endif // COLOR_KEM_HPP
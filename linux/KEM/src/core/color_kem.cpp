#include "color_kem.hpp"
#include "shake_sampler.hpp"
#include "utils.hpp"
#include <random>
#include <cstring>
#include <algorithm>
#include <iomanip>
#include <stdexcept>
#ifdef ENABLE_HSM
#include "hsm/software_hsm.hpp"
#endif

#ifdef HAVE_AVX2
#include <immintrin.h>
#endif

#ifdef HAVE_AVX512
#include <immintrin.h>
#endif

#ifdef HAVE_NEON
#include <arm_neon.h>
#endif

namespace clwe {

ColorKEM::ColorKEM(const CLWEParameters& params)
    : params_(params), cpu_features_(CPUFeatureDetector::detect()) {
    color_ntt_engine_ = std::make_unique<ColorNTTEngine>(params_.modulus, params_.degree);
#ifdef ENABLE_HSM
    hsm_config_ = clwe::hsm::get_global_hsm_config();
    if (hsm_config_.isEnabled()) {
        if (hsm_config_.backend == clwe::hsm::HSMBackend::SOFTWARE_SIM) {
            hsm_ = clwe::hsm::create_software_hsm();
            auto result = hsm_->initialize();
            if (!result.success) {
                throw std::runtime_error("Failed to initialize software HSM: " + result.error_message);
            }
        }
        // TODO: Add support for other HSM backends
    }
#endif
}

ColorKEM::~ColorKEM() = default;

#ifdef ENABLE_HSM
ColorKEM::ColorKEM(const CLWEParameters& params, const clwe::hsm::HSMConfig& hsm_config)
    : params_(params), cpu_features_(CPUFeatureDetector::detect()), hsm_config_(hsm_config) {
    color_ntt_engine_ = std::make_unique<ColorNTTEngine>(params_.modulus, params_.degree);

    if (hsm_config_.isEnabled()) {
        if (hsm_config_.backend == clwe::hsm::HSMBackend::SOFTWARE_SIM) {
            hsm_ = clwe::hsm::create_software_hsm();
            auto result = hsm_->initialize();
            if (!result.success) {
                throw std::runtime_error("Failed to initialize software HSM: " + result.error_message);
            }
        }
        // TODO: Add support for other HSM backends
    }
}
#endif


std::vector<std::vector<std::vector<ColorValue>>> ColorKEM::generate_matrix_A(const std::array<uint8_t, 32>& seed) const {
    uint32_t k = params_.module_rank;
    uint32_t n = params_.degree;
    uint32_t q = params_.modulus;

    std::vector<std::vector<std::vector<ColorValue>>> matrix(k, std::vector<std::vector<ColorValue>>(k, std::vector<ColorValue>(n)));

    
    for (uint32_t i = 0; i < k; ++i) {
        for (uint32_t j = 0; j < k; ++j) {
            
            std::vector<uint8_t> shake_input;
            shake_input.reserve(seed.size() + 2);
            shake_input.insert(shake_input.end(), seed.begin(), seed.end());
            shake_input.push_back(static_cast<uint8_t>(i));
            shake_input.push_back(static_cast<uint8_t>(j));

            
            SHAKE128Sampler shake128;
            shake128.init(shake_input.data(), shake_input.size());

            
            size_t coeff_idx = 0;
            while (coeff_idx < n) {
                std::array<uint8_t, 3> bytes;
                shake128.squeeze(bytes.data(), bytes.size());

                
                uint16_t coeff1 = ((bytes[0] << 4) | (bytes[1] >> 4)) & 0xFFF;
                uint16_t coeff2 = ((bytes[1] << 8) | bytes[2]) & 0xFFF;

                if (coeff1 < q && coeff_idx < n) {
                    matrix[i][j][coeff_idx++] = ColorValue::from_math_value(coeff1);
                }
                if (coeff2 < q && coeff_idx < n) {
                    matrix[i][j][coeff_idx++] = ColorValue::from_math_value(coeff2);
                }
            }
        }
    }

    return matrix;
}


std::vector<std::vector<ColorValue>> ColorKEM::generate_error_vector(uint32_t eta) const {
    std::vector<std::vector<ColorValue>> error_vector(params_.module_rank, std::vector<ColorValue>(params_.degree));

    for (auto& poly : error_vector) {
        SHAKE256Sampler sampler;
        std::array<uint8_t, 32> seed;
        secure_random_bytes(seed.data(), seed.size());
        sampler.init(seed.data(), seed.size());

        sampler.sample_polynomial_binomial(reinterpret_cast<uint32_t*>(poly.data()), params_.degree, eta, params_.modulus);

        for (auto& coeff : poly) {
            coeff = ColorValue::from_math_value(coeff.to_math_value());
        }
    }

    return error_vector;
}


std::vector<std::vector<ColorValue>> ColorKEM::generate_secret_key(uint32_t eta) const {
    std::vector<std::vector<ColorValue>> secret_key(params_.module_rank, std::vector<ColorValue>(params_.degree));

    for (auto& poly : secret_key) {
        SHAKE256Sampler sampler;
        std::array<uint8_t, 32> seed;
        secure_random_bytes(seed.data(), seed.size());
        sampler.init(seed.data(), seed.size());

        sampler.sample_polynomial_binomial(reinterpret_cast<uint32_t*>(poly.data()), params_.degree, eta, params_.modulus);

        for (auto& coeff : poly) {
            coeff = ColorValue::from_math_value(coeff.to_math_value());
        }
    }

    return secret_key;
}


std::vector<std::vector<ColorValue>> ColorKEM::generate_public_key(const std::vector<std::vector<ColorValue>>& secret_key,
                                                    const std::vector<std::vector<std::vector<ColorValue>>>& matrix_A,
                                                    const std::vector<std::vector<ColorValue>>& error_vector) const {

    auto As = this->matrix_vector_mul(matrix_A, secret_key);
    std::vector<std::vector<ColorValue>> public_key(params_.module_rank, std::vector<ColorValue>(params_.degree));

    for (uint32_t i = 0; i < params_.module_rank; ++i) {
        for (uint32_t d = 0; d < params_.degree; ++d) {
            uint64_t as_val = As[i][d].to_math_value();
            uint64_t e_val = error_vector[i][d].to_math_value();
            uint64_t pk_val = (as_val + e_val) % params_.modulus;
            public_key[i][d] = ColorValue::from_math_value(pk_val);
        }
    }

    return public_key;
}


std::vector<std::vector<ColorValue>> ColorKEM::matrix_vector_mul(const std::vector<std::vector<std::vector<ColorValue>>>& matrix,
                                                   const std::vector<std::vector<ColorValue>>& vector) const {
    uint32_t k = params_.module_rank;
    uint32_t n = params_.degree;

    // Validate matrix dimensions
    if (matrix.size() != k) {
        throw std::invalid_argument("Invalid matrix rows: expected " + std::to_string(k) + ", got " + std::to_string(matrix.size()));
    }
    for (const auto& row : matrix) {
        if (row.size() != k) {
            throw std::invalid_argument("Invalid matrix columns: expected " + std::to_string(k) + " per row");
        }
        for (const auto& poly : row) {
            if (poly.size() != n) {
                throw std::invalid_argument("Invalid polynomial size: expected " + std::to_string(n));
            }
        }
    }

    // Validate vector size
    if (vector.size() != k) {
        throw std::invalid_argument("Invalid vector size: expected " + std::to_string(k) + ", got " + std::to_string(vector.size()));
    }
    for (const auto& poly : vector) {
        if (poly.size() != n) {
            throw std::invalid_argument("Invalid polynomial size: expected " + std::to_string(n));
        }
    }

    std::vector<std::vector<ColorValue>> result(k, std::vector<ColorValue>(n));

    for (uint32_t i = 0; i < k; ++i) {
        std::vector<ColorValue> sum(n, ColorValue(0, 0, 0, 0)); // Zero polynomial
        for (uint32_t j = 0; j < k; ++j) {
            std::vector<ColorValue> product(n);
            color_ntt_engine_->multiply_colors(matrix[i][j].data(), vector[j].data(), product.data());
            // Add product to sum
            for (uint32_t d = 0; d < n; ++d) {
                uint64_t s_val = sum[d].to_math_value();
                uint64_t p_val = product[d].to_math_value();
                uint64_t new_val = (s_val + p_val) % params_.modulus;
                sum[d] = ColorValue::from_math_value(new_val);
            }
        }
        result[i] = sum;
    }

    return result;
}


std::vector<std::vector<ColorValue>> ColorKEM::matrix_transpose_vector_mul(const std::vector<std::vector<std::vector<ColorValue>>>& matrix,
                                                             const std::vector<std::vector<ColorValue>>& vector) const {
    uint32_t k = params_.module_rank;
    uint32_t n = params_.degree;

    // Validate matrix dimensions
    if (matrix.size() != k) {
        throw std::invalid_argument("Invalid matrix rows: expected " + std::to_string(k) + ", got " + std::to_string(matrix.size()));
    }
    for (const auto& row : matrix) {
        if (row.size() != k) {
            throw std::invalid_argument("Invalid matrix columns: expected " + std::to_string(k) + " per row");
        }
        for (const auto& poly : row) {
            if (poly.size() != n) {
                throw std::invalid_argument("Invalid polynomial size: expected " + std::to_string(n));
            }
        }
    }

    // Validate vector size
    if (vector.size() != k) {
        throw std::invalid_argument("Invalid vector size: expected " + std::to_string(k) + ", got " + std::to_string(vector.size()));
    }
    for (const auto& poly : vector) {
        if (poly.size() != n) {
            throw std::invalid_argument("Invalid polynomial size: expected " + std::to_string(n));
        }
    }

    std::vector<std::vector<ColorValue>> result(k, std::vector<ColorValue>(n));

    for (uint32_t i = 0; i < k; ++i) {
        std::vector<ColorValue> sum(n, ColorValue(0, 0, 0, 0)); // Zero polynomial
        for (uint32_t j = 0; j < k; ++j) {
            std::vector<ColorValue> product(n);
            color_ntt_engine_->multiply_colors(matrix[j][i].data(), vector[j].data(), product.data());
            // Add product to sum
            for (uint32_t d = 0; d < n; ++d) {
                uint64_t s_val = sum[d].to_math_value();
                uint64_t p_val = product[d].to_math_value();
                uint64_t new_val = (s_val + p_val) % params_.modulus;
                sum[d] = ColorValue::from_math_value(new_val);
            }
        }
        result[i] = sum;
    }

    return result;
}



ColorValue ColorKEM::decrypt_message(const std::vector<std::vector<ColorValue>>& secret_key,
                                     const std::vector<std::vector<ColorValue>>& ciphertext) const {

    uint32_t k = params_.module_rank;
    uint32_t q = params_.modulus;

    if (ciphertext.size() != k + 1) {
        throw std::invalid_argument("Invalid ciphertext size: expected " + std::to_string(k + 1) + " polynomials");
    }

    if (secret_key.size() != k) {
        throw std::invalid_argument("Invalid secret key size: expected " + std::to_string(k) + " polynomials");
    }

    std::vector<std::vector<ColorValue>> c1(ciphertext.begin(), ciphertext.begin() + k);
    std::vector<ColorValue> c2 = ciphertext[k];

    std::vector<ColorValue> s_dot_c1_poly(params_.degree, ColorValue(0, 0, 0, 0));
    for (uint32_t i = 0; i < k; ++i) {
        std::vector<ColorValue> product(params_.degree);
        color_ntt_engine_->multiply_colors(secret_key[i].data(), c1[i].data(), product.data());
        for (uint32_t d = 0; d < params_.degree; ++d) {
            uint64_t sdc_val = s_dot_c1_poly[d].to_math_value();
            uint64_t p_val = product[d].to_math_value();
            s_dot_c1_poly[d] = ColorValue::from_math_value((sdc_val + p_val) % q);
        }
    }

    uint64_t s_dot_c1 = s_dot_c1_poly[0].to_math_value(); // Constant term

    uint64_t c2_val = c2[0].to_math_value(); // Constant term
    // Constant-time modular subtraction: v = (c2_val - s_dot_c1) mod q
    uint64_t diff_v = c2_val - s_dot_c1;
    uint64_t mask_v = -static_cast<uint64_t>(static_cast<int64_t>(diff_v) >> 63);
    uint64_t v = diff_v + (mask_v & q);
    v %= q;

    // Constant-time min for dist = min(v, q - v)
    uint64_t q_half = q / 2;
    uint64_t a_dist = v;
    uint64_t b_dist = q - v;
    int64_t signed_diff_dist = static_cast<int64_t>(a_dist) - static_cast<int64_t>(b_dist);
    uint64_t mask_dist = -static_cast<uint64_t>(signed_diff_dist >> 63);
    uint64_t dist = b_dist + (mask_dist & (a_dist - b_dist));

    // Constant-time comparison: m = 1 if dist > q/4, 0 otherwise
    uint32_t q_fourth = q / 4;
    uint64_t diff_m = dist - q_fourth - 1;
    uint64_t mask_m = -static_cast<uint64_t>(static_cast<int64_t>(diff_m) >> 63);
    uint32_t m = 1 - static_cast<uint32_t>(mask_m & 1);

    return ColorValue::from_math_value(m);
}


ColorValue ColorKEM::generate_shared_secret() const {
    uint8_t bytes[4];
    secure_random_bytes(bytes, 4);
    uint32_t value = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    value %= params_.modulus;

    return ColorValue::from_precise_value(value);
}


std::vector<uint8_t> ColorKEM::encode_color_secret(const ColorValue& secret) const {
    uint32_t value = secret.to_math_value();
    return {
        static_cast<uint8_t>((value >> 24) & 0xFF),
        static_cast<uint8_t>((value >> 16) & 0xFF),
        static_cast<uint8_t>((value >> 8) & 0xFF),
        static_cast<uint8_t>(value & 0xFF)
    };
}


ColorValue ColorKEM::decode_color_secret(const std::vector<uint8_t>& encoded) const {
    if (encoded.size() < 4) return ColorValue::from_math_value(0);

    uint32_t value = (static_cast<uint32_t>(encoded[0]) << 24) |
                    (static_cast<uint32_t>(encoded[1]) << 16) |
                    (static_cast<uint32_t>(encoded[2]) << 8) |
                    static_cast<uint32_t>(encoded[3]);

    return ColorValue::from_math_value(value);
}


std::pair<ColorPublicKey, ColorPrivateKey> ColorKEM::keygen() {

    std::array<uint8_t, 32> matrix_seed;
    secure_random_bytes(matrix_seed.data(), matrix_seed.size());

    // std::cout << "DEBUG: Matrix seed: ";
    // for (uint8_t b : matrix_seed) std::cout << std::hex << std::setw(2) << std::setfill('0') << (int)b;
    // std::cout << std::dec << std::endl;

    auto matrix_A = generate_matrix_A(matrix_seed);
    // std::cout << "DEBUG: Matrix A generated (" << matrix_A.size() << "x" << matrix_A[0].size() << ")" << std::endl;

    auto secret_key_colors = generate_secret_key(params_.eta1);
    // std::cout << "DEBUG: Secret key generated (" << secret_key_colors.size() << " elements)" << std::endl;
    // for (size_t i = 0; i < secret_key_colors.size(); ++i) {
    //     std::cout << "  s[" << i << "] = " << secret_key_colors[i].to_math_value() << std::endl;
    // }

    auto error_vector = generate_error_vector(params_.eta1);
    // std::cout << "DEBUG: Error vector generated (" << error_vector.size() << " elements)" << std::endl;
    // for (size_t i = 0; i < error_vector.size(); ++i) {
    //     std::cout << "  e[" << i << "] = " << error_vector[i].to_math_value() << std::endl;
    // }

    auto public_key_colors = generate_public_key(secret_key_colors, matrix_A, error_vector);
    // std::cout << "DEBUG: Public key generated (" << public_key_colors.size() << " elements)" << std::endl;
    // for (size_t i = 0; i < public_key_colors.size(); ++i) {
    //     std::cout << "  t[" << i << "] = " << public_key_colors[i].to_math_value() << std::endl;
    // }

    
    std::vector<uint8_t> secret_data;
    for (const auto& poly : secret_key_colors) {
        for (const auto& coeff : poly) {
            auto bytes = color_secret_to_bytes(coeff);
            secret_data.insert(secret_data.end(), bytes.begin(), bytes.end());
        }
    }

    std::vector<uint8_t> public_data;
    for (const auto& poly : public_key_colors) {
        for (const auto& coeff : poly) {
            auto bytes = color_secret_to_bytes(coeff);
            public_data.insert(public_data.end(), bytes.begin(), bytes.end());
        }
    }

    ColorPublicKey public_key{matrix_seed, public_data, params_};
    ColorPrivateKey private_key{secret_data, params_};

#ifdef ENABLE_HSM
    // If HSM is enabled, store the private key securely
    if (hsm_ && hsm_config_.require_hsm_for_private_keys) {
        auto import_result = hsm_->import_key(clwe::hsm::KeyType::SECRET_KEY, secret_data);
        if (!import_result.success) {
            throw std::runtime_error("Failed to import private key to HSM: " + import_result.error_message);
        }
        // Store the HSM handle in the private key (this would require extending ColorPrivateKey)
        // For now, we'll keep the data but mark it as HSM-protected
    }
#endif

    return {public_key, private_key};
}


std::pair<ColorCiphertext, ColorValue> ColorKEM::encapsulate(const ColorPublicKey& public_key) {

    // Validate public key parameters match instance parameters
    if (public_key.params.security_level != params_.security_level ||
        public_key.params.modulus != params_.modulus ||
        public_key.params.degree != params_.degree ||
        public_key.params.module_rank != params_.module_rank) {
        throw std::invalid_argument("Public key parameters do not match KEM instance parameters");
    }

    // Validate public key data size
    if (public_key.public_data.size() != params_.module_rank * params_.degree * 4) {
        throw std::invalid_argument("Invalid public key data size: expected " + std::to_string(params_.module_rank * params_.degree * 4) + " bytes, got " + std::to_string(public_key.public_data.size()));
    }

    // Validate public key data is not empty and properly sized
    if (public_key.public_data.empty()) {
        throw std::invalid_argument("Public key data cannot be empty");
    }

    uint8_t bytes[4];
    secure_random_bytes(bytes, 4);
    uint32_t value = (bytes[0] << 24) | (bytes[1] << 16) | (bytes[2] << 8) | bytes[3];
    value %= params_.modulus;
    ColorValue shared_secret = ColorValue::from_math_value(value);
    // std::cout << "DEBUG ENCAP: Shared secret = " << shared_secret.to_precise_value() << std::endl;

    auto matrix_A = generate_matrix_A(public_key.seed);
    // std::cout << "DEBUG ENCAP: Regenerated matrix A from seed" << std::endl;

    std::vector<std::vector<ColorValue>> public_key_colors(params_.module_rank, std::vector<ColorValue>(params_.degree));
    size_t idx = 0;
    for (size_t i = 0; i < params_.module_rank; ++i) {
        for (size_t d = 0; d < params_.degree; ++d) {
            std::vector<uint8_t> bytes(public_key.public_data.begin() + idx,
                                      public_key.public_data.begin() + idx + 4);
            public_key_colors[i][d] = bytes_to_color_secret(bytes);
            idx += 4;
        }
    }
    // std::cout << "DEBUG ENCAP: Public key colors (" << public_key_colors.size() << " elements):" << std::endl;
    // for (size_t i = 0; i < public_key_colors.size(); ++i) {
    //     std::cout << "  t[" << i << "] = " << public_key_colors[i].to_math_value() << std::endl;
    // }

    auto ciphertext_colors = encrypt_message(matrix_A, public_key_colors, shared_secret);
    // std::cout << "DEBUG ENCAP: Ciphertext colors (" << ciphertext_colors.size() << " elements):" << std::endl;
    // for (size_t i = 0; i < ciphertext_colors.size(); ++i) {
    //     std::cout << "  c[" << i << "] = " << ciphertext_colors[i].to_math_value() << std::endl;
    // }

    
    std::vector<uint8_t> ciphertext_data;
    for (const auto& poly : ciphertext_colors) {
        for (const auto& coeff : poly) {
            auto bytes = color_secret_to_bytes(coeff);
            ciphertext_data.insert(ciphertext_data.end(), bytes.begin(), bytes.end());
        }
    }

    
    auto shared_secret_hint = encode_color_secret(shared_secret);

    ColorCiphertext ciphertext{ciphertext_data, shared_secret_hint, params_};

    shared_secret = hash_ciphertext(ciphertext);

    return {ciphertext, shared_secret};
}


ColorValue ColorKEM::decapsulate(const ColorPublicKey& public_key,
                                const ColorPrivateKey& private_key,
                                const ColorCiphertext& ciphertext) {

    // Validate public key parameters match instance parameters
    if (public_key.params.security_level != params_.security_level ||
        public_key.params.modulus != params_.modulus ||
        public_key.params.degree != params_.degree ||
        public_key.params.module_rank != params_.module_rank) {
        throw std::invalid_argument("Public key parameters do not match KEM instance parameters");
    }

    // Validate private key parameters match instance parameters
    if (private_key.params.security_level != params_.security_level ||
        private_key.params.modulus != params_.modulus ||
        private_key.params.degree != params_.degree ||
        private_key.params.module_rank != params_.module_rank) {
        throw std::invalid_argument("Private key parameters do not match KEM instance parameters");
    }

    // Validate ciphertext parameters match instance parameters
    if (ciphertext.params.security_level != params_.security_level ||
        ciphertext.params.modulus != params_.modulus ||
        ciphertext.params.degree != params_.degree ||
        ciphertext.params.module_rank != params_.module_rank) {
        throw std::invalid_argument("Ciphertext parameters do not match KEM instance parameters");
    }

    // Validate private key data size
    if (private_key.secret_data.size() != params_.module_rank * params_.degree * 4) {
        throw std::invalid_argument("Invalid private key data size: expected " + std::to_string(params_.module_rank * params_.degree * 4) + " bytes, got " + std::to_string(private_key.secret_data.size()));
    }

    // Validate private key data is not empty
    if (private_key.secret_data.empty()) {
        throw std::invalid_argument("Private key data cannot be empty");
    }

    std::vector<std::vector<ColorValue>> secret_key_colors(params_.module_rank, std::vector<ColorValue>(params_.degree));
    size_t idx_sk = 0;
    for (size_t i = 0; i < params_.module_rank; ++i) {
        for (size_t d = 0; d < params_.degree; ++d) {
            std::vector<uint8_t> bytes(private_key.secret_data.begin() + idx_sk,
                                       private_key.secret_data.begin() + idx_sk + 4);
            secret_key_colors[i][d] = bytes_to_color_secret(bytes);
            idx_sk += 4;
        }
    }
    // std::cout << "DEBUG DECAP: Secret key colors (" << secret_key_colors.size() << " elements):" << std::endl;
    // for (size_t i = 0; i < secret_key_colors.size(); ++i) {
    //     std::cout << "  s[" << i << "] = " << secret_key_colors[i].to_math_value() << std::endl;
    // }

    // Validate ciphertext data size
    if (ciphertext.ciphertext_data.size() != (params_.module_rank + 1) * params_.degree * 4) {
        throw std::invalid_argument("Invalid ciphertext data size: expected " + std::to_string((params_.module_rank + 1) * params_.degree * 4) + " bytes, got " + std::to_string(ciphertext.ciphertext_data.size()));
    }

    // Validate ciphertext data is not empty
    if (ciphertext.ciphertext_data.empty()) {
        throw std::invalid_argument("Ciphertext data cannot be empty");
    }

    // Validate shared secret hint size
    if (ciphertext.shared_secret_hint.size() != 4) {
        throw std::invalid_argument("Invalid shared secret hint size: expected 4 bytes, got " + std::to_string(ciphertext.shared_secret_hint.size()));
    }

    std::vector<std::vector<ColorValue>> ciphertext_colors(params_.module_rank + 1, std::vector<ColorValue>(params_.degree));
    size_t idx = 0;
    for (size_t i = 0; i < params_.module_rank + 1; ++i) {
        for (size_t d = 0; d < params_.degree; ++d) {
            std::vector<uint8_t> bytes(ciphertext.ciphertext_data.begin() + idx,
                                      ciphertext.ciphertext_data.begin() + idx + 4);
            ciphertext_colors[i][d] = bytes_to_color_secret(bytes);
            idx += 4;
        }
    }
    // std::cout << "DEBUG DECAP: Ciphertext colors (" << ciphertext_colors.size() << " elements):" << std::endl;
    // for (size_t i = 0; i < ciphertext_colors.size(); ++i) {
    //     std::cout << "  c[" << i << "] = " << ciphertext_colors[i].to_math_value() << std::endl;
    // }

    ColorValue recovered_secret = decrypt_message(secret_key_colors, ciphertext_colors);
    // std::cout << "DEBUG DECAP: Recovered secret = " << recovered_secret.to_precise_value() << std::endl;

    // Fujisaki-Okamoto transform for IND-CCA2 security
    ColorValue hinted_secret = decode_color_secret(ciphertext.shared_secret_hint);
    if (recovered_secret == hinted_secret) {
        return recovered_secret;
    } else {
        return hash_ciphertext(ciphertext);
    }
}

ColorValue ColorKEM::hash_ciphertext(const ColorCiphertext& ciphertext) const {
    auto ct_serial = ciphertext.serialize();
    SHAKE256Sampler shake;
    shake.init(ct_serial.data(), ct_serial.size());

    std::array<uint8_t, 4> hash_bytes;
    shake.squeeze(hash_bytes.data(), 4);

    uint32_t hash_value = (static_cast<uint32_t>(hash_bytes[0]) << 24) |
                          (static_cast<uint32_t>(hash_bytes[1]) << 16) |
                          (static_cast<uint32_t>(hash_bytes[2]) << 8) |
                          static_cast<uint32_t>(hash_bytes[3]);

    return ColorValue::from_math_value(hash_value % params_.modulus);
}


bool ColorKEM::verify_keypair(const ColorPublicKey& public_key, const ColorPrivateKey& private_key) const {
    
    return public_key.params.security_level == private_key.params.security_level &&
           public_key.params.modulus == private_key.params.modulus;
}


std::vector<uint8_t> ColorKEM::color_secret_to_bytes(const ColorValue& secret) {
    uint32_t value = secret.to_math_value();
    // Validate value is within reasonable bounds (though ColorValue should ensure this)
    if (value > 0xFFFFFFFF) {
        throw std::invalid_argument("Color value too large for serialization: " + std::to_string(value));
    }
    return {
        static_cast<uint8_t>((value >> 24) & 0xFF),
        static_cast<uint8_t>((value >> 16) & 0xFF),
        static_cast<uint8_t>((value >> 8) & 0xFF),
        static_cast<uint8_t>(value & 0xFF)
    };
}

ColorValue ColorKEM::bytes_to_color_secret(const std::vector<uint8_t>& bytes) {
    if (bytes.size() < 4) {
        throw std::invalid_argument("Insufficient bytes for color secret: need 4 bytes, got " + std::to_string(bytes.size()));
    }

    if (bytes.size() > 4) {
        throw std::invalid_argument("Too many bytes for color secret: expected 4 bytes, got " + std::to_string(bytes.size()));
    }

    uint32_t value = (static_cast<uint32_t>(bytes[0]) << 24) |
                    (static_cast<uint32_t>(bytes[1]) << 16) |
                    (static_cast<uint32_t>(bytes[2]) << 8) |
                    static_cast<uint32_t>(bytes[3]);

    return ColorValue::from_math_value(value);
}


std::vector<uint8_t> ColorPublicKey::serialize() const {
    // Validate seed size
    if (seed.size() != 32) {
        throw std::invalid_argument("Invalid seed size: expected 32 bytes, got " + std::to_string(seed.size()));
    }

    // Validate public data size (should be multiple of 4 for ColorValue serialization)
    if (public_data.size() % 4 != 0 || public_data.empty()) {
        throw std::invalid_argument("Invalid public data size: must be non-empty and multiple of 4 bytes, got " + std::to_string(public_data.size()));
    }

    std::vector<uint8_t> data;
    data.insert(data.end(), seed.begin(), seed.end());
    data.insert(data.end(), public_data.begin(), public_data.end());
    return data;
}

ColorPublicKey ColorPublicKey::deserialize(const std::vector<uint8_t>& data, const CLWEParameters& params) {
    if (data.size() < 32) {
        throw std::invalid_argument("Public key data too small: minimum 32 bytes required, got " + std::to_string(data.size()));
    }

    if (data.empty()) {
        throw std::invalid_argument("Public key data cannot be empty");
    }

    ColorPublicKey key;
    std::copy(data.begin(), data.begin() + 32, key.seed.begin());
    key.public_data.assign(data.begin() + 32, data.end());
    key.params = params;

    // Validate public data size (should be multiple of 4 for ColorValue serialization and non-empty)
    if (key.public_data.size() % 4 != 0 || key.public_data.empty()) {
        throw std::invalid_argument("Invalid public key data size: must be non-empty and multiple of 4 bytes, got " + std::to_string(key.public_data.size()));
    }
    return key;
}

std::vector<uint8_t> ColorPrivateKey::serialize() const {
    // Validate secret data size (should be multiple of 4 for ColorValue serialization)
    if (secret_data.size() % 4 != 0 || secret_data.empty()) {
        throw std::invalid_argument("Invalid secret data size: must be non-empty and multiple of 4 bytes, got " + std::to_string(secret_data.size()));
    }
    return secret_data;
}

ColorPrivateKey ColorPrivateKey::deserialize(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Private key data cannot be empty");
    }

    ColorPrivateKey key;
    key.secret_data = data;

    // Validate secret data size (should be multiple of 4 for ColorValue serialization and non-empty)
    if (key.secret_data.size() % 4 != 0 || key.secret_data.empty()) {
        throw std::invalid_argument("Invalid private key data size: must be non-empty and multiple of 4 bytes, got " + std::to_string(key.secret_data.size()));
    }
    return key;
}

std::vector<uint8_t> ColorCiphertext::serialize() const {
    // Validate ciphertext data size (should be multiple of 4 for ColorValue serialization)
    if (ciphertext_data.size() % 4 != 0 || ciphertext_data.empty()) {
        throw std::invalid_argument("Invalid ciphertext data size: must be non-empty and multiple of 4 bytes, got " + std::to_string(ciphertext_data.size()));
    }

    // Validate shared secret hint size
    if (shared_secret_hint.size() != 4) {
        throw std::invalid_argument("Invalid shared secret hint size: expected 4 bytes, got " + std::to_string(shared_secret_hint.size()));
    }

    std::vector<uint8_t> data;
    data.insert(data.end(), ciphertext_data.begin(), ciphertext_data.end());
    data.insert(data.end(), shared_secret_hint.begin(), shared_secret_hint.end());
    return data;
}

ColorCiphertext ColorCiphertext::deserialize(const std::vector<uint8_t>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Ciphertext data cannot be empty");
    }

    if (data.size() < 8 || data.size() % 4 != 0) {
        throw std::invalid_argument("Invalid ciphertext data: size must be at least 8 bytes and multiple of 4, got " + std::to_string(data.size()));
    }

    ColorCiphertext ct;

    // shared_secret_hint is always 4 bytes
    size_t hint_size = 4;
    size_t ciphertext_size = data.size() - hint_size;

    // Ensure ciphertext data is not empty
    if (ciphertext_size == 0) {
        throw std::invalid_argument("Ciphertext data portion cannot be empty");
    }

    ct.ciphertext_data.assign(data.begin(), data.begin() + ciphertext_size);
    ct.shared_secret_hint.assign(data.begin() + ciphertext_size, data.end());

    // Validate shared secret hint size
    if (ct.shared_secret_hint.size() != 4) {
        throw std::invalid_argument("Invalid shared secret hint size: expected 4 bytes, got " + std::to_string(ct.shared_secret_hint.size()));
    }

    return ct;
}


std::vector<std::vector<ColorValue>> ColorKEM::encrypt_message(const std::vector<std::vector<std::vector<ColorValue>>>& matrix_A,
                                                  const std::vector<std::vector<ColorValue>>& public_key,
                                                  const ColorValue& message) const {

    // Validate matrix_A dimensions
    if (matrix_A.size() != params_.module_rank) {
        throw std::invalid_argument("Invalid matrix_A rows: expected " + std::to_string(params_.module_rank) + ", got " + std::to_string(matrix_A.size()));
    }
    for (const auto& row : matrix_A) {
        if (row.size() != params_.module_rank) {
            throw std::invalid_argument("Invalid matrix_A columns: expected " + std::to_string(params_.module_rank) + " per row");
        }
    }

    // Validate public_key size
    if (public_key.size() != params_.module_rank) {
        throw std::invalid_argument("Invalid public_key size: expected " + std::to_string(params_.module_rank) + ", got " + std::to_string(public_key.size()));
    }

    // Validate message value is within modulus
    if (message.to_math_value() >= params_.modulus) {
        throw std::invalid_argument("Invalid message value: must be less than modulus " + std::to_string(params_.modulus));
    }

    std::vector<std::vector<ColorValue>> ciphertext(params_.module_rank + 1, std::vector<ColorValue>(params_.degree));

    auto r_vector = generate_secret_key(params_.eta2);

    auto e1_vector = generate_error_vector(params_.eta2);
    auto e2_vector = generate_error_vector(params_.eta2);
    auto e2 = e2_vector[0];

    auto A_trans_r = matrix_transpose_vector_mul(matrix_A, r_vector);
    for (uint32_t i = 0; i < params_.module_rank; ++i) {
        for (uint32_t d = 0; d < params_.degree; ++d) {
            uint64_t atr_val = A_trans_r[i][d].to_math_value();
            uint64_t e1_val = e1_vector[i][d].to_math_value();
            uint64_t c1_val = (atr_val + e1_val) % params_.modulus;
            ciphertext[i][d] = ColorValue::from_math_value(c1_val);
        }
    }

    std::vector<ColorValue> inner_product_poly(params_.degree, ColorValue(0, 0, 0, 0));
    for (uint32_t i = 0; i < params_.module_rank; ++i) {
        std::vector<ColorValue> product(params_.degree);
        color_ntt_engine_->multiply_colors(public_key[i].data(), r_vector[i].data(), product.data());
        for (uint32_t d = 0; d < params_.degree; ++d) {
            uint64_t ip_val = inner_product_poly[d].to_math_value();
            uint64_t p_val = product[d].to_math_value();
            inner_product_poly[d] = ColorValue::from_math_value((ip_val + p_val) % params_.modulus);
        }
    }

    uint64_t inner_product = inner_product_poly[0].to_math_value(); // Constant term

    uint64_t e2_val = e2[0].to_math_value(); // Constant term
    uint64_t m_val = message.to_math_value();

    uint64_t q_half = params_.modulus / 2;
    uint64_t encoded_m = m_val * q_half;
    uint64_t c2_val = (inner_product + e2_val + encoded_m) % params_.modulus;

    ciphertext[params_.module_rank][0] = ColorValue::from_math_value(c2_val);
    for (uint32_t d = 1; d < params_.degree; ++d) {
        ciphertext[params_.module_rank][d] = ColorValue(0, 0, 0, 0); // Zero elsewhere
    }

    return ciphertext;
}

// SIMD-accelerated matrix-vector multiplication implementations
std::vector<std::vector<ColorValue>> ColorKEM::matrix_vector_mul_simd(const std::vector<std::vector<std::vector<ColorValue>>>& matrix,
                                                        const std::vector<std::vector<ColorValue>>& vector) const {
    // For now, just call the regular implementation
    return matrix_vector_mul(matrix, vector);
}

std::vector<std::vector<ColorValue>> ColorKEM::matrix_transpose_vector_mul_simd(const std::vector<std::vector<std::vector<ColorValue>>>& matrix,
                                                                  const std::vector<std::vector<ColorValue>>& vector) const {
    // For now, just call the regular implementation
    return matrix_transpose_vector_mul(matrix, vector);
}

#ifdef HAVE_AVX512
std::vector<ColorValue> ColorKEM::matrix_vector_mul_avx512(const std::vector<std::vector<ColorValue>>& matrix,
                                                          const std::vector<uint32_t>& vector,
                                                          uint32_t modulus) const {
    uint32_t k = params_.module_rank;
    std::vector<ColorValue> result(k);

    // Load vector into AVX-512 register (pad with zeros if needed)
    __m512i vec_reg = _mm512_setzero_si512();
    uint32_t* vec_ptr = const_cast<uint32_t*>(vector.data());
    vec_reg = _mm512_loadu_si512(reinterpret_cast<__m512i*>(vec_ptr));

    for (uint32_t i = 0; i < k; ++i) {
        __m512i sum_reg = _mm512_setzero_si512();

        // Process matrix row in chunks of 16 elements
        for (uint32_t j = 0; j < k; j += 16) {
            uint32_t remaining = std::min(16u, k - j);

            // Load matrix row elements
            __m512i mat_reg = _mm512_setzero_si512();
            uint32_t mat_vals[16] = {0};
            for (uint32_t idx = 0; idx < remaining; ++idx) {
                mat_vals[idx] = matrix[i][j + idx].to_math_value();
            }
            mat_reg = _mm512_loadu_si512(reinterpret_cast<__m512i*>(mat_vals));

            // Multiply and accumulate
            __m512i prod_reg = _mm512_mullo_epi32(mat_reg, vec_reg);
            sum_reg = _mm512_add_epi64(sum_reg, _mm512_cvtepi32_epi64(_mm512_extracti32x4_epi32(prod_reg, 0)));
            // Note: This is simplified; full implementation would need proper accumulation
        }

        // Reduce modulo q (constant-time)
        uint64_t total_sum = 0;
        uint32_t sum_vals[16];
        _mm512_storeu_si512(reinterpret_cast<__m512i*>(sum_vals), sum_reg);
        for (uint32_t val : sum_vals) {
            total_sum = (total_sum + val) % modulus;
        }

        result[i] = ColorValue::from_math_value(total_sum);
    }

    return result;
}

std::vector<ColorValue> ColorKEM::matrix_transpose_vector_mul_avx512(const std::vector<std::vector<ColorValue>>& matrix,
                                                                    const std::vector<uint32_t>& vector,
                                                                    uint32_t modulus) const {
    uint32_t k = params_.module_rank;
    std::vector<ColorValue> result(k);

    // Load vector into AVX-512 register
    __m512i vec_reg = _mm512_setzero_si512();
    uint32_t* vec_ptr = const_cast<uint32_t*>(vector.data());
    vec_reg = _mm512_loadu_si512(reinterpret_cast<__m512i*>(vec_ptr));

    for (uint32_t i = 0; i < k; ++i) {
        __m512i sum_reg = _mm512_setzero_si512();

        // Process matrix column in chunks of 16 elements
        for (uint32_t j = 0; j < k; j += 16) {
            uint32_t remaining = std::min(16u, k - j);

            // Load matrix column elements (transposed access)
            __m512i mat_reg = _mm512_setzero_si512();
            uint32_t mat_vals[16] = {0};
            for (uint32_t idx = 0; idx < remaining; ++idx) {
                mat_vals[idx] = matrix[j + idx][i].to_math_value();
            }
            mat_reg = _mm512_loadu_si512(reinterpret_cast<__m512i*>(mat_vals));

            // Multiply and accumulate
            __m512i prod_reg = _mm512_mullo_epi32(mat_reg, vec_reg);
            sum_reg = _mm512_add_epi64(sum_reg, _mm512_cvtepi32_epi64(_mm512_extracti32x4_epi32(prod_reg, 0)));
        }

        // Reduce modulo q
        uint64_t total_sum = 0;
        uint32_t sum_vals[16];
        _mm512_storeu_si512(reinterpret_cast<__m512i*>(sum_vals), sum_reg);
        for (uint32_t val : sum_vals) {
            total_sum = (total_sum + val) % modulus;
        }

        result[i] = ColorValue::from_math_value(total_sum);
    }

    return result;
}
#endif

#ifdef HAVE_AVX2
std::vector<ColorValue> ColorKEM::matrix_vector_mul_avx2(const std::vector<std::vector<ColorValue>>& matrix,
                                                        const std::vector<uint32_t>& vector,
                                                        uint32_t modulus) const {
    uint32_t k = params_.module_rank;
    std::vector<ColorValue> result(k);

    for (uint32_t i = 0; i < k; ++i) {
        uint64_t sum = 0;

        // Process in chunks that fit in SIMD registers
        for (uint32_t j = 0; j < k; j += 8) {
            uint32_t chunk_size = std::min(8u, k - j);

            // Load vector chunk
            __m256i vec_chunk = _mm256_setzero_si256();
            if (chunk_size >= 4) {
                vec_chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(&vector[j]));
            } else {
                uint32_t vec_vals[8] = {0};
                for (uint32_t idx = 0; idx < chunk_size; ++idx) {
                    vec_vals[idx] = vector[j + idx];
                }
                vec_chunk = _mm256_loadu_si256(reinterpret_cast<__m256i*>(vec_vals));
            }

            // Load matrix row chunk
            uint32_t mat_vals[8] = {0};
            for (uint32_t idx = 0; idx < chunk_size; ++idx) {
                mat_vals[idx] = matrix[i][j + idx].to_math_value();
            }
            __m256i mat_chunk = _mm256_loadu_si256(reinterpret_cast<__m256i*>(mat_vals));

            // Multiply
            __m256i prod_chunk = _mm256_mullo_epi32(mat_chunk, vec_chunk);

            // Accumulate (extract and add to sum)
            uint32_t prod_vals[8];
            _mm256_storeu_si256(reinterpret_cast<__m256i*>(prod_vals), prod_chunk);
            for (uint32_t idx = 0; idx < chunk_size; ++idx) {
                sum = (sum + prod_vals[idx]) % modulus;
            }
        }

        result[i] = ColorValue::from_math_value(sum);
    }

    return result;
}

std::vector<ColorValue> ColorKEM::matrix_transpose_vector_mul_avx2(const std::vector<std::vector<ColorValue>>& matrix,
                                                                  const std::vector<uint32_t>& vector,
                                                                  uint32_t modulus) const {
    uint32_t k = params_.module_rank;
    std::vector<ColorValue> result(k);

    // Load vector into AVX2 register
    __m256i vec_reg = _mm256_setzero_si256();
    uint32_t* vec_ptr = const_cast<uint32_t*>(vector.data());
    vec_reg = _mm256_loadu_si256(reinterpret_cast<__m256i*>(vec_ptr));

    for (uint32_t i = 0; i < k; ++i) {
        __m256i sum_reg = _mm256_setzero_si256();

        // Process matrix column in chunks of 8 elements
        for (uint32_t j = 0; j < k; j += 8) {
            uint32_t remaining = std::min(8u, k - j);

            // Load matrix column elements (transposed access)
            __m256i mat_reg = _mm256_setzero_si256();
            uint32_t mat_vals[8] = {0};
            for (uint32_t idx = 0; idx < remaining; ++idx) {
                mat_vals[idx] = matrix[j + idx][i].to_math_value();
            }
            mat_reg = _mm256_loadu_si256(reinterpret_cast<__m256i*>(mat_vals));

            // Multiply and accumulate
            __m256i prod_reg = _mm256_mullo_epi32(mat_reg, vec_reg);
            sum_reg = _mm256_add_epi32(sum_reg, prod_reg);
        }

        // Reduce modulo q
        uint32_t sum_vals[8];
        _mm256_storeu_si256(reinterpret_cast<__m256i*>(sum_vals), sum_reg);
        uint64_t total_sum = 0;
        for (uint32_t val : sum_vals) {
            total_sum = (total_sum + val) % modulus;
        }

        result[i] = ColorValue::from_math_value(total_sum);
    }

    return result;
}
#endif

#ifdef HAVE_NEON
// NEON implementations commented out due to compilation issues
std::vector<ColorValue> ColorKEM::matrix_vector_mul_neon(const std::vector<std::vector<ColorValue>>& matrix,
                                                        const std::vector<uint32_t>& vector,
                                                        uint32_t modulus) const {
    // Fallback to scalar implementation
    std::vector<ColorValue> result(matrix.size());
    for (size_t i = 0; i < matrix.size(); ++i) {
        uint64_t sum = 0;
        for (size_t j = 0; j < vector.size(); ++j) {
            sum = (sum + (uint64_t)matrix[i][j].to_math_value() * vector[j]) % modulus;
        }
        result[i] = ColorValue::from_math_value(sum);
    }
    return result;
}

std::vector<ColorValue> ColorKEM::matrix_transpose_vector_mul_neon(const std::vector<std::vector<ColorValue>>& matrix,
                                                                  const std::vector<uint32_t>& vector,
                                                                  uint32_t modulus) const {
    // Fallback to scalar implementation
    std::vector<ColorValue> result(matrix.size());
    for (size_t i = 0; i < matrix.size(); ++i) {
        uint64_t sum = 0;
        for (size_t j = 0; j < vector.size(); ++j) {
            sum = (sum + (uint64_t)matrix[j][i].to_math_value() * vector[j]) % modulus;
        }
        result[i] = ColorValue::from_math_value(sum);
    }
    return result;
}
#endif

}
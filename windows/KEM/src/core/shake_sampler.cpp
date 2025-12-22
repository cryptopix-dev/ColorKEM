#include "shake_sampler.hpp"
#include "utils.hpp"
#include <cstring>
#include <algorithm>
#include <random>
#include <stdexcept>
#ifndef _MSC_VER
#include <openssl/evp.h>
#endif

#ifdef _MSC_VER
#include <intrin.h>
#endif

namespace clwe {

SHAKE256Sampler::SHAKE256Sampler() {
#ifdef _MSC_VER
    shake256_init(&ctx_);
#else
    ctx_ = EVP_MD_CTX_new();
    if (!ctx_) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }
#endif
}

SHAKE256Sampler::~SHAKE256Sampler() {
#ifndef _MSC_VER
    if (ctx_) {
        EVP_MD_CTX_free(ctx_);
    }
#endif
}

void SHAKE256Sampler::reset() {
#ifdef _MSC_VER
    shake256_init(&ctx_);
#else
    EVP_MD_CTX_reset(ctx_);
#endif
}

void SHAKE256Sampler::init(const uint8_t* seed, size_t seed_len) {
#ifdef _MSC_VER
    shake256_init(&ctx_);
    shake_update(&ctx_, seed, seed_len);
    shake_xof(&ctx_);
#else
    reset();
    if (EVP_DigestInit_ex(ctx_, EVP_shake256(), NULL) != 1) {
        throw std::runtime_error("Failed to initialize SHAKE-256");
    }
    if (EVP_DigestUpdate(ctx_, seed, seed_len) != 1) {
        throw std::runtime_error("Failed to absorb seed into SHAKE-256");
    }
#endif
}

void SHAKE256Sampler::squeeze(uint8_t* out, size_t len) {
#ifdef _MSC_VER
    shake_out(&ctx_, out, len);
#else
    if (EVP_DigestSqueeze(ctx_, out, len) != 1) {
        throw std::runtime_error("Failed to squeeze from SHAKE-256");
    }
#endif
}

void SHAKE256Sampler::random_bytes(uint8_t* out, size_t len) {
    squeeze(out, len);
}

int32_t SHAKE256Sampler::sample_binomial_coefficient(uint32_t eta) {
    // Sample from centered binomial distribution B(2η, 0.5) - η
    // Count the number of 1s in 2η random bits, then subtract η
    uint32_t count_ones = 0;
    size_t num_bytes = (2 * eta + 7) / 8; // Enough bytes for 2η bits
    std::vector<uint8_t> bytes(num_bytes);
    random_bytes(bytes.data(), num_bytes);

    for (uint32_t i = 0; i < 2 * eta; ++i) {
        uint8_t byte = bytes[i / 8];
        uint8_t bit = (byte >> (i % 8)) & 1;
        count_ones += bit;
    }

    return static_cast<int32_t>(count_ones) - static_cast<int32_t>(eta);
}

void SHAKE256Sampler::sample_polynomial_binomial(uint32_t* coeffs, size_t degree,
                                                uint32_t eta, uint32_t modulus) {
    for (size_t i = 0; i < degree; ++i) {
        int32_t sample = sample_binomial_coefficient(eta);
        // Map to positive range: (sample mod modulus + modulus) mod modulus
        coeffs[i] = (sample % static_cast<int32_t>(modulus) + modulus) % modulus;
    }
}

void SHAKE256Sampler::sample_polynomial_binomial_batch(uint32_t** coeffs_batch, size_t count,
                                                     size_t degree, uint32_t eta, uint32_t modulus) {
    for (size_t poly = 0; poly < count; ++poly) {
        sample_polynomial_binomial(coeffs_batch[poly], degree, eta, modulus);
    }
}

void SHAKE256Sampler::sample_polynomial_binomial_batch_avx512(uint32_t** coeffs_batch, size_t count,
                                                             size_t degree, uint32_t eta, uint32_t modulus) {
    // For now, fall back to scalar implementation
    // In production, this would use AVX-512 instructions for parallel sampling
    sample_polynomial_binomial_batch(coeffs_batch, count, degree, eta, modulus);
}

uint32_t SHAKE256Sampler::sample_uniform(uint32_t modulus) {
    // Sample uniformly from [0, modulus)
    // Use rejection sampling for uniform distribution
#ifdef _MSC_VER
    unsigned long index;
    _BitScanReverse(&index, modulus - 1);
    uint32_t mask = (1U << (index + 1)) - 1;
#else
    uint32_t mask = (1U << (32 - __builtin_clz(modulus - 1))) - 1;
#endif

    while (true) {
        uint8_t bytes[4];
        random_bytes(bytes, 4);

        uint32_t sample = (bytes[0] << 24) | (bytes[1] << 16) |
                         (bytes[2] << 8) | bytes[3];
        sample &= mask;

        if (sample < modulus) {
            return sample;
        }
    }
}

void SHAKE256Sampler::sample_polynomial_uniform(uint32_t* coeffs, size_t degree, uint32_t modulus) {
    for (size_t i = 0; i < degree; ++i) {
        coeffs[i] = sample_uniform(modulus);
    }
}

// SHAKE128Sampler implementation for Kyber matrix generation
SHAKE128Sampler::SHAKE128Sampler() {
#ifdef _MSC_VER
    shake128_init(&ctx_);
#else
    ctx_ = EVP_MD_CTX_new();
    if (!ctx_) {
        throw std::runtime_error("Failed to create EVP_MD_CTX for SHAKE-128");
    }
#endif
}

SHAKE128Sampler::~SHAKE128Sampler() {
#ifndef _MSC_VER
    if (ctx_) {
        EVP_MD_CTX_free(ctx_);
    }
#endif
}

void SHAKE128Sampler::reset() {
#ifdef _MSC_VER
    shake128_init(&ctx_);
#else
    EVP_MD_CTX_reset(ctx_);
#endif
}

void SHAKE128Sampler::init(const uint8_t* seed, size_t seed_len) {
#ifdef _MSC_VER
    shake128_init(&ctx_);
    shake_update(&ctx_, seed, seed_len);
    shake_xof(&ctx_);
#else
    reset();
    if (EVP_DigestInit_ex(ctx_, EVP_shake128(), NULL) != 1) {
        throw std::runtime_error("Failed to initialize SHAKE-128");
    }
    if (EVP_DigestUpdate(ctx_, seed, seed_len) != 1) {
        throw std::runtime_error("Failed to absorb seed into SHAKE-128");
    }
#endif
}

void SHAKE128Sampler::squeeze(uint8_t* out, size_t len) {
#ifdef _MSC_VER
    shake_out(&ctx_, out, len);
#else
    if (EVP_DigestSqueeze(ctx_, out, len) != 1) {
        throw std::runtime_error("Failed to squeeze from SHAKE-128");
    }
#endif
}

} // namespace clwe
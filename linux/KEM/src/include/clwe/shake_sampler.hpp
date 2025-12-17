/**
 * @file shake_sampler.hpp
 * @brief SHAKE-based cryptographic samplers for lattice-based cryptography
 *
 * This header provides SHAKE128 and SHAKE256-based pseudorandom number
 * generators optimized for lattice-based cryptographic operations. These
 * samplers are used for generating random polynomials, matrix elements,
 * and other cryptographic values in ML-KEM and similar schemes.
 *
 * SHAKE128 is used for deterministic matrix generation (public operation),
 * while SHAKE256 is used for secret sampling operations that require
 * higher security margins.
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 *
 * @see https://doi.org/10.6028/NIST.FIPS.202 (SHAKE specification)
 * @see https://doi.org/10.6028/NIST.FIPS.203 (ML-KEM sampling)
 */

#ifndef SHAKE_SAMPLER_HPP
#define SHAKE_SAMPLER_HPP

#include <cstdint>
#include <vector>
#include <array>
#include <openssl/evp.h>

namespace clwe {

/**
 * @brief SHAKE-128 based sampler for public cryptographic operations
 *
 * Provides deterministic pseudorandom output using SHAKE-128, suitable for
 * public operations like matrix generation where the output needs to be
 * reproducible from a seed but doesn't contain sensitive information.
 *
 * Used primarily for generating the public matrix A in lattice-based schemes.
 */
class SHAKE128Sampler {
private:
    EVP_MD_CTX* ctx_;              /**< OpenSSL EVP context for SHAKE-128 */
    std::vector<uint8_t> output_buffer_;  /**< Internal buffer for squeezed output */
    size_t output_pos_ = 0;        /**< Current position in output buffer */

    /** @brief Reset internal state */
    void reset();

public:
    /**
     * @brief Construct SHAKE-128 sampler
     *
     * Initializes the OpenSSL EVP context for SHAKE-128 operations.
     *
     * @throws std::runtime_error If OpenSSL context initialization fails
     */
    SHAKE128Sampler();

    /**
     * @brief Destroy SHAKE-128 sampler
     *
     * Properly cleans up OpenSSL context and erases sensitive data.
     */
    ~SHAKE128Sampler();

    /**
     * @brief Initialize sampler with seed
     *
     * Absorbs the seed into the SHAKE-128 sponge, preparing for squeezing.
     *
     * @param seed Seed bytes for initialization
     * @param seed_len Length of seed in bytes
     */
    void init(const uint8_t* seed, size_t seed_len);

    /**
     * @brief Squeeze pseudorandom bytes from SHAKE-128
     *
     * Extracts pseudorandom output from the SHAKE-128 sponge.
     *
     * @param out Output buffer for squeezed bytes
     * @param len Number of bytes to squeeze
     */
    void squeeze(uint8_t* out, size_t len);
};

/**
 * @brief SHAKE-256 based sampler for cryptographic random number generation
 *
 * Provides high-security pseudorandom sampling for lattice-based cryptography.
 * SHAKE-256 is used for all secret operations including key generation,
 * error sampling, and nonce generation in ML-KEM and similar schemes.
 *
 * Features:
 * - Binomial distribution sampling for "small" polynomials
 * - Uniform distribution sampling for matrix elements
 * - Batch sampling for improved performance
 * - AVX-512 acceleration for high-throughput applications
 * - Cryptographically secure random byte generation
 */
class SHAKE256Sampler {
private:
    EVP_MD_CTX* ctx_;              /**< OpenSSL EVP context for SHAKE-256 */
    std::vector<uint8_t> output_buffer_;  /**< Internal buffer for squeezed output */
    size_t output_pos_ = 0;        /**< Current position in output buffer */

    /** @brief Reset internal state */
    void reset();

public:
    /**
     * @brief Construct SHAKE-256 sampler
     *
     * Initializes the OpenSSL EVP context for SHAKE-256 operations.
     *
     * @throws std::runtime_error If OpenSSL context initialization fails
     */
    SHAKE256Sampler();

    /**
     * @brief Destroy SHAKE-256 sampler
     *
     * Properly cleans up OpenSSL context and erases sensitive data.
     */
    ~SHAKE256Sampler();

    /**
     * @brief Initialize sampler with seed
     *
     * Absorbs the seed into the SHAKE-256 sponge, preparing for sampling operations.
     *
     * @param seed Seed bytes for initialization (cryptographically random)
     * @param seed_len Length of seed in bytes
     */
    void init(const uint8_t* seed, size_t seed_len);

    /**
     * @brief Sample single coefficient from binomial distribution
     *
     * Samples a single coefficient from the centered binomial distribution B_2η.
     * Used for generating individual small coefficients in polynomials.
     *
     * @param eta Binomial parameter (η), typically 2 or 3
     * @return int32_t Sampled coefficient in range [-η, η]
     */
    int32_t sample_binomial_coefficient(uint32_t eta);

    /**
     * @brief Sample polynomial from binomial distribution
     *
     * Fills a polynomial with coefficients sampled from the binomial distribution.
     * This is the primary sampling function for secret key and error polynomials.
     *
     * @param coeffs Output array for polynomial coefficients
     * @param degree Degree of the polynomial (n)
     * @param eta Binomial parameter (η)
     * @param modulus Prime modulus q (for coefficient reduction)
     */
    void sample_polynomial_binomial(uint32_t* coeffs, size_t degree, uint32_t eta, uint32_t modulus);

    /**
     * @brief Batch sampling for improved performance
     *
     * Samples multiple polynomials simultaneously for efficiency in key generation
     * and encryption operations.
     *
     * @param coeffs_batch Array of pointers to coefficient arrays
     * @param count Number of polynomials to sample
     * @param degree Degree of each polynomial
     * @param eta Binomial parameter
     * @param modulus Prime modulus q
     */
    void sample_polynomial_binomial_batch(uint32_t** coeffs_batch, size_t count,
                                        size_t degree, uint32_t eta, uint32_t modulus);

    /**
     * @brief AVX-512 accelerated batch sampling
     *
     * Vectorized batch sampling using AVX-512 instructions for maximum performance
     * on supported hardware. Automatically falls back to scalar implementation
     * if AVX-512 is not available.
     *
     * @param coeffs_batch Array of pointers to coefficient arrays
     * @param count Number of polynomials to sample
     * @param degree Degree of each polynomial
     * @param eta Binomial parameter
     * @param modulus Prime modulus q
     */
    void sample_polynomial_binomial_batch_avx512(uint32_t** coeffs_batch, size_t count,
                                                size_t degree, uint32_t eta, uint32_t modulus);

    /**
     * @brief Sample from uniform distribution
     *
     * Samples a single value uniformly from [0, modulus).
     *
     * @param modulus Upper bound (exclusive)
     * @return uint32_t Uniform random value in [0, modulus)
     */
    uint32_t sample_uniform(uint32_t modulus);

    /**
     * @brief Sample polynomial from uniform distribution
     *
     * Fills a polynomial with coefficients sampled uniformly from [0, modulus).
     * Used for generating random matrix elements.
     *
     * @param coeffs Output array for polynomial coefficients
     * @param degree Degree of the polynomial
     * @param modulus Prime modulus q
     */
    void sample_polynomial_uniform(uint32_t* coeffs, size_t degree, uint32_t modulus);

    /**
     * @brief Generate cryptographically secure random bytes
     *
     * Produces high-quality random bytes suitable for cryptographic use.
     * Uses the full security of SHAKE-256 for randomness generation.
     *
     * @param out Output buffer for random bytes
     * @param len Number of bytes to generate
     */
    void random_bytes(uint8_t* out, size_t len);
};

} // namespace clwe

#endif // SHAKE_SAMPLER_HPP
/**
 * @file sampling.hpp
 * @brief Polynomial sampling functions for lattice-based cryptography
 *
 * This header provides functions for sampling random polynomials from various
 * distributions, primarily the binomial distribution used in lattice-based
 * cryptographic schemes like ML-KEM. The sampling functions use SHAKE256
 * as a cryptographically secure pseudorandom number generator.
 *
 * The binomial distribution sampling is used to generate "small" polynomials
 * with coefficients in the range [-η, η], which is essential for the security
 * of lattice-based encryption schemes.
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 *
 * @see SHAKE256Sampler for the underlying PRNG
 * @see https://doi.org/10.6028/NIST.FIPS.203 for ML-KEM sampling requirements
 */

#ifndef SAMPLING_HPP
#define SAMPLING_HPP

#include <cstdint>

// Forward declaration for SHAKE256Sampler
namespace clwe {
class SHAKE256Sampler;
}

namespace clwe {

/**
 * @brief Sample a single polynomial from binomial distribution
 *
 * Samples coefficients for a polynomial from the centered binomial distribution
 * B_2η, where each coefficient is the difference of two independent binomial
 * samples. This produces coefficients in the range [-η, η].
 *
 * The sampling uses SHAKE256 with a random seed for cryptographic security.
 *
 * @param coeffs Output array for polynomial coefficients (must be pre-allocated)
 * @param degree Degree of the polynomial (n)
 * @param eta Binomial parameter (η), typically 2 or 3 for ML-KEM
 * @param modulus Prime modulus q (used for coefficient reduction)
 *
 * @note Coefficients are reduced modulo modulus after sampling
 * @note Uses cryptographically secure random seed generation
 */
void sample_polynomial_binomial(uint32_t* coeffs, uint32_t degree, uint32_t eta, uint32_t modulus);

/**
 * @brief Sample a batch of polynomials from binomial distribution
 *
 * Efficiently samples multiple polynomials simultaneously. This is optimized
 * for key generation and encryption operations that require multiple small
 * polynomials.
 *
 * @param coeffs_batch Array of pointers to coefficient arrays (batch_size elements)
 * @param batch_size Number of polynomials to sample
 * @param degree Degree of each polynomial (n)
 * @param eta Binomial parameter (η)
 * @param modulus Prime modulus q
 *
 * @note Each polynomial in the batch is sampled independently
 * @note All coefficient arrays must be pre-allocated with size degree
 */
void sample_polynomial_binomial_batch(uint32_t** coeffs_batch, uint32_t batch_size,
                                     uint32_t degree, uint32_t eta, uint32_t modulus);

/**
 * @brief AVX-512 accelerated batch sampling (when available)
 *
 * Vectorized version of batch sampling using AVX-512 instructions for
 * improved performance on supported hardware. Falls back to scalar
 * implementation if AVX-512 is not available.
 *
 * @param coeffs_batch Array of pointers to coefficient arrays
 * @param batch_size Number of polynomials to sample
 * @param degree Degree of each polynomial (n)
 * @param eta Binomial parameter (η)
 * @param modulus Prime modulus q
 *
 * @note Automatically detects AVX-512 support at runtime
 * @note Performance improvement depends on batch_size and degree
 */
void sample_polynomial_binomial_batch_avx512(uint32_t** coeffs_batch, uint32_t batch_size,
                                            uint32_t degree, uint32_t eta, uint32_t modulus);

} // namespace clwe

#endif // SAMPLING_HPP
/**
 * @file clwe.hpp
 * @brief Core CLWE (Color Learning With Errors) definitions and parameters
 *
 * This header provides the fundamental types, constants, and parameter
 * structures used throughout the ColorKEM implementation. It defines
 * the cryptographic parameters, error codes, and utility functions.
 *
 * The CLWE scheme is mathematically equivalent to the standard Learning
 * With Errors (LWE) problem but uses color values for coefficient representation,
 * enabling visual interpretation of cryptographic operations.
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef CLWE_HPP
#define CLWE_HPP

#include <cstdint>
#include <stdexcept>
#include <string>

// Forward declarations
struct CLWEParameters;

/**
 * @brief Main CLWE namespace
 *
 * Contains all ColorKEM-related classes, functions, constants, and type definitions.
 * The namespace encapsulates the entire ColorKEM cryptographic library.
 */
namespace clwe {

/** @brief Current version of the ColorKEM library */
const std::string VERSION = "1.0.0";

/**
 * @brief Cryptographic parameters for CLWE operations
 *
 * This structure defines all the parameters needed for ColorKEM cryptographic operations.
 * It includes security level, ring dimension, module rank, modulus, and noise parameters.
 *
 * The parameters are designed to be compatible with NIST-standard ML-KEM (FIPS 203)
 * while supporting the color-based coefficient representation.
 *
 * Key Parameters:
 * - **security_level**: Target security level (512, 768, 1024 bits)
 * - **degree**: Ring dimension n (power of 2, typically 256)
 * - **module_rank**: Module rank k (2, 3, or 4 for ML-KEM levels)
 * - **modulus**: Prime modulus q (3329 for ML-KEM)
 * - **eta1**: Noise parameter for key generation
 * - **eta2**: Noise parameter for encryption
 *
 * @note All parameters are validated during construction.
 * @see https://doi.org/10.6028/NIST.FIPS.203 for ML-KEM parameter details
 */
struct CLWEParameters {
    uint32_t security_level;  // Security level (512, 768, 1024)
    uint32_t degree;          // Ring degree (power of 2)
    uint32_t module_rank;     // Module rank k
    uint32_t modulus;         // Prime modulus q
    uint32_t eta1;           // Binomial distribution parameter for key generation
    uint32_t eta2;           // Binomial distribution parameter for encryption

    /**
     * @brief Construct CLWE parameters with standard ML-KEM settings
     *
     * Creates parameters based on the specified security level using
     * NIST-standard ML-KEM parameter sets.
     *
     * @param sec_level Security level (512, 768, or 1024). Defaults to 512.
     *
     * @throws std::invalid_argument If security level is not supported
     *
     * @note Parameter validation is performed automatically.
     */
    CLWEParameters(uint32_t sec_level = 512)
        : security_level(sec_level), degree(256), module_rank(2),
          modulus(3329), eta1(3), eta2(2) {  // ML-KEM modulus: 2^12 + 1
        // Set parameters based on security level (ML-KEM standard)
        switch (sec_level) {
            case 512:  // ML-KEM-512
                degree = 256;
                module_rank = 2;
                modulus = 3329;
                eta1 = 3;
                eta2 = 2;
                break;
            case 768:  // ML-KEM-768
                degree = 256;
                module_rank = 3;
                modulus = 3329;
                eta1 = 2;
                eta2 = 2;
                break;
            case 1024:  // ML-KEM-1024
                degree = 256;
                module_rank = 4;
                modulus = 3329;
                eta1 = 2;
                eta2 = 2;
                break;
            default:
                // Keep defaults but validation will fail
                break;
        }
        validate();
    }

    /**
     * @brief Construct CLWE parameters with custom values
     *
     * Allows creation of custom parameter sets for research or specialized applications.
     * All parameters are validated during construction.
     *
     * @param sec_level Target security level (for identification only)
     * @param deg Ring degree n (must be power of 2)
     * @param rank Module rank k
     * @param mod Prime modulus q
     * @param e1 Noise parameter eta1 for key generation
     * @param e2 Noise parameter eta2 for encryption
     *
     * @throws std::invalid_argument If any parameter is invalid
     *
     * @warning Custom parameters may not provide the expected security level.
     * Use standard ML-KEM parameters for production applications.
     */
    CLWEParameters(uint32_t sec_level, uint32_t deg, uint32_t rank, uint32_t mod, uint32_t e1, uint32_t e2)
        : security_level(sec_level), degree(deg), module_rank(rank),
          modulus(mod), eta1(e1), eta2(e2) {
        validate();
    }

    /**
     * @brief Validate parameter values
     *
     * Performs comprehensive validation of all parameter values to ensure
     * they meet cryptographic requirements and implementation constraints.
     *
     * Validation checks:
     * - Security level must be 512, 768, or 1024
     * - Degree must be power of 2 between 1 and 8192
     * - Module rank must be between 1 and 16
     * - Modulus must be prime between 256 and 65536
     * - Noise parameters must be between 1 and 16
     *
     * @throws std::invalid_argument If any parameter validation fails
     *
     * @note This method is called automatically during construction.
     */
    void validate() const {
        // Validate security level
        if (security_level != 512 && security_level != 768 && security_level != 1024) {
            throw std::invalid_argument("Invalid security level: must be 512, 768, or 1024");
        }

        // Validate degree: must be power of 2 and reasonable
        if (degree == 0 || (degree & (degree - 1)) != 0 || degree > 8192) {
            throw std::invalid_argument("Invalid degree: must be a power of 2 between 1 and 8192");
        }

        // Validate module rank: positive and reasonable
        if (module_rank == 0 || module_rank > 16) {
            throw std::invalid_argument("Invalid module rank: must be between 1 and 16");
        }

        // Validate modulus: must be prime and appropriate size
        if (!is_prime(modulus) || modulus < 256 || modulus > 65536) {
            throw std::invalid_argument("Invalid modulus: must be a prime between 256 and 65536");
        }

        // Validate eta1: positive and reasonable
        if (eta1 == 0 || eta1 > 16) {
            throw std::invalid_argument("Invalid eta1: must be between 1 and 16");
        }

        // Validate eta2: positive and reasonable
        if (eta2 == 0 || eta2 > 16) {
            throw std::invalid_argument("Invalid eta2: must be between 1 and 16");
        }
    }

private:
    // Helper function to check if a number is prime
    static bool is_prime(uint32_t n) {
        if (n <= 1) return false;
        if (n <= 3) return true;
        if (n % 2 == 0 || n % 3 == 0) return false;
        for (uint32_t i = 5; i * i <= n; i += 6) {
            if (n % i == 0 || n % (i + 2) == 0) return false;
        }
        return true;
    }
};

/**
 * @brief Error codes for CLWE operations
 *
 * Enumerates all possible error conditions that can occur during
 * ColorKEM cryptographic operations. These codes provide detailed
 * information about the type of failure that occurred.
 */
enum class CLWEError {
    SUCCESS = 0,                    /**< Operation completed successfully */
    INVALID_PARAMETERS = 1,         /**< Invalid cryptographic parameters provided */
    MEMORY_ALLOCATION_FAILED = 2,   /**< Failed to allocate required memory */
    AVX_NOT_SUPPORTED = 3,          /**< AVX instructions not supported on this platform */
    INVALID_KEY = 4,                /**< Provided key is malformed or invalid */
    VERIFICATION_FAILED = 5,        /**< Cryptographic verification failed */
    UNKNOWN_ERROR = 6               /**< An unspecified error occurred */
};

/**
 * @brief Get human-readable error message for CLWE error code
 *
 * Converts an error code into a descriptive string that can be
 * displayed to users or logged for debugging purposes.
 *
 * @param error The error code to convert
 * @return std::string Human-readable description of the error
 *
 * @note This function is thread-safe and can be called from any context.
 */
std::string get_error_message(CLWEError error);

} // namespace clwe

#endif // CLWE_HPP
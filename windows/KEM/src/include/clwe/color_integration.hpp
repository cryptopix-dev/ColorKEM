#ifndef CLWE_COLOR_INTEGRATION_HPP
#define CLWE_COLOR_INTEGRATION_HPP

#include <vector>
#include <cstdint>
#include "color_value.hpp"

namespace clwe {

/**
 * @brief Color integration module for ColorKEM
 *
 * Provides functions to encode polynomials, vectors, KEM keys and ciphertexts into RGB pixel arrays
 * compatible with WebP format for visualization and storage, and decode back. Coefficients are
 * ColorValue (32-bit) packed into RGB pixels where each ColorValue uses 4 RGB pixels (12 bytes)
 * to preserve full 32-bit precision. KEM keys are serialized as byte arrays where each 4 bytes
 * represent a ColorValue coefficient, with the lower 8 bits packed into RGB pixel format.
 */

/**
 * @brief Encode a single polynomial into RGB color data
 *
 * Each ColorValue (32-bit) is packed into 4 RGB pixels (12 bytes) to preserve
 * full precision. Each pixel contains parts of the 32-bit value.
 *
 * @param poly The polynomial coefficients as ColorValue
 * @return RGB color data as byte array (12 bytes per ColorValue)
 */
std::vector<uint8_t> encode_polynomial_as_colors(const std::vector<ColorValue>& poly);

/**
 * @brief Decode RGB color data into a single polynomial
 *
 * Unpacks RGB pixel data back into ColorValue, where each ColorValue uses
 * 4 RGB pixels (12 bytes).
 *
 * @param color_data RGB color data
 * @return Polynomial coefficients as ColorValue
 */
std::vector<ColorValue> decode_colors_to_polynomial(const std::vector<uint8_t>& color_data);

/**
 * @brief Encode a vector of polynomials into RGB color data
 *
 * Polynomials are flattened and coefficients are packed into RGB pixels,
 * with each ColorValue using 4 RGB pixels.
 *
 * @param poly_vector Vector of polynomials
 * @return RGB color data as byte array
 */
std::vector<uint8_t> encode_polynomial_vector_as_colors(const std::vector<std::vector<ColorValue>>& poly_vector);

/**
 * @brief Decode RGB color data into a vector of polynomials
 *
 * @param color_data RGB color data where each ColorValue uses 4 RGB pixels
 * @param k Number of polynomials in the vector
 * @param n Degree of each polynomial
 * @return Vector of polynomials
 */
std::vector<std::vector<ColorValue>> decode_colors_to_polynomial_vector(const std::vector<uint8_t>& color_data, uint32_t k, uint32_t n);

// Additional functions for WebP integration and precision handling
std::vector<uint8_t> encode_polynomial_vector_as_colors_compressed(const std::vector<std::vector<ColorValue>>& poly_vector, uint32_t modulus);
std::vector<std::vector<ColorValue>> decode_colors_to_polynomial_vector_compressed(const std::vector<uint8_t>& color_data, uint32_t k, uint32_t n, uint32_t modulus);
std::vector<uint8_t> convert_compressed_to_color_format(const std::vector<uint8_t>& compressed_data, uint32_t k, uint32_t n, uint32_t modulus);
std::vector<uint8_t> encode_polynomial_vector_as_colors_auto(const std::vector<std::vector<ColorValue>>& poly_vector, uint32_t modulus);
std::vector<uint8_t> encode_polynomial_vector_as_colors_huffman(const std::vector<std::vector<ColorValue>>& poly_vector, uint32_t modulus);
std::vector<uint8_t> generate_color_representation_from_compressed(const std::vector<uint8_t>& compressed_data, uint32_t k, uint32_t n, uint32_t modulus);
std::vector<uint8_t> compress_with_color_support(const std::vector<std::vector<ColorValue>>& poly_vector, uint32_t modulus, bool enable_color_metadata = true);
std::vector<std::vector<ColorValue>> decompress_with_color_support(const std::vector<uint8_t>& dual_format_data, uint32_t& out_k, uint32_t& out_n, uint32_t& out_modulus);
std::vector<uint8_t> generate_color_from_dual_format(const std::vector<uint8_t>& dual_format_data);
std::vector<uint8_t> encode_polynomial_vector_with_color_integration(const std::vector<std::vector<ColorValue>>& poly_vector, uint32_t modulus, bool enable_on_demand_color = true);
std::vector<std::vector<ColorValue>> decode_polynomial_vector_with_color_integration(const std::vector<uint8_t>& color_integrated_data, uint32_t modulus);

/**
 * @brief Encode a ColorKEM public key into RGB color data
 *
 * The public key data (serialized ColorValue coefficients as 4-byte values) are decoded
 * into uint32_t coefficients, and the lower 8 bits of each coefficient are packed into
 * RGB pixel format compatible with WebP.
 *
 * @param public_key_data Serialized public key data (vector of uint8_t)
 * @return RGB color data as byte array (1 byte per coefficient, arranged for RGB pixels)
 */
std::vector<uint8_t> encode_color_kem_public_key_as_colors(const std::vector<uint8_t>& public_key_data);

/**
 * @brief Decode RGB color data back into ColorKEM public key data
 *
 * Unpacks RGB pixel data back into the serialized public key format,
 * reconstructing the 4-byte coefficient representation from the lower 8 bits.
 *
 * @param color_data RGB color data
 * @param expected_size Expected size of the public key data in bytes
 * @return Serialized public key data
 */
std::vector<uint8_t> decode_colors_to_color_kem_public_key(const std::vector<uint8_t>& color_data, size_t expected_size);

/**
 * @brief Encode a ColorKEM private key into RGB color data
 *
 * The private key data (serialized ColorValue coefficients) are packed into RGB pixels
 * where each pixel contains 3 bytes (R, G, B channels).
 *
 * @param private_key_data Serialized private key data (vector of uint8_t)
 * @return RGB color data as byte array (3 bytes per pixel)
 */
std::vector<uint8_t> encode_color_kem_private_key_as_colors(const std::vector<uint8_t>& private_key_data);

/**
 * @brief Decode RGB color data back into ColorKEM private key data
 *
 * Unpacks RGB pixel data back into the serialized private key format.
 *
 * @param color_data RGB color data
 * @param expected_size Expected size of the private key data in bytes
 * @return Serialized private key data
 */
std::vector<uint8_t> decode_colors_to_color_kem_private_key(const std::vector<uint8_t>& color_data, size_t expected_size);

/**
 * @brief Encode a ColorKEM ciphertext into RGB color data
 *
 * The ciphertext data (serialized ColorValue coefficients) are packed into RGB pixels.
 *
 * @param ciphertext_data Serialized ciphertext data (vector of uint8_t)
 * @return RGB color data as byte array (3 bytes per pixel)
 */
std::vector<uint8_t> encode_color_kem_ciphertext_as_colors(const std::vector<uint8_t>& ciphertext_data);

/**
 * @brief Decode RGB color data back into ColorKEM ciphertext data
 *
 * Unpacks RGB pixel data back into the serialized ciphertext format.
 *
 * @param color_data RGB color data
 * @param expected_size Expected size of the ciphertext data in bytes
 * @return Serialized ciphertext data
 */
std::vector<uint8_t> decode_colors_to_color_kem_ciphertext(const std::vector<uint8_t>& color_data, size_t expected_size);

// Compression functions for color integration
std::vector<uint8_t> encode_color_kem_key_as_colors_compressed(const std::vector<uint8_t>& key_data);
std::vector<uint8_t> decode_colors_to_color_kem_key_compressed(const std::vector<uint8_t>& color_data, size_t expected_size);
std::vector<uint8_t> convert_compressed_key_to_color_format(const std::vector<uint8_t>& compressed_data, size_t expected_size);
std::vector<uint8_t> encode_color_kem_key_as_colors_auto(const std::vector<uint8_t>& key_data);

// Advanced color integration functions for KEM
std::vector<uint8_t> generate_color_representation_from_compressed_key(const std::vector<uint8_t>& compressed_data, size_t expected_size);
std::vector<uint8_t> compress_kem_key_with_color_support(const std::vector<uint8_t>& key_data, bool enable_color_metadata = true);
std::vector<uint8_t> decompress_kem_key_with_color_support(const std::vector<uint8_t>& dual_format_data, size_t& out_size);
std::vector<uint8_t> generate_color_from_kem_dual_format(const std::vector<uint8_t>& dual_format_data);
std::vector<uint8_t> encode_kem_key_with_color_integration(const std::vector<uint8_t>& key_data, bool enable_on_demand_color = true);
std::vector<uint8_t> decode_kem_key_with_color_integration(const std::vector<uint8_t>& color_integrated_data);

} // namespace clwe

#endif // CLWE_COLOR_INTEGRATION_HPP
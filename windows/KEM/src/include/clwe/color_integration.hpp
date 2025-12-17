#ifndef CLWE_COLOR_INTEGRATION_HPP
#define CLWE_COLOR_INTEGRATION_HPP

#include <vector>
#include <cstdint>
#include "color_value.hpp"

namespace clwe {

/**
 * @brief Color integration module for ColorKEM
 *
 * Provides functions to encode polynomials and vectors into RGB pixel arrays
 * for visualization, and decode back. Coefficients are ColorValue (32-bit)
 * packed into RGB pixels where each ColorValue uses 4 RGB pixels (12 bytes)
 * to preserve full 32-bit precision.
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

} // namespace clwe

#endif // CLWE_COLOR_INTEGRATION_HPP
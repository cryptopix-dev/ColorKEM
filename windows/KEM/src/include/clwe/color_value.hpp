#ifndef COLOR_VALUE_HPP
#define COLOR_VALUE_HPP

#include <cstdint>
#include <iostream>

namespace clwe {

/**
 * @brief Represents an RGBA color value used in Color-CLWE cryptographic operations.
 *
 * In the Color-CLWE scheme, colors serve as coefficients in ring elements (polynomials)
 * over the ring R_q = Z_q[X]/(X^n + 1), where q is a prime modulus and n is the ring dimension.
 *
 * Mapping from RGBA to Ring Elements:
 * - Each ColorValue (r, g, b, a) is packed into a single 32-bit unsigned integer via to_math_value():
 *   value = (r << 24) | (g << 16) | (b << 8) | a
 * - This packed value is treated as a coefficient in Z_q, i.e., coefficient ≡ value mod q
 * - For polynomial operations, each coefficient is a ColorValue, allowing visual interpretation
 *   of cryptographic computations while maintaining mathematical equivalence to standard LWE/CLWE.
 *
 * Mathematical Equivalence:
 * - Arithmetic operations (addition, subtraction, multiplication) are performed modulo q
 *   on the packed uint32_t representation, preserving the algebraic structure of the ring.
 * - The color channels (r,g,b,a) provide a visual representation but do not affect the
 *   underlying mathematical operations, which operate on the full 32-bit packed value.
 * - This allows cryptographic schemes to be "colored" for visualization while maintaining
 *   the security properties of the underlying lattice-based cryptography.
 */
struct ColorValue {
    uint8_t r, g, b, a;

    ColorValue() : r(0), g(0), b(0), a(255) {}
    ColorValue(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha = 255)
        : r(red), g(green), b(blue), a(alpha) {}

    uint32_t to_math_value() const {
        return (static_cast<uint32_t>(r) << 24) |
               (static_cast<uint32_t>(g) << 16) |
               (static_cast<uint32_t>(b) << 8) |
               static_cast<uint32_t>(a);
    }

    static ColorValue from_math_value(uint32_t value) {
        return ColorValue(
            (value >> 24) & 0xFF,
            (value >> 16) & 0xFF,
            (value >> 8) & 0xFF,
            value & 0xFF
        );
    }

    uint64_t to_precise_value() const {
        return (static_cast<uint64_t>(r) << 32) |
               (static_cast<uint64_t>(g) << 16) |
               static_cast<uint64_t>(b);
    }

    static ColorValue from_precise_value(uint64_t value) {
        return ColorValue(
            (value >> 32) & 0xFF,
            (value >> 16) & 0xFF,
            value & 0xFF,
            255
        );
    }

    ColorValue mod_add(const ColorValue& other, uint32_t modulus) const;
    ColorValue mod_subtract(const ColorValue& other, uint32_t modulus) const;
    ColorValue mod_multiply(const ColorValue& other, uint32_t modulus) const;

    ColorValue to_hsv() const;
    ColorValue from_hsv() const;

    bool operator==(const ColorValue& other) const {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    bool operator!=(const ColorValue& other) const {
        return !(*this == other);
    }

    std::string to_string() const;
    void print() const { std::cout << to_string() << std::endl; }
};

namespace color_ops {

    ColorValue add_colors(const ColorValue& a, const ColorValue& b);
    ColorValue multiply_colors(const ColorValue& a, const ColorValue& b);
    ColorValue mod_reduce_color(const ColorValue& c, uint32_t modulus);

    #ifdef HAVE_AVX512
    __m512i add_colors_avx512(__m512i a, __m512i b);
    __m512i multiply_colors_avx512(__m512i a, __m512i b);
    __m512i mod_reduce_colors_avx512(__m512i c, uint32_t modulus);
    #endif

    #ifdef __ARM_NEON
    #include <arm_neon.h>
    uint32x4_t add_colors_neon(uint32x4_t a, uint32x4_t b);
    uint32x4_t multiply_colors_neon(uint32x4_t a, uint32x4_t b);
    uint32x4_t mod_reduce_colors_neon(uint32x4_t c, uint32_t modulus);
    #endif

    ColorValue add_colors_simd(const ColorValue& a, const ColorValue& b);
    ColorValue multiply_colors_simd(const ColorValue& a, const ColorValue& b);
    ColorValue mod_reduce_color_simd(const ColorValue& c, uint32_t modulus);

};

} // namespace clwe

#endif // COLOR_VALUE_HPP
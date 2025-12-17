#include "clwe/color_kem.hpp"
#include "clwe/color_integration.hpp"
#include <iostream>

int main() {
    clwe::CLWEParameters params(768);
    clwe::ColorKEM kem(params);

    auto [pk, sk] = kem.keygen();

    auto color_data = clwe::encode_color_kem_public_key_as_colors(pk.public_data);

    std::cout << "Size of original key: " << pk.public_data.size() << " bytes" << std::endl;
    std::cout << "Size of color data: " << color_data.size() << " bytes" << std::endl;

    std::cout << "First 10 RGB values:" << std::endl;
    for (size_t i = 0; i < 10 && i * 3 + 2 < color_data.size(); ++i) {
        uint8_t r = color_data[i * 3];
        uint8_t g = color_data[i * 3 + 1];
        uint8_t b = color_data[i * 3 + 2];
        std::cout << "(" << (int)r << ", " << (int)g << ", " << (int)b << ")" << std::endl;
    }

    return 0;
}
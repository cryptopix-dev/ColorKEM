#include "src/include/clwe/color_kem.hpp"
#include "src/include/clwe/clwe.hpp"

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <array>
#include <cmath>
#include <stdexcept>

#include <webp/encode.h>
#include <webp/decode.h>

#ifdef _WIN32
#include <windows.h>
#include <bcrypt.h>
#pragma comment(lib, "bcrypt.lib")
#endif

/* ================= SECURE RANDOM ================= */

std::array<uint8_t, 32> secure_random_seed() {
    std::array<uint8_t, 32> seed{};

#ifdef _WIN32
    if (BCryptGenRandom(nullptr, seed.data(), seed.size(),
                        BCRYPT_USE_SYSTEM_PREFERRED_RNG) != 0) {
        throw std::runtime_error("Windows RNG failed");
    }
#else
    std::ifstream urandom("/dev/urandom", std::ios::binary);
    if (!urandom.read(reinterpret_cast<char*>(seed.data()), seed.size())) {
        throw std::runtime_error("Linux RNG failed");
    }
#endif
    return seed;
}

/* ================= WEBP SAVE ================= */

bool save_webp_file(const std::vector<uint8_t>& data,
                    const std::string& filename) {
    if (data.empty()) return false;

    std::vector<uint8_t> payload;
    uint32_t size = static_cast<uint32_t>(data.size());

    payload.push_back((size >> 24) & 0xFF);
    payload.push_back((size >> 16) & 0xFF);
    payload.push_back((size >> 8) & 0xFF);
    payload.push_back(size & 0xFF);
    payload.insert(payload.end(), data.begin(), data.end());

    size_t pixels = (payload.size() + 2) / 3;
    size_t width = static_cast<size_t>(std::ceil(std::sqrt(pixels)));
    size_t height = (pixels + width - 1) / width;

    std::vector<uint8_t> image(width * height * 3, 0);
    std::copy(payload.begin(), payload.end(), image.begin());

    uint8_t* webp_data = nullptr;
    size_t webp_size = WebPEncodeLosslessRGB(
        image.data(), width, height, width * 3, &webp_data);

    if (webp_size == 0) return false;

    std::ofstream out(filename, std::ios::binary);
    out.write(reinterpret_cast<char*>(webp_data), webp_size);
    WebPFree(webp_data);

    return out.good();
}

/* ================= WEBP LOAD ================= */

std::vector<uint8_t> load_webp_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) throw std::runtime_error("Cannot open WebP file");

    std::streamsize size = file.tellg();
    file.seekg(0);
    std::vector<uint8_t> buffer(size);
    file.read(reinterpret_cast<char*>(buffer.data()), size);

    int w, h;
    uint8_t* rgb = WebPDecodeRGB(buffer.data(), buffer.size(), &w, &h);
    if (!rgb) throw std::runtime_error("WebP decode failed");

    std::vector<uint8_t> data(rgb, rgb + (w * h * 3));
    WebPFree(rgb);

    uint32_t original_size =
        (data[0] << 24) | (data[1] << 16) | (data[2] << 8) | data[3];

    return std::vector<uint8_t>(
        data.begin() + 4, data.begin() + 4 + original_size);
}

/* ================= MAIN ================= */

int main() {
    try {
        // Secure parameters
        clwe::CLWEParameters params(512);
        clwe::ColorKEM kem(params);

        // Secure randomness
        auto matrix_seed = secure_random_seed();
        auto secret_seed = secure_random_seed();
        auto error_seed  = secure_random_seed();
        auto r_seed      = secure_random_seed();
        auto e1_seed     = secure_random_seed();
        auto e2_seed     = secure_random_seed();

        std::cout << "Generating secure keypair...\n";
        auto [public_key, private_key] =
            kem.keygen_deterministic(matrix_seed, secret_seed, error_seed);

        std::cout << "Encapsulating secret...\n";
        clwe::ColorValue fixed_secret = clwe::ColorValue::from_math_value(1);
        auto [ciphertext, shared_enc] =
            kem.encapsulate_deterministic(
                public_key, r_seed, e1_seed, e2_seed, fixed_secret);

        std::cout << "Decapsulating...\n";
        auto shared_dec =
            kem.decapsulate(public_key, private_key, ciphertext);

        std::cout << "shared_enc: " << shared_enc.to_math_value() << std::endl;
        std::cout << "shared_dec: " << shared_dec.to_math_value() << std::endl;

        if (shared_enc != shared_dec) {
            throw std::runtime_error("KEM verification FAILED");
        }

        std::cout << "KEM SUCCESS: shared secret verified\n";

        // Save public key as WebP
        save_webp_file(public_key.serialize(), "public_key.webp");
        save_webp_file(ciphertext.serialize(), "ciphertext.webp");

        std::cout << "Keys stored as WebP images\n";
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << "\n";
        return 1;
    }
}

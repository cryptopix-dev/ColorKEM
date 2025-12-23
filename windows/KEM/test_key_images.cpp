#include "src/include/clwe/color_kem.hpp"
#include "src/include/clwe/clwe.hpp"
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <webp/decode.h>
#include <webp/encode.h>
#include <cmath>

bool save_webp_file(const std::vector<uint8_t>& data, const std::string& filename) {
    if (data.empty()) return false;

    // Prepend the data size as 4 bytes (big-endian)
    std::vector<uint8_t> rgb_data;
    uint32_t size = data.size();
    rgb_data.push_back((size >> 24) & 0xFF);
    rgb_data.push_back((size >> 16) & 0xFF);
    rgb_data.push_back((size >> 8) & 0xFF);
    rgb_data.push_back(size & 0xFF);
    rgb_data.insert(rgb_data.end(), data.begin(), data.end());

    size_t total_size = rgb_data.size();
    size_t num_pixels = (total_size + 2) / 3; // Ensure at least enough for the data
    size_t width = static_cast<size_t>(std::ceil(std::sqrt(static_cast<double>(num_pixels))));
    size_t height = (num_pixels + width - 1) / width;

    // Create image buffer, pad with black pixels if necessary
    std::vector<uint8_t> image(width * height * 3, 0);
    for (size_t i = 0; i < total_size; ++i) {
        image[i] = rgb_data[i];
    }

    uint8_t* webp_data = nullptr;
    size_t webp_size = WebPEncodeLosslessRGB(image.data(), width, height, static_cast<int>(width * 3), &webp_data);
    if (webp_size == 0) {
        return false;
    }

    // Write to file
    std::ofstream file(filename, std::ios::binary);
    if (!file) {
        WebPFree(webp_data);
        return false;
    }
    file.write(reinterpret_cast<char*>(webp_data), webp_size);
    if (!file.good()) {
        WebPFree(webp_data);
        return false;
    }
    file.close();

    WebPFree(webp_data);
    return true;
}


std::vector<uint8_t> load_webp_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Failed to read file: " + filename);
    }

    // Decode WebP to RGB
    int width, height;
    uint8_t* rgb_data = WebPDecodeRGB(buffer.data(), buffer.size(), &width, &height);
    if (!rgb_data) {
        throw std::runtime_error("Failed to decode WebP: " + filename);
    }

    size_t num_pixels = static_cast<size_t>(width) * height;
    std::vector<uint8_t> full_data(rgb_data, rgb_data + num_pixels * 3);

    WebPFree(rgb_data);

    // Extract the original data size from the first 4 bytes
    if (full_data.size() < 4) {
        throw std::runtime_error("Invalid WebP data: too small");
    }
    uint32_t data_size = (static_cast<uint32_t>(full_data[0]) << 24) |
                         (static_cast<uint32_t>(full_data[1]) << 16) |
                         (static_cast<uint32_t>(full_data[2]) << 8) |
                         static_cast<uint32_t>(full_data[3]);

    if (full_data.size() < 4 + data_size) {
        throw std::runtime_error("Invalid WebP data: size mismatch");
    }

    std::vector<uint8_t> result(full_data.begin() + 4, full_data.begin() + 4 + data_size);
    return result;
}

std::vector<uint8_t> load_bin_file(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary | std::ios::ate);
    if (!file) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);
    std::vector<uint8_t> buffer(size);
    if (!file.read(reinterpret_cast<char*>(buffer.data()), size)) {
        throw std::runtime_error("Failed to read file: " + filename);
    }
    return buffer;
}

int main(int argc, char* argv[]) {
    std::string input_dir = ".";
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-d" && i + 1 < argc) {
            input_dir = argv[i + 1];
            ++i;
        }
    }

    try {
        clwe::CLWEParameters params(512);
        clwe::ColorKEM kem(params);

        // Fixed seeds for deterministic key generation and encapsulation
        std::array<uint8_t, 32> matrix_seed = {0};
        std::array<uint8_t, 32> secret_seed = {0};
        std::array<uint8_t, 32> error_seed = {0};
        std::array<uint8_t, 32> r_seed = {0};
        std::array<uint8_t, 32> e1_seed = {0};
        std::array<uint8_t, 32> e2_seed = {0};
        clwe::ColorValue fixed_shared_secret(0, 0, 0, 1); // shared_secret = 1

        std::cout << "Generating keys for testing..." << std::endl;
        auto [public_key, private_key] = kem.keygen_deterministic(matrix_seed, secret_seed, error_seed);
        std::cout << "Keys generated successfully!" << std::endl;

        // Test serialization and deserialization
        auto pub_ser = public_key.serialize();
        auto priv_ser = private_key.serialize();

        auto pub_loaded = clwe::ColorPublicKey::deserialize(pub_ser, params);
        auto priv_loaded = clwe::ColorPrivateKey::deserialize(priv_ser, params);
        std::cout << "Keys serialized and deserialized successfully!" << std::endl;
std::cout << "Encapsulating shared secret..." << std::endl;

auto [ciphertext, shared_secret_enc] = kem.encapsulate_deterministic(public_key, r_seed, e1_seed, e2_seed, fixed_shared_secret);

std::cout << "Encapsulation successful!" << std::endl;

        std::cout << "Decapsulating shared secret..." << std::endl;
        auto shared_secret_dec = kem.decapsulate(public_key, private_key, ciphertext);

        std::cout << "Encapsulated secret: " << shared_secret_enc.to_math_value() << std::endl;
        std::cout << "Decapsulated secret: " << shared_secret_dec.to_math_value() << std::endl;

        if (shared_secret_enc == shared_secret_dec) {
            std::cout << "Shared secret matches!" << std::endl;
            std::cout << "Key images can be used for KEM operations!" << std::endl;
        } else {
            std::cout << "Shared secret does not match!" << std::endl;
            return 1;
        }

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}

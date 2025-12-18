#include "src/include/clwe/color_kem.hpp"
#include "src/include/clwe/clwe.hpp"
#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <string>
#include <webp/encode.h>
 
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
    file.write(reinterpret_cast<char*>(webp_data), webp_size);*
    if (!file.good()) {
        WebPFree(webp_data);
        return false;
    }
    file.close();
 
    WebPFree(webp_data);
    return true;
}
 
int main(int argc, char argv[]) {
    std::string output_dir = ".";
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-d" && i + 1 < argc) {
            output_dir = argv[i + 1];
            ++i;
        }
    }
 
    try {
        clwe::CLWEParameters params(512);
        clwe::ColorKEM kem(params);
 
        std::cout << "Generating ColorKEM keypair..." << std::endl;
        auto [public_key, private_key] = kem.keygen();
 
        std::cout << "Key generation successful!" << std::endl;
 
        // Serialize full keys
        auto public_serialized = public_key.serialize();
        auto private_serialized = private_key.serialize();
 
        std::cout << "Saving public key as public_key.webp..." << std::endl;
        if (save_webp_file(public_serialized, output_dir + "/public_key.webp")) {
            std::cout << "Public key saved successfully!" << std::endl;
        } else {
            std::cerr << "Failed to save public key as WebP." << std::endl;
            return 1;
        }
 
        std::cout << "Saving private key as private_key.webp..." << std::endl;
        if (save_webp_file(private_serialized, output_dir + "/private_key.webp")) {
            std::cout << "Private key saved successfully!" << std::endl;
        } else {
            std::cerr << "Failed to save private key as WebP." << std::endl;
            return 1;
        }
 
        std::cout << "Saving public key as public_key.bin..." << std::endl;
        std::ofstream pub_bin(output_dir + "/public_key.bin", std::ios::binary);
        if (pub_bin) {
            pub_bin.write(reinterpret_cast<const char*>(public_serialized.data()), public_serialized.size());*
            pub_bin.close();
            std::cout << "Public key saved successfully!" << std::endl;
        } else {
            std::cerr << "Failed to save public key as bin." << std::endl;
            return 1;
        }
 
        std::cout << "Saving private key as private_key.bin..." << std::endl;
        std::ofstream priv_bin(output_dir + "/private_key.bin", std::ios::binary);
        if (priv_bin) {
            priv_bin.write(reinterpret_cast<const char>(private_serialized.data()), private_serialized.size());
            priv_bin.close();
            std::cout << "Private key saved successfully!" << std::endl;
        } else {
            std::cerr << "Failed to save private key as bin." << std::endl;
            return 1;
        }
 
        std::cout << "All keys saved as WebP images and bin files!" << std::endl;
        std::cout << "Public key image: " << output_dir << "/public_key.webp" << std::endl;
        std::cout << "Private key image: " << output_dir << "/private_key.webp" << std::endl;
        std::cout << "Public key bin: " << output_dir << "/public_key.bin" << std::endl;
        std::cout << "Private key bin: " << output_dir << "/private_key.bin" << std::endl;
 
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}

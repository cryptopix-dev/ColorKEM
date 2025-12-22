#include <filesystem>
#include <iostream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <fstream>
#include <string>
#include <stdexcept>
#include <system_error>

#ifdef __has_include
#  if __has_include(<webp/encode.h>)
#    include <webp/encode.h>
#    define HAS_WEBP 1
#  else
#    define HAS_WEBP 0
#  endif
#else
#  include <webp/encode.h>
#  define HAS_WEBP 1
#endif

#include "src/include/clwe/color_kem.hpp"
#include "src/include/clwe/clwe.hpp"

namespace fs = std::filesystem;

/**
 * Saves serialized data as a WebP image file.
 * Encodes the data into an RGB image and saves it as lossless WebP.
 * @param data The serialized data to save.
 * @param filepath The path to the output WebP file.
 * @return true if successful, false otherwise.
 */
bool save_webp_file(const std::vector<uint8_t>& data, const fs::path& filepath) {
    if (data.empty()) {
        std::cerr << "Error: Data is empty, cannot save WebP file." << std::endl;
        return false;
    }

#if HAS_WEBP
    // Prepend the data size as 4 bytes (big-endian for cross-platform compatibility)
    std::vector<uint8_t> rgb_data;
    uint32_t size = static_cast<uint32_t>(data.size());
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
    size_t webp_size = WebPEncodeLosslessRGB(image.data(), static_cast<int>(width), static_cast<int>(height), static_cast<int>(width * 3), &webp_data);
    if (webp_size == 0) {
        std::cerr << "Error: Failed to encode WebP data." << std::endl;
        return false;
    }

    // Write to file with error checking
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open file for writing: " << filepath << std::endl;
        WebPFree(webp_data);
        return false;
    }
    file.write(reinterpret_cast<char*>(webp_data), webp_size);
    if (!file.good()) {
        std::cerr << "Error: Failed to write WebP data to file: " << filepath << std::endl;
        WebPFree(webp_data);
        file.close();
        return false;
    }
    file.close();
    if (!file.good()) {
        std::cerr << "Error: Failed to close WebP file: " << filepath << std::endl;
        WebPFree(webp_data);
        return false;
    }

    WebPFree(webp_data);
    return true;
#else
    std::cerr << "Error: WebP library not available. Cannot save WebP file." << std::endl;
    return false;
#endif
}

/**
 * Saves serialized data as a binary file.
 * @param data The serialized data to save.
 * @param filepath The path to the output binary file.
 * @return true if successful, false otherwise.
 */
bool save_binary_file(const std::vector<uint8_t>& data, const fs::path& filepath) {
    std::ofstream file(filepath, std::ios::binary);
    if (!file) {
        std::cerr << "Error: Cannot open binary file for writing: " << filepath << std::endl;
        return false;
    }
    file.write(reinterpret_cast<const char*>(data.data()), data.size());
    if (!file.good()) {
        std::cerr << "Error: Failed to write binary data to file: " << filepath << std::endl;
        file.close();
        return false;
    }
    file.close();
    if (!file.good()) {
        std::cerr << "Error: Failed to close binary file: " << filepath << std::endl;
        return false;
    }
    return true;
}

/**
 * Validates and prepares the output directory.
 * Checks if the directory exists and is writable.
 * @param dir_path The path to the output directory.
 * @return true if valid, false otherwise.
 */
bool validate_output_directory(const fs::path& dir_path) {
    std::error_code ec;
    if (!fs::exists(dir_path, ec)) {
        std::cerr << "Error: Output directory does not exist: " << dir_path << std::endl;
        return false;
    }
    if (!fs::is_directory(dir_path, ec)) {
        std::cerr << "Error: Specified path is not a directory: " << dir_path << std::endl;
        return false;
    }
    // Check writability by attempting to create a temp file
    fs::path temp_file = dir_path / "temp_check.tmp";
    std::ofstream temp(temp_file, std::ios::binary);
    if (!temp) {
        std::cerr << "Error: Output directory is not writable: " << dir_path << std::endl;
        return false;
    }
    temp.close();
    fs::remove(temp_file, ec); // Ignore errors on remove
    return true;
}

/**
 * Prints usage information.
 */
void print_usage(const char* program_name) {
    std::cout << "Usage: " << program_name << " [options]\n"
              << "Options:\n"
              << "  -d <directory>  Output directory (default: current directory)\n"
              << "  -h              Show this help message\n";
}

/**
 * Main function: Generates ColorKEM keypair and saves as WebP images and binary files.
 */
int main(int argc, char* argv[]) {
    fs::path output_dir = ".";

    // Parse command-line arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-d" && i + 1 < argc) {
            output_dir = argv[++i];
        } else if (arg == "-h" || arg == "--help") {
            print_usage(argv[0]);
            return 0;
        } else {
            std::cerr << "Error: Unknown option: " << arg << std::endl;
            print_usage(argv[0]);
            return 1;
        }
    }

    // Validate output directory
    if (!validate_output_directory(output_dir)) {
        return 1;
    }

    try {
        // Initialize parameters and KEM
        clwe::CLWEParameters params(512);
        clwe::ColorKEM kem(params);

        std::cout << "Generating ColorKEM keypair..." << std::endl;
        auto [public_key, private_key] = kem.keygen();
        std::cout << "Key generation successful!" << std::endl;

        // Serialize keys
        auto public_serialized = public_key.serialize();
        auto private_serialized = private_key.serialize();

        // Save public key as WebP
        fs::path pub_webp_path = output_dir / "public_key.webp";
        std::cout << "Saving public key as WebP: " << pub_webp_path << std::endl;
        if (!save_webp_file(public_serialized, pub_webp_path)) {
            return 1;
        }
        std::cout << "Public key saved successfully as WebP!" << std::endl;

        // Save private key as WebP
        fs::path priv_webp_path = output_dir / "private_key.webp";
        std::cout << "Saving private key as WebP: " << priv_webp_path << std::endl;
        if (!save_webp_file(private_serialized, priv_webp_path)) {
            return 1;
        }
        std::cout << "Private key saved successfully as WebP!" << std::endl;

        // Save public key as binary
        fs::path pub_bin_path = output_dir / "public_key.bin";
        std::cout << "Saving public key as binary: " << pub_bin_path << std::endl;
        if (!save_binary_file(public_serialized, pub_bin_path)) {
            return 1;
        }
        std::cout << "Public key saved successfully as binary!" << std::endl;

        // Save private key as binary
        fs::path priv_bin_path = output_dir / "private_key.bin";
        std::cout << "Saving private key as binary: " << priv_bin_path << std::endl;
        if (!save_binary_file(private_serialized, priv_bin_path)) {
            return 1;
        }
        std::cout << "Private key saved successfully as binary!" << std::endl;

        std::cout << "\nAll keys saved successfully!" << std::endl;
        std::cout << "Public key WebP: " << pub_webp_path << std::endl;
        std::cout << "Private key WebP: " << priv_webp_path << std::endl;
        std::cout << "Public key binary: " << pub_bin_path << std::endl;
        std::cout << "Private key binary: " << priv_bin_path << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}

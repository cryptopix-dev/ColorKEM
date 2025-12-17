#include "../include/clwe/color_integration.hpp"
#include <stdexcept>
#include <algorithm>

namespace clwe {

std::vector<uint8_t> encode_color_kem_public_key_as_colors(const std::vector<uint8_t>& public_key_data) {
    std::vector<uint8_t> color_data;

    // For ColorKEM, the serialized data contains uint32_t coefficients stored as 4 bytes each (big-endian)
    // We extract each coefficient, take its lower 8 bits, and pack them into RGB pixel format
    for (size_t i = 0; i < public_key_data.size(); i += 4) {
        if (i + 3 >= public_key_data.size()) break;

        // Reconstruct the uint32_t coefficient from big-endian bytes
        uint32_t coeff = (static_cast<uint32_t>(public_key_data[i]) << 24) |
                        (static_cast<uint32_t>(public_key_data[i + 1]) << 16) |
                        (static_cast<uint32_t>(public_key_data[i + 2]) << 8) |
                        static_cast<uint32_t>(public_key_data[i + 3]);

        // Take lower 8 bits and add to color data
        color_data.push_back(coeff & 0xFF);
    }

    return color_data;
}

std::vector<uint8_t> decode_colors_to_color_kem_public_key(const std::vector<uint8_t>& color_data, size_t expected_size) {
    // For ColorKEM, the color data contains lower 8 bits of uint32_t coefficients
    // We need to reconstruct the full 4-byte representation for each coefficient
    std::vector<uint8_t> key_data;

    // Each color byte represents the lower 8 bits of a coefficient
    for (uint8_t color_byte : color_data) {
        // Reconstruct the full 4-byte coefficient (lower 8 bits contain the value, upper 24 bits are 0)
        uint32_t coeff = color_byte;  // Only lower 8 bits are meaningful

        // Store as big-endian bytes
        key_data.push_back((coeff >> 24) & 0xFF);
        key_data.push_back((coeff >> 16) & 0xFF);
        key_data.push_back((coeff >> 8) & 0xFF);
        key_data.push_back(coeff & 0xFF);
    }

    if (key_data.size() != expected_size) {
        throw std::invalid_argument("Reconstructed key data size does not match expected size");
    }

    return key_data;
}

std::vector<uint8_t> encode_color_kem_private_key_as_colors(const std::vector<uint8_t>& private_key_data) {
    // Same encoding as public key - pack coefficients into RGB pixels
    return encode_color_kem_public_key_as_colors(private_key_data);
}

std::vector<uint8_t> decode_colors_to_color_kem_private_key(const std::vector<uint8_t>& color_data, size_t expected_size) {
    // Same decoding as public key
    return decode_colors_to_color_kem_public_key(color_data, expected_size);
}

std::vector<uint8_t> encode_color_kem_ciphertext_as_colors(const std::vector<uint8_t>& ciphertext_data) {
    // Same encoding as keys - pack coefficients into RGB pixels
    return encode_color_kem_public_key_as_colors(ciphertext_data);
}

std::vector<uint8_t> decode_colors_to_color_kem_ciphertext(const std::vector<uint8_t>& color_data, size_t expected_size) {
    // Same decoding as keys
    return decode_colors_to_color_kem_public_key(color_data, expected_size);
}

// Compressed color encoding adapted for KEM keys using variable-length encoding
std::vector<uint8_t> encode_color_kem_key_as_colors_compressed(const std::vector<uint8_t>& key_data) {
    std::vector<uint8_t> compressed;
    compressed.reserve(key_data.size() + 8);

    // Add format version and compression flag for KEM key color-compatible compression
    compressed.push_back(0x01); // Version 1
    compressed.push_back(0x07); // Compression flag (7 = KEM key color-compatible compressed)

    // Store key data size
    uint32_t data_size = key_data.size();
    compressed.push_back(static_cast<uint8_t>(data_size >> 24));
    compressed.push_back(static_cast<uint8_t>(data_size >> 16));
    compressed.push_back(static_cast<uint8_t>(data_size >> 8));
    compressed.push_back(static_cast<uint8_t>(data_size & 0xFF));

    // Compress the coefficients using variable-length encoding
    // Each coefficient is 4 bytes (uint32_t big-endian)
    for (size_t i = 0; i < key_data.size(); i += 4) {
        if (i + 3 >= key_data.size()) break;

        // Reconstruct the uint32_t coefficient from big-endian bytes
        uint32_t coeff = (static_cast<uint32_t>(key_data[i]) << 24) |
                        (static_cast<uint32_t>(key_data[i + 1]) << 16) |
                        (static_cast<uint32_t>(key_data[i + 2]) << 8) |
                        static_cast<uint32_t>(key_data[i + 3]);

        // Apply variable-length encoding similar to ColorSign
        if (coeff == 0) {
            compressed.push_back(0x00); // Single byte for zero
        } else if (coeff < 0x40) {
            compressed.push_back(static_cast<uint8_t>(coeff | 0x80)); // 1 byte (0x80-0xBF)
        } else if (coeff < 0x2000) {
            compressed.push_back(static_cast<uint8_t>((coeff >> 8) | 0xC0)); // 2 bytes (0xC0-0xDF)
            compressed.push_back(static_cast<uint8_t>(coeff & 0xFF));
        } else if (coeff < 0x100000) {
            compressed.push_back(static_cast<uint8_t>((coeff >> 16) | 0xE0)); // 3 bytes (0xE0-0xEF)
            compressed.push_back(static_cast<uint8_t>((coeff >> 8) & 0xFF));
            compressed.push_back(static_cast<uint8_t>(coeff & 0xFF));
        } else {
            // For larger values, use 5 bytes (0xF0 + 4 bytes)
            compressed.push_back(0xF0);
            compressed.push_back(static_cast<uint8_t>((coeff >> 24) & 0xFF));
            compressed.push_back(static_cast<uint8_t>((coeff >> 16) & 0xFF));
            compressed.push_back(static_cast<uint8_t>((coeff >> 8) & 0xFF));
            compressed.push_back(static_cast<uint8_t>(coeff & 0xFF));
        }
    }

    return compressed;
}

// Decode compressed KEM key color data
std::vector<uint8_t> decode_colors_to_color_kem_key_compressed(const std::vector<uint8_t>& color_data, size_t expected_size) {
    if (color_data.size() < 8) {
        throw std::invalid_argument("Compressed color data too small");
    }

    size_t offset = 0;
    uint8_t version = color_data[offset++];
    uint8_t compression_flag = color_data[offset++];

    // Validate version and compression flag
    if (version != 0x01 || compression_flag != 0x07) {
        throw std::invalid_argument("Unsupported KEM key color-compatible compression format");
    }

    // Read data size
    uint32_t data_size = (static_cast<uint32_t>(color_data[offset]) << 24) |
                        (static_cast<uint32_t>(color_data[offset + 1]) << 16) |
                        (static_cast<uint32_t>(color_data[offset + 2]) << 8) |
                        color_data[offset + 3];
    offset += 4;

    if (data_size != expected_size) {
        throw std::invalid_argument("Data size mismatch in compressed color data");
    }

    // Decompress the coefficients using variable-length decoding
    std::vector<uint8_t> key_data;
    key_data.reserve(data_size);

    size_t num_coeffs = data_size / 4; // Each coefficient is 4 bytes
    size_t coeff_idx = 0;

    while (coeff_idx < num_coeffs && offset < color_data.size()) {
        if (offset >= color_data.size()) {
            throw std::invalid_argument("Truncated compressed color data");
        }

        uint8_t first_byte = color_data[offset++];
        uint32_t coeff = 0;

        if (first_byte == 0x00) {
            coeff = 0;
        } else if ((first_byte & 0xC0) == 0x80) {
            // 1-byte encoding: 0x80-0xBF
            coeff = first_byte & 0x3F;
        } else if ((first_byte & 0xE0) == 0xC0) {
            // 2-byte encoding: 0xC0-0xDF
            if (offset >= color_data.size()) throw std::invalid_argument("Truncated compressed color data");
            coeff = ((first_byte & 0x1F) << 8) | color_data[offset++];
        } else if ((first_byte & 0xF0) == 0xE0) {
            // 3-byte encoding: 0xE0-0xEF
            if (offset + 1 >= color_data.size()) throw std::invalid_argument("Truncated compressed color data");
            coeff = ((first_byte & 0x0F) << 16) | (color_data[offset] << 8) | color_data[offset + 1];
            offset += 2;
        } else if (first_byte == 0xF0) {
            // 5-byte encoding: 0xF0 + 4 bytes
            if (offset + 3 >= color_data.size()) throw std::invalid_argument("Truncated compressed color data");
            coeff = (color_data[offset] << 24) | (color_data[offset + 1] << 16) | (color_data[offset + 2] << 8) | color_data[offset + 3];
            offset += 4;
        } else {
            throw std::invalid_argument("Invalid KEM key color-compatible compression encoding");
        }

        // Convert back to 4-byte big-endian representation
        key_data.push_back(static_cast<uint8_t>((coeff >> 24) & 0xFF));
        key_data.push_back(static_cast<uint8_t>((coeff >> 16) & 0xFF));
        key_data.push_back(static_cast<uint8_t>((coeff >> 8) & 0xFF));
        key_data.push_back(static_cast<uint8_t>(coeff & 0xFF));

        coeff_idx++;
    }

    if (key_data.size() != expected_size) {
        throw std::invalid_argument("Decompressed key data size does not match expected size: got " + std::to_string(key_data.size()) + ", expected " + std::to_string(expected_size));
    }

    return key_data;
}

// Convert compressed key data to RGB color format
std::vector<uint8_t> convert_compressed_key_to_color_format(const std::vector<uint8_t>& compressed_data, size_t expected_size) {
    // First decode the compressed data
    auto key_data = decode_colors_to_color_kem_key_compressed(compressed_data, expected_size);

    // Then encode as RGB color format
    return encode_color_kem_public_key_as_colors(key_data);
}

// Auto-select best compression method for KEM key color integration
std::vector<uint8_t> encode_color_kem_key_as_colors_auto(const std::vector<uint8_t>& key_data) {
    // Count zero bytes to determine sparsity
    size_t total_bytes = key_data.size();
    size_t zero_bytes = 0;

    for (uint8_t byte : key_data) {
        if (byte == 0) {
            zero_bytes++;
        }
    }

    // If the data has many zeros, use color-compatible compression
    // Otherwise use standard color format
    if (zero_bytes > total_bytes * 0.3) { // 30% threshold for compression
        return encode_color_kem_key_as_colors_compressed(key_data);
    } else {
        return encode_color_kem_public_key_as_colors(key_data);
    }
}

// On-demand color generation from compressed key data
std::vector<uint8_t> generate_color_representation_from_compressed_key(const std::vector<uint8_t>& compressed_data, size_t expected_size) {
    // First decode the compressed data to key data
    auto key_data = decode_colors_to_color_kem_key_compressed(compressed_data, expected_size);

    // Then convert to RGB color format for visualization
    return encode_color_kem_public_key_as_colors(key_data);
}

// Dual-format compression with color metadata integration for KEM keys
std::vector<uint8_t> compress_kem_key_with_color_support(const std::vector<uint8_t>& key_data, bool enable_color_metadata) {
    // First compress the key data using auto compression
    auto compressed_data = encode_color_kem_key_as_colors_auto(key_data);

    // If color metadata is enabled, add color generation capabilities
    if (enable_color_metadata) {
        std::vector<uint8_t> dual_format_data;

        // Add dual-format header
        dual_format_data.push_back(0x03); // Version 3 (KEM specific)
        dual_format_data.push_back(0x01); // Dual-format flag

        // Store key data size
        uint32_t data_size = key_data.size();
        dual_format_data.push_back(static_cast<uint8_t>(data_size >> 24));
        dual_format_data.push_back(static_cast<uint8_t>(data_size >> 16));
        dual_format_data.push_back(static_cast<uint8_t>(data_size >> 8));
        dual_format_data.push_back(static_cast<uint8_t>(data_size & 0xFF));

        // Add compressed key data
        dual_format_data.insert(dual_format_data.end(), compressed_data.begin(), compressed_data.end());

        // Add color metadata (optional, can be used for visualization hints)
        dual_format_data.push_back(0x01); // Color generation enabled

        return dual_format_data;
    }

    return compressed_data;
}

// Decompress dual-format KEM key data with color support
std::vector<uint8_t> decompress_kem_key_with_color_support(const std::vector<uint8_t>& dual_format_data, size_t& out_size) {
    if (dual_format_data.size() < 8) {
        throw std::invalid_argument("Dual-format data too small");
    }

    size_t offset = 0;
    uint8_t version = dual_format_data[offset++];
    uint8_t format_flag = dual_format_data[offset++];

    // Read key data size
    uint32_t data_size = (static_cast<uint32_t>(dual_format_data[offset]) << 24) |
                        (static_cast<uint32_t>(dual_format_data[offset + 1]) << 16) |
                        (static_cast<uint32_t>(dual_format_data[offset + 2]) << 8) |
                        dual_format_data[offset + 3];
    offset += 4;

    // Set output size
    out_size = data_size;

    // Extract the compressed key data (skip color metadata for now)
    std::vector<uint8_t> compressed_key_data(dual_format_data.begin() + offset, dual_format_data.end());

    // Decompress the key data
    return decode_colors_to_color_kem_key_compressed(compressed_key_data, data_size);
}

// Generate color representation from KEM dual-format data
std::vector<uint8_t> generate_color_from_kem_dual_format(const std::vector<uint8_t>& dual_format_data) {
    size_t data_size;
    // First decompress to get the key data
    auto key_data = decompress_kem_key_with_color_support(dual_format_data, data_size);

    // Then generate RGB color representation
    return encode_color_kem_public_key_as_colors(key_data);
}

// Advanced color integration with on-demand generation for KEM keys
std::vector<uint8_t> encode_kem_key_with_color_integration(const std::vector<uint8_t>& key_data, bool enable_on_demand_color) {
    if (enable_on_demand_color) {
        // Use dual-format architecture for on-demand color generation
        return compress_kem_key_with_color_support(key_data, true);
    } else {
        // Use standard color encoding
        return encode_color_kem_key_as_colors_auto(key_data);
    }
}

// Decode color-integrated KEM key data with on-demand support
std::vector<uint8_t> decode_kem_key_with_color_integration(const std::vector<uint8_t>& color_integrated_data) {
    // Check if this is dual-format data
    if (color_integrated_data.size() >= 8 &&
        color_integrated_data[0] == 0x03 &&
        color_integrated_data[1] == 0x01) {
        // Dual-format data - extract key data
        size_t data_size;
        return decompress_kem_key_with_color_support(color_integrated_data, data_size);
    } else {
        // Standard color format - decode normally
        // For standard format, we need to know the expected size
        // This is a limitation - dual format includes size information
        throw std::invalid_argument("Standard color format requires expected size parameter. Use dual-format for automatic size detection.");
    }
}

} // namespace clwe
#ifndef NIST_KAT_PARSER_HPP
#define NIST_KAT_PARSER_HPP

#include <vector>
#include <string>
#include <array>
#include <cstdint>

namespace clwe {

// Structure for ML-KEM KAT test vector
struct MLKEM_KAT_TestVector {
    uint32_t count;
    std::array<uint8_t, 32> seed;
    std::vector<uint8_t> pk;
    std::vector<uint8_t> sk;
    std::vector<uint8_t> ct;
    std::vector<uint8_t> ss;
};

// Structure for ML-DSA KAT test vector
struct MLDSA_KAT_TestVector {
    uint32_t count;
    std::array<uint8_t, 32> seed;
    std::vector<uint8_t> pk;
    std::vector<uint8_t> sk;
    std::vector<uint8_t> message;
    std::vector<uint8_t> sig;
};

// Class to download and parse NIST KAT files
class NIST_KAT_Downloader {
public:
    // Download ML-KEM KAT file for given parameter set
    static std::string download_mlkem_kat(uint32_t security_level);

    // Download ML-DSA KAT file for given parameter set
    static std::string download_mldsa_kat(uint32_t security_level);

    // Parse ML-KEM .rsp file content
    static std::vector<MLKEM_KAT_TestVector> parse_mlkem_kat(const std::string& content);

    // Parse ML-DSA .rsp file content
    static std::vector<MLDSA_KAT_TestVector> parse_mldsa_kat(const std::string& content);
};

} // namespace clwe

#endif // NIST_KAT_PARSER_HPP
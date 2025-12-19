#include <gtest/gtest.h>
#include "color_kem.hpp"
#include "clwe.hpp"
#include "nist_kat_parser.hpp"
#include <vector>
#include <array>
#include <iostream>
#include <stdexcept>

namespace clwe {

class ColorKEMKnownAnswerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Test with different security levels
        security_levels_ = {512, 768, 1024};
    }

    std::vector<uint32_t> security_levels_;

    // Load NIST KAT vectors for a given security level
    std::vector<MLKEM_KAT_TestVector> load_nist_kat_vectors(uint32_t security_level) {
        try {
            std::string kat_content = NIST_KAT_Downloader::download_mlkem_kat(security_level);
            return NIST_KAT_Downloader::parse_mlkem_kat(kat_content);
        } catch (const std::exception& e) {
            std::cerr << "Failed to load NIST KAT vectors for level " << security_level
                      << ": " << e.what() << std::endl;
            // Return empty vector if download fails
            return {};
        }
    }
};

// Test NIST KAT vectors for key generation
TEST_F(ColorKEMKnownAnswerTest, NIST_KAT_KeyGeneration512) {
    auto kat_vectors = load_nist_kat_vectors(512);
    if (kat_vectors.empty()) {
        GTEST_SKIP() << "NIST KAT vectors not available for ML-KEM-512";
    }

    CLWEParameters params(512);
    ColorKEM kem(params);

    // Test first few vectors
    for (size_t i = 0; i < std::min(size_t(5), kat_vectors.size()); ++i) {
        const auto& tv = kat_vectors[i];

        // Generate keypair using the seed from KAT
        auto [public_key, private_key] = kem.keygen_deterministic(
            tv.seed, tv.seed, tv.seed); // Using same seed for all as per ML-KEM spec

        // Verify against expected values
        auto pk_serialized = public_key.serialize();
        auto sk_serialized = private_key.serialize();

        ASSERT_EQ(pk_serialized, tv.pk) << "Public key mismatch for count " << tv.count;
        ASSERT_EQ(sk_serialized, tv.sk) << "Private key mismatch for count " << tv.count;
    }
}

// Test NIST KAT vectors for encapsulation/decapsulation
TEST_F(ColorKEMKnownAnswerTest, NIST_KAT_Encapsulation512) {
    auto kat_vectors = load_nist_kat_vectors(512);
    if (kat_vectors.empty()) {
        GTEST_SKIP() << "NIST KAT vectors not available for ML-KEM-512";
    }

    CLWEParameters params(512);
    ColorKEM kem(params);

    // Test first few vectors
    for (size_t i = 0; i < std::min(size_t(5), kat_vectors.size()); ++i) {
        const auto& tv = kat_vectors[i];

        // Generate keypair
        auto [public_key, private_key] = kem.keygen_deterministic(
            tv.seed, tv.seed, tv.seed);

        // Deserialize expected public key for encapsulation
        auto expected_pk = ColorPublicKey::deserialize(tv.pk, params);

        // Perform encapsulation with the seed from KAT
        auto [ciphertext, shared_secret] = kem.encapsulate_deterministic(
            expected_pk, tv.seed, tv.seed, tv.seed, ColorValue::from_math_value(0));

        // Verify against expected values
        auto ct_serialized = ciphertext.serialize();
        auto ss_serialized = shared_secret.serialize();

        ASSERT_EQ(ct_serialized, tv.ct) << "Ciphertext mismatch for count " << tv.count;
        ASSERT_EQ(ss_serialized, tv.ss) << "Shared secret mismatch for count " << tv.count;

        // Test decapsulation
        ColorValue recovered_secret = kem.decapsulate(expected_pk, private_key, ciphertext);
        ASSERT_EQ(recovered_secret, shared_secret) << "Decapsulation failed for count " << tv.count;
    }
}

// Test NIST KAT vectors for key generation - 768
TEST_F(ColorKEMKnownAnswerTest, NIST_KAT_KeyGeneration768) {
    auto kat_vectors = load_nist_kat_vectors(768);
    if (kat_vectors.empty()) {
        GTEST_SKIP() << "NIST KAT vectors not available for ML-KEM-768";
    }

    CLWEParameters params(768);
    ColorKEM kem(params);

    // Test first few vectors
    for (size_t i = 0; i < std::min(size_t(3), kat_vectors.size()); ++i) {
        const auto& tv = kat_vectors[i];

        auto [public_key, private_key] = kem.keygen_deterministic(
            tv.seed, tv.seed, tv.seed);

        auto pk_serialized = public_key.serialize();
        auto sk_serialized = private_key.serialize();

        ASSERT_EQ(pk_serialized, tv.pk) << "Public key mismatch for count " << tv.count;
        ASSERT_EQ(sk_serialized, tv.sk) << "Private key mismatch for count " << tv.count;
    }
}

// Test NIST KAT vectors for key generation - 1024
TEST_F(ColorKEMKnownAnswerTest, NIST_KAT_KeyGeneration1024) {
    auto kat_vectors = load_nist_kat_vectors(1024);
    if (kat_vectors.empty()) {
        GTEST_SKIP() << "NIST KAT vectors not available for ML-KEM-1024";
    }

    CLWEParameters params(1024);
    ColorKEM kem(params);

    // Test first few vectors
    for (size_t i = 0; i < std::min(size_t(3), kat_vectors.size()); ++i) {
        const auto& tv = kat_vectors[i];

        auto [public_key, private_key] = kem.keygen_deterministic(
            tv.seed, tv.seed, tv.seed);

        auto pk_serialized = public_key.serialize();
        auto sk_serialized = private_key.serialize();

        ASSERT_EQ(pk_serialized, tv.pk) << "Public key mismatch for count " << tv.count;
        ASSERT_EQ(sk_serialized, tv.sk) << "Private key mismatch for count " << tv.count;
    }
}

} // namespace clwe
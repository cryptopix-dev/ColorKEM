/**
 * @file test_hsm_integration.cpp
 * @brief Basic test for HSM integration functionality
 *
 * This test verifies that the HSM infrastructure works correctly
 * and doesn't break existing ColorKEM functionality.
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 */

#include <iostream>
#include <cassert>
#include "hsm/hsm_interface.hpp"
#include "hsm/software_hsm.hpp"
#include "hsm/hsm_config.hpp"

#ifdef ENABLE_HSM
#include "clwe/color_kem.hpp"
#endif

void test_software_hsm() {
    std::cout << "Testing Software HSM..." << std::endl;

    auto hsm = clwe::hsm::create_software_hsm();
    assert(hsm != nullptr);

    // Test initialization
    auto init_result = hsm->initialize();
    assert(init_result.success);
    std::cout << "  HSM initialized: " << init_result.error_message << std::endl;

    // Test key generation
    auto gen_result = hsm->generate_key(clwe::hsm::KeyType::SECRET_KEY, 32);
    assert(gen_result.success);
    assert(!gen_result.data.empty());
    std::cout << "  Key generated, size: " << gen_result.data.size() << std::endl;

    // Test key import
    std::vector<uint8_t> test_key = {1, 2, 3, 4, 5};
    auto import_result = hsm->import_key(clwe::hsm::KeyType::SECRET_KEY, test_key);
    assert(import_result.success);
    std::cout << "  Key imported successfully" << std::endl;

    // Test status
    auto status_result = hsm->get_status();
    assert(status_result.success);
    std::cout << "  HSM status retrieved" << std::endl;

    std::cout << "Software HSM tests passed!" << std::endl;
}

void test_hsm_config() {
    std::cout << "Testing HSM Configuration..." << std::endl;

    // Test default config
    clwe::hsm::HSMConfig config;
    assert(!config.isEnabled());
    assert(config.allowSoftwareFallback());
    std::cout << "  Default config: disabled" << std::endl;

    // Test software simulation config
    auto sim_config = clwe::hsm::HSMConfig::softwareSimulation();
    assert(sim_config.isEnabled());
    assert(sim_config.backend == clwe::hsm::HSMBackend::SOFTWARE_SIM);
    std::cout << "  Software simulation config created" << std::endl;

    // Test global config
    clwe::hsm::set_global_hsm_config(sim_config);
    const auto& global_config = clwe::hsm::get_global_hsm_config();
    assert(global_config.isEnabled());
    std::cout << "  Global config set and retrieved" << std::endl;

    std::cout << "HSM Configuration tests passed!" << std::endl;
}

#ifdef ENABLE_HSM
void test_colorkem_with_hsm() {
    std::cout << "Testing ColorKEM with HSM..." << std::endl;

    // Set up HSM config
    auto hsm_config = clwe::hsm::HSMConfig::softwareSimulation();
    clwe::hsm::set_global_hsm_config(hsm_config);

    // Create ColorKEM instance
    clwe::CLWEParameters params(512);  // ML-KEM-512
    clwe::ColorKEM kem(params);

    // Test key generation
    auto [pk, sk] = kem.keygen();
    assert(!pk.public_data.empty());
    assert(!sk.secret_data.empty());
    std::cout << "  Key pair generated with HSM support" << std::endl;

    // Test encapsulation
    auto [ct, ss] = kem.encapsulate(pk);
    assert(!ct.ciphertext_data.empty());
    assert(!ct.shared_secret_hint.empty());
    std::cout << "  Encapsulation completed" << std::endl;

    // Test decapsulation
    auto recovered_ss = kem.decapsulate(pk, sk, ct);
    assert(recovered_ss == ss);
    std::cout << "  Decapsulation completed and verified" << std::endl;

    std::cout << "ColorKEM with HSM tests passed!" << std::endl;
}
#endif

void test_colorkem_without_hsm() {
    std::cout << "Testing ColorKEM without HSM..." << std::endl;

    // Ensure no HSM
    clwe::hsm::HSMConfig no_hsm_config;
    clwe::hsm::set_global_hsm_config(no_hsm_config);

#ifdef ENABLE_HSM
    // Create ColorKEM instance without HSM
    clwe::CLWEParameters params(512);
    clwe::ColorKEM kem(params);

    // Test that it still works
    auto [pk, sk] = kem.keygen();
    assert(!pk.public_data.empty());
    assert(!sk.secret_data.empty());
    std::cout << "  Key pair generated without HSM" << std::endl;

    auto [ct, ss] = kem.encapsulate(pk);
    auto recovered_ss = kem.decapsulate(pk, sk, ct);
    assert(recovered_ss == ss);
    std::cout << "  KEM operations work without HSM" << std::endl;
#endif

    std::cout << "ColorKEM without HSM tests passed!" << std::endl;
}

int main() {
    std::cout << "Running HSM Integration Tests..." << std::endl;

    try {
        test_software_hsm();
        test_hsm_config();
#ifdef ENABLE_HSM
        test_colorkem_with_hsm();
#endif
        test_colorkem_without_hsm();

        std::cout << "All HSM integration tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}
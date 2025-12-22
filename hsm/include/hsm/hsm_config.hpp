/**
 * @file hsm_config.hpp
 * @brief Configuration options for HSM integration in ColorKEM
 *
 * This header defines configuration options to enable or disable HSM usage
 * and specify HSM backend implementations.
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef HSM_CONFIG_HPP
#define HSM_CONFIG_HPP

#include <string>
#include <memory>

namespace clwe {
namespace hsm {

/**
 * @brief HSM backend types
 */
enum class HSMBackend {
    NONE,           /**< No HSM - software only operation */
    SOFTWARE_SIM,   /**< Software simulation for development/testing */
    HARDWARE,       /**< Real hardware HSM */
    PKCS11          /**< PKCS#11 compatible HSM */
};

/**
 * @brief HSM configuration structure
 */
struct HSMConfig {
    HSMBackend backend;               /**< Type of HSM backend to use */
    std::string library_path;         /**< Path to HSM library (for PKCS#11) */
    std::string config_file;          /**< Path to HSM configuration file */
    std::string pin;                  /**< PIN for HSM authentication */
    uint32_t slot_id;                 /**< PKCS#11 slot ID */
    bool enable_key_caching;          /**< Enable key caching in memory */
    bool require_hsm_for_private_keys; /**< Require HSM for private key operations */

    /**
     * @brief Default constructor with software-only configuration
     */
    HSMConfig() :
        backend(HSMBackend::NONE),
        library_path(""),
        config_file(""),
        pin(""),
        slot_id(0),
        enable_key_caching(false),
        require_hsm_for_private_keys(false) {}

    /**
     * @brief Constructor for software simulation
     */
    static HSMConfig softwareSimulation() {
        HSMConfig config;
        config.backend = HSMBackend::SOFTWARE_SIM;
        config.enable_key_caching = true;
        return config;
    }

    /**
     * @brief Constructor for PKCS#11 HSM
     */
    static HSMConfig pkcs11(const std::string& lib_path,
                           const std::string& pin_val = "",
                           uint32_t slot = 0) {
        HSMConfig config;
        config.backend = HSMBackend::PKCS11;
        config.library_path = lib_path;
        config.pin = pin_val;
        config.slot_id = slot;
        config.enable_key_caching = true;
        config.require_hsm_for_private_keys = true;
        return config;
    }

    /**
     * @brief Check if HSM is enabled
     */
    bool isEnabled() const {
        return backend != HSMBackend::NONE;
    }

    /**
     * @brief Check if software fallback is allowed
     */
    bool allowSoftwareFallback() const {
        return !require_hsm_for_private_keys;
    }
};

/**
 * @brief Global HSM configuration instance
 *
 * This can be set at application startup to configure HSM usage.
 */
extern HSMConfig global_hsm_config;

/**
 * @brief Set the global HSM configuration
 *
 * @param config New HSM configuration
 */
void set_global_hsm_config(const HSMConfig& config);

/**
 * @brief Get the current global HSM configuration
 *
 * @return Current HSM configuration
 */
const HSMConfig& get_global_hsm_config();

} // namespace hsm
} // namespace clwe

#endif // HSM_CONFIG_HPP
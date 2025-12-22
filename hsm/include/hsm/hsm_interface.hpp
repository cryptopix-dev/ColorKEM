/**
 * @file hsm_interface.hpp
 * @brief Hardware Security Module (HSM) abstraction interface for ColorKEM
 *
 * This header defines the abstract base class for HSM operations, providing
 * a unified interface for key management and cryptographic operations.
 * Implementations can be hardware-backed or software-simulated.
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef HSM_INTERFACE_HPP
#define HSM_INTERFACE_HPP

#include <vector>
#include <array>
#include <memory>
#include <string>

namespace clwe {
namespace hsm {

/**
 * @brief HSM key handle for referencing stored keys
 */
using KeyHandle = uint64_t;

/**
 * @brief Invalid key handle value
 */
const KeyHandle INVALID_KEY_HANDLE = 0;

/**
 * @brief Key types supported by the HSM
 */
enum class KeyType {
    SECRET_KEY,     /**< Secret/private key for lattice operations */
    PUBLIC_KEY,     /**< Public key for verification */
    SHARED_SECRET   /**< Ephemeral shared secret */
};

/**
 * @brief Cryptographic operation types
 */
enum class CryptoOperation {
    ENCRYPT,        /**< Encryption operation */
    DECRYPT,        /**< Decryption operation */
    SIGN,          /**< Digital signature */
    VERIFY,        /**< Signature verification */
    DERIVE         /**< Key derivation */
};

/**
 * @brief HSM operation result
 */
struct HSMResult {
    bool success;                   /**< Operation success flag */
    std::string error_message;      /**< Error message if operation failed */
    std::vector<uint8_t> data;      /**< Result data */

    HSMResult() : success(true) {}
    HSMResult(bool s, const std::string& msg = "") : success(s), error_message(msg) {}
};

/**
 * @brief Abstract base class defining HSM operations
 *
 * This interface provides methods for key management and cryptographic
 * operations in a hardware security module. Implementations should ensure
 * that sensitive key material never leaves the HSM boundary.
 */
class HSMInterface {
public:
    HSMInterface() = default;
    virtual ~HSMInterface() = default;

    /**
     * @brief Initialize the HSM
     *
     * Performs any necessary initialization, such as connecting to hardware
     * or setting up secure contexts.
     *
     * @return HSMResult indicating success or failure
     */
    virtual HSMResult initialize() = 0;

    /**
     * @brief Generate a new key pair
     *
     * Creates a new key pair within the HSM. The private key remains
     * stored in the HSM and is referenced by a handle.
     *
     * @param key_type Type of key to generate
     * @param key_size Size parameter for key generation
     * @return HSMResult with public key data and private key handle
     */
    virtual HSMResult generate_key(KeyType key_type, size_t key_size) = 0;

    /**
     * @brief Import a key into the HSM
     *
     * Imports key material into the HSM for secure storage.
     * The key data is securely erased from memory after import.
     *
     * @param key_type Type of key being imported
     * @param key_data Raw key material
     * @return HSMResult with key handle
     */
    virtual HSMResult import_key(KeyType key_type, const std::vector<uint8_t>& key_data) = 0;

    /**
     * @brief Export a public key from the HSM
     *
     * Exports public key data from the HSM. Private keys cannot be exported.
     *
     * @param key_handle Handle to the key
     * @return HSMResult with public key data
     */
    virtual HSMResult export_public_key(KeyHandle key_handle) = 0;

    /**
     * @brief Delete a key from the HSM
     *
     * Securely deletes a key from HSM storage.
     *
     * @param key_handle Handle to the key to delete
     * @return HSMResult indicating success or failure
     */
    virtual HSMResult delete_key(KeyHandle key_handle) = 0;

    /**
     * @brief Perform cryptographic operation
     *
     * Executes a cryptographic operation using keys stored in the HSM.
     *
     * @param operation Type of cryptographic operation
     * @param key_handle Handle to the key to use
     * @param input_data Input data for the operation
     * @param parameters Additional operation parameters
     * @return HSMResult with operation result
     */
    virtual HSMResult perform_crypto_operation(CryptoOperation operation,
                                             KeyHandle key_handle,
                                             const std::vector<uint8_t>& input_data,
                                             const std::vector<uint8_t>& parameters = {}) = 0;

    /**
     * @brief Get HSM status information
     *
     * Returns information about the HSM's current state and capabilities.
     *
     * @return HSMResult with status information
     */
    virtual HSMResult get_status() = 0;

    /**
     * @brief Check if a key handle is valid
     *
     * @param key_handle Handle to check
     * @return true if handle is valid, false otherwise
     */
    virtual bool is_valid_key_handle(KeyHandle key_handle) const = 0;

    // Disable copy and assignment
    HSMInterface(const HSMInterface&) = delete;
    HSMInterface& operator=(const HSMInterface&) = delete;
};

} // namespace hsm
} // namespace clwe

#endif // HSM_INTERFACE_HPP
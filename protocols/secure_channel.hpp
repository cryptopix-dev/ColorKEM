/**
 * @file secure_channel.hpp
 * @brief Secure Channel Protocol using ColorKEM-derived keys
 *
 * This header defines the structures and state machine for a secure
 * communication channel established using ColorKEM shared secrets.
 * The protocol provides authenticated encryption for data transmission.
 *
 * Protocol Flow:
 * 1. Channel established with shared key from key exchange
 * 2. Messages encrypted with AEAD using derived keys
 * 3. Sequence numbers prevent replay attacks
 *
 * Security Analysis:
 * - Confidentiality: AEAD encryption of all data
 * - Authentication: AEAD provides message authentication
 * - Forward secrecy: Keys derived from ephemeral KEM exchanges
 * - Replay protection: Sequence numbers and nonces
 * - Quantum resistance: Based on quantum-resistant key exchange
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef SECURE_CHANNEL_HPP
#define SECURE_CHANNEL_HPP

#include "clwe/color_kem.hpp"
#include <vector>
#include <array>
#include <cstdint>
#include <memory>

namespace clwe {
namespace protocols {

/**
 * @brief Protocol version identifier
 */
const uint16_t SECURE_CHANNEL_VERSION = 0x0100;

/**
 * @brief Message types for secure channel protocol
 */
enum class SecureChannelMessageType : uint8_t {
    CHANNEL_INIT = 1,
    ENCRYPTED_DATA = 2,
    CHANNEL_CLOSE = 3,
    ERROR = 255
};

/**
 * @brief Error codes for secure channel protocol
 */
enum class SecureChannelError : uint8_t {
    SUCCESS = 0,
    INVALID_VERSION = 1,
    INVALID_SEQUENCE = 2,
    DECRYPTION_FAILED = 3,
    AUTHENTICATION_FAILED = 4,
    CHANNEL_NOT_ESTABLISHED = 5,
    KEY_DERIVATION_FAILED = 6
};

/**
 * @brief Abstract AEAD cipher interface
 *
 * Defines the interface for authenticated encryption with associated data.
 * Implementations should provide quantum-resistant symmetric encryption
 * such as AES-GCM, ChaCha20-Poly1305, or Ascon.
 */
class AEADCipher {
public:
    virtual ~AEADCipher() = default;

    /**
     * @brief Get key size in bytes
     * @return Key size
     */
    virtual size_t key_size() const = 0;

    /**
     * @brief Get nonce size in bytes
     * @return Nonce size
     */
    virtual size_t nonce_size() const = 0;

    /**
     * @brief Get authentication tag size in bytes
     * @return Tag size
     */
    virtual size_t tag_size() const = 0;

    /**
     * @brief Encrypt and authenticate data
     * @param key Encryption key
     * @param nonce Unique nonce
     * @param plaintext Data to encrypt
     * @param associated_data Associated data (authenticated but not encrypted)
     * @return Pair of (ciphertext, authentication_tag)
     */
    virtual std::pair<std::vector<uint8_t>, std::vector<uint8_t>>
        encrypt(const std::vector<uint8_t>& key,
                const std::vector<uint8_t>& nonce,
                const std::vector<uint8_t>& plaintext,
                const std::vector<uint8_t>& associated_data) = 0;

    /**
     * @brief Decrypt and verify data
     * @param key Decryption key
     * @param nonce Nonce used for encryption
     * @param ciphertext Encrypted data
     * @param tag Authentication tag
     * @param associated_data Associated data
     * @return Plaintext if verification succeeds, empty vector otherwise
     */
    virtual std::vector<uint8_t>
        decrypt(const std::vector<uint8_t>& key,
                const std::vector<uint8_t>& nonce,
                const std::vector<uint8_t>& ciphertext,
                const std::vector<uint8_t>& tag,
                const std::vector<uint8_t>& associated_data) = 0;

    /**
     * @brief Get cipher identifier
     * @return Unique identifier for the cipher
     */
    virtual uint16_t get_cipher_id() const = 0;
};

/**
 * @brief Key derivation function interface
 *
 * Defines HKDF-like key derivation for deriving channel keys
 * from ColorKEM shared secrets.
 */
class KeyDerivationFunction {
public:
    virtual ~KeyDerivationFunction() = default;

    /**
     * @brief Derive keys from shared secret
     * @param shared_secret Input key material (ColorKEM shared secret)
     * @param context Protocol context string
     * @param key_length Length of derived key in bytes
     * @return Derived key
     */
    virtual std::vector<uint8_t> derive_key(const ColorValue& shared_secret,
                                           const std::string& context,
                                           size_t key_length) = 0;

    /**
     * @brief Get KDF identifier
     * @return Unique identifier for the KDF
     */
    virtual uint16_t get_kdf_id() const = 0;
};

/**
 * @brief Channel initialization message
 *
 * Establishes the secure channel parameters and confirms
 * that both parties have the shared key.
 */
struct ChannelInit {
    uint16_t version;                    /**< Protocol version */
    std::array<uint8_t, 32> channel_id;  /**< Unique channel identifier */
    uint16_t cipher_suite;              /**< AEAD cipher identifier */
    uint16_t kdf_suite;                 /**< Key derivation function identifier */
    std::vector<uint8_t> key_confirmation; /**< Proof of key possession */

    /**
     * @brief Serialize the message to bytes
     * @return Serialized message data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize from bytes
     * @param data Serialized message data
     * @return Deserialized ChannelInit
     */
    static ChannelInit deserialize(const std::vector<uint8_t>& data);
};

/**
 * @brief Encrypted data message
 *
 * Contains authenticated encrypted payload with sequence number
 * for replay protection.
 */
struct EncryptedData {
    uint16_t version;                    /**< Protocol version */
    std::array<uint8_t, 32> channel_id;  /**< Channel identifier */
    uint64_t sequence_number;           /**< Message sequence number */
    std::vector<uint8_t> nonce;         /**< AEAD nonce */
    std::vector<uint8_t> ciphertext;    /**< Encrypted payload */
    std::vector<uint8_t> auth_tag;      /**< Authentication tag */

    /**
     * @brief Serialize the message to bytes
     * @return Serialized message data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize from bytes
     * @param data Serialized message data
     * @return Deserialized EncryptedData
     */
    static EncryptedData deserialize(const std::vector<uint8_t>& data);
};

/**
 * @brief Channel close message
 *
 * Gracefully terminates the secure channel.
 */
struct ChannelClose {
    uint16_t version;                    /**< Protocol version */
    std::array<uint8_t, 32> channel_id;  /**< Channel identifier */
    std::vector<uint8_t> close_confirmation; /**< Proof of close intent */

    /**
     * @brief Serialize the message to bytes
     * @return Serialized message data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize from bytes
     * @param data Serialized message data
     * @return Deserialized ChannelClose
     */
    static ChannelClose deserialize(const std::vector<uint8_t>& data);
};

/**
 * @brief Error message for secure channel
 */
struct ChannelErrorMessage {
    uint16_t version;                    /**< Protocol version */
    std::array<uint8_t, 32> channel_id;  /**< Channel identifier */
    SecureChannelError error_code;      /**< Error code */
    std::string error_message;          /**< Human-readable error description */

    /**
     * @brief Serialize the message to bytes
     * @return Serialized message data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize from bytes
     * @param data Serialized message data
     * @return Deserialized ChannelErrorMessage
     */
    static ChannelErrorMessage deserialize(const std::vector<uint8_t>& data);
};

/**
 * @brief Secure Channel Protocol State Machine
 */
enum class SecureChannelState {
    INIT,               /**< Initial state */
    ESTABLISHING,       /**< Channel being established */
    ESTABLISHED,        /**< Channel established, ready for data */
    CLOSING,            /**< Channel being closed */
    CLOSED,             /**< Channel closed */
    ERROR               /**< Error state */
};

/**
 * @brief Secure Channel Session Context
 */
struct SecureChannelSession {
    SecureChannelState state;            /**< Current channel state */
    std::array<uint8_t, 32> channel_id;  /**< Unique channel identifier */
    ColorValue shared_secret;            /**< Shared secret from key exchange */

    // Cryptographic primitives
    std::unique_ptr<AEADCipher> cipher;
    std::unique_ptr<KeyDerivationFunction> kdf;

    // Derived keys
    std::vector<uint8_t> encryption_key; /**< Derived encryption key */
    std::vector<uint8_t> authentication_key; /**< Derived authentication key (if needed) */

    // Sequence numbers
    uint64_t send_sequence;              /**< Next sequence number to send */
    uint64_t receive_sequence;           /**< Next expected sequence number */

    /**
     * @brief Initialize a new secure channel
     * @param shared_secret Shared secret from key exchange
     * @param cipher AEAD cipher implementation
     * @param kdf Key derivation function
     */
    void initialize(const ColorValue& shared_secret,
                   std::unique_ptr<AEADCipher> cipher,
                   std::unique_ptr<KeyDerivationFunction> kdf);

    /**
     * @brief Generate a unique channel ID
     */
    void generate_channel_id();

    /**
     * @brief Derive channel keys from shared secret
     * @return True if key derivation succeeds
     */
    bool derive_keys();

    /**
     * @brief Validate sequence number
     * @param sequence Received sequence number
     * @return True if sequence is valid (no replay)
     */
    bool validate_sequence(uint64_t sequence);
};

/**
 * @brief Secure Channel Protocol API
 */
class SecureChannelProtocol {
private:
    ColorKEM& kem_;  /**< Reference to ColorKEM instance (for key confirmation) */

public:
    /**
     * @brief Construct protocol handler
     * @param kem ColorKEM instance for key operations
     */
    explicit SecureChannelProtocol(ColorKEM& kem) : kem_(kem) {}

    /**
     * @brief Initiate secure channel establishment
     * @param session Session context
     * @param shared_secret Shared secret from key exchange
     * @param cipher AEAD cipher to use
     * @param kdf Key derivation function to use
     * @return ChannelInit message to send
     */
    ChannelInit initiate_channel(SecureChannelSession& session,
                                const ColorValue& shared_secret,
                                std::unique_ptr<AEADCipher> cipher,
                                std::unique_ptr<KeyDerivationFunction> kdf);

    /**
     * @brief Process channel initialization
     * @param session Session context
     * @param init_message Received ChannelInit message
     * @param expected_shared_secret Expected shared secret
     * @param cipher AEAD cipher to use
     * @param kdf Key derivation function to use
     * @return True if channel established successfully
     */
    bool process_channel_init(SecureChannelSession& session,
                             const ChannelInit& init_message,
                             const ColorValue& expected_shared_secret,
                             std::unique_ptr<AEADCipher> cipher,
                             std::unique_ptr<KeyDerivationFunction> kdf);

    /**
     * @brief Encrypt and send data
     * @param session Session context
     * @param plaintext Data to encrypt
     * @param associated_data Associated data for authentication
     * @return EncryptedData message to send
     */
    EncryptedData encrypt_data(SecureChannelSession& session,
                              const std::vector<uint8_t>& plaintext,
                              const std::vector<uint8_t>& associated_data);

    /**
     * @brief Receive and decrypt data
     * @param session Session context
     * @param encrypted_message Received EncryptedData message
     * @param associated_data Expected associated data
     * @return Decrypted plaintext if successful, empty vector otherwise
     */
    std::vector<uint8_t> decrypt_data(SecureChannelSession& session,
                                     const EncryptedData& encrypted_message,
                                     const std::vector<uint8_t>& associated_data);

    /**
     * @brief Initiate channel closure
     * @param session Session context
     * @return ChannelClose message to send
     */
    ChannelClose close_channel(SecureChannelSession& session);

    /**
     * @brief Process channel closure
     * @param session Session context
     * @param close_message Received ChannelClose message
     * @return True if closure accepted
     */
    bool process_channel_close(SecureChannelSession& session, const ChannelClose& close_message);
};

} // namespace protocols
} // namespace clwe

#endif // SECURE_CHANNEL_HPP
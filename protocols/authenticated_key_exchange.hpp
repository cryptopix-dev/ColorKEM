/**
 * @file authenticated_key_exchange.hpp
 * @brief Authenticated Key Exchange Protocol using ColorKEM with Signatures
 *
 * This header defines the structures and state machine for an authenticated
 * quantum-resistant key exchange protocol. The protocol provides mutual
 * authentication using digital signatures combined with ColorKEM key establishment.
 *
 * Protocol Flow:
 * 1. Client sends SignedClientHello with signature over its public key
 * 2. Server verifies signature and responds with SignedServerHello
 * 3. Both parties derive authenticated shared key
 *
 * Security Analysis:
 * - Mutual authentication: Both parties prove identity via signatures
 * - Forward secrecy: Unique shared secret per exchange
 * - Quantum resistance: Lattice-based KEM + hash-based signatures
 * - Key compromise impersonation resistance: Signature-based authentication
 * - Unknown key-share attack resistance: Signature verification prevents
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef AUTHENTICATED_KEY_EXCHANGE_HPP
#define AUTHENTICATED_KEY_EXCHANGE_HPP

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
const uint16_t AUTH_KEY_EXCHANGE_VERSION = 0x0100;

/**
 * @brief Message types for authenticated key exchange protocol
 */
enum class AuthKeyExchangeMessageType : uint8_t {
    SIGNED_CLIENT_HELLO = 1,
    SIGNED_SERVER_HELLO = 2,
    ERROR = 255
};

/**
 * @brief Error codes for authenticated key exchange protocol
 */
enum class AuthKeyExchangeError : uint8_t {
    SUCCESS = 0,
    INVALID_VERSION = 1,
    INVALID_SIGNATURE = 2,
    INVALID_PARAMETERS = 3,
    KEY_GENERATION_FAILED = 4,
    SIGNATURE_VERIFICATION_FAILED = 5,
    PROTOCOL_VIOLATION = 6
};

/**
 * @brief Abstract signature interface
 *
 * Defines the interface for digital signature operations.
 * Implementations should provide quantum-resistant signature schemes
 * such as XMSS, LMS, or Dilithium.
 */
class SignatureScheme {
public:
    virtual ~SignatureScheme() = default;

    /**
     * @brief Generate a new key pair
     * @return Pair of (public_key, private_key) as byte vectors
     */
    virtual std::pair<std::vector<uint8_t>, std::vector<uint8_t>> generate_keypair() = 0;

    /**
     * @brief Sign a message
     * @param private_key Private key for signing
     * @param message Message to sign
     * @return Signature as byte vector
     */
    virtual std::vector<uint8_t> sign(const std::vector<uint8_t>& private_key,
                                     const std::vector<uint8_t>& message) = 0;

    /**
     * @brief Verify a signature
     * @param public_key Public key for verification
     * @param message Original message
     * @param signature Signature to verify
     * @return True if signature is valid
     */
    virtual bool verify(const std::vector<uint8_t>& public_key,
                       const std::vector<uint8_t>& message,
                       const std::vector<uint8_t>& signature) = 0;

    /**
     * @brief Get signature scheme identifier
     * @return Unique identifier for the scheme
     */
    virtual uint16_t get_scheme_id() const = 0;
};

/**
 * @brief SignedClientHello message structure
 *
 * Initiates authenticated key exchange with signed client identity.
 * Includes client's ColorKEM public key and signature over the key material.
 */
struct SignedClientHello {
    uint16_t version;                    /**< Protocol version */
    std::array<uint8_t, 32> session_id;  /**< Unique session identifier */
    uint16_t signature_scheme;          /**< Signature scheme identifier */
    std::vector<uint8_t> client_public_key_sig; /**< Client's signature public key */
    ColorPublicKey kem_public_key;      /**< Client's ColorKEM public key */
    std::vector<uint8_t> signature;     /**< Signature over (session_id || kem_public_key) */

    /**
     * @brief Serialize the message to bytes
     * @return Serialized message data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize from bytes
     * @param data Serialized message data
     * @param params CLWE parameters for deserialization
     * @return Deserialized SignedClientHello
     */
    static SignedClientHello deserialize(const std::vector<uint8_t>& data, const CLWEParameters& params);

    /**
     * @brief Verify the signature
     * @param sig_scheme Signature scheme implementation
     * @return True if signature is valid
     */
    bool verify_signature(const SignatureScheme& sig_scheme) const;
};

/**
 * @brief SignedServerHello message structure
 *
 * Responds to SignedClientHello with authenticated server identity
 * and encapsulated shared secret.
 */
struct SignedServerHello {
    uint16_t version;                    /**< Protocol version */
    std::array<uint8_t, 32> session_id;  /**< Session identifier (echoed) */
    uint16_t signature_scheme;          /**< Signature scheme identifier */
    std::vector<uint8_t> server_public_key_sig; /**< Server's signature public key */
    ColorCiphertext ciphertext;         /**< Encapsulated shared secret */
    std::vector<uint8_t> signature;     /**< Signature over (session_id || ciphertext) */

    /**
     * @brief Serialize the message to bytes
     * @return Serialized message data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize from bytes
     * @param data Serialized message data
     * @param params CLWE parameters for deserialization
     * @return Deserialized SignedServerHello
     */
    static SignedServerHello deserialize(const std::vector<uint8_t>& data, const CLWEParameters& params);

    /**
     * @brief Verify the signature
     * @param sig_scheme Signature scheme implementation
     * @return True if signature is valid
     */
    bool verify_signature(const SignatureScheme& sig_scheme) const;
};

/**
 * @brief Error message structure for authenticated exchange
 */
struct AuthErrorMessage {
    uint16_t version;                    /**< Protocol version */
    std::array<uint8_t, 32> session_id;  /**< Session identifier */
    AuthKeyExchangeError error_code;    /**< Error code */
    std::string error_message;          /**< Human-readable error description */

    /**
     * @brief Serialize the message to bytes
     * @return Serialized message data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize from bytes
     * @param data Serialized message data
     * @return Deserialized AuthErrorMessage
     */
    static AuthErrorMessage deserialize(const std::vector<uint8_t>& data);
};

/**
 * @brief Authenticated Key Exchange Protocol State Machine
 */
enum class AuthKeyExchangeState {
    INIT,                        /**< Initial state */
    SENT_SIGNED_CLIENT_HELLO,    /**< Client has sent SignedClientHello */
    RECEIVED_SIGNED_CLIENT_HELLO,/**< Server has received SignedClientHello */
    SENT_SIGNED_SERVER_HELLO,    /**< Server has sent SignedServerHello */
    RECEIVED_SIGNED_SERVER_HELLO,/**< Client has received SignedServerHello */
    AUTHENTICATED,               /**< Mutual authentication completed */
    ERROR                        /**< Error state */
};

/**
 * @brief Authenticated Key Exchange Session Context
 */
struct AuthKeyExchangeSession {
    AuthKeyExchangeState state;          /**< Current protocol state */
    std::array<uint8_t, 32> session_id;  /**< Unique session identifier */
    CLWEParameters params;               /**< Cryptographic parameters */

    // Signature scheme
    std::unique_ptr<SignatureScheme> signature_scheme;

    // Client-side data
    std::vector<uint8_t> client_sig_private_key; /**< Client's signature private key */
    std::vector<uint8_t> client_sig_public_key;  /**< Client's signature public key */
    ColorPrivateKey client_kem_private_key;     /**< Client's KEM private key */
    ColorPublicKey client_kem_public_key;       /**< Client's KEM public key */

    // Server-side data
    std::vector<uint8_t> server_sig_public_key;  /**< Server's signature public key */
    ColorPublicKey server_kem_public_key;       /**< Server's KEM public key */

    // Shared secret
    ColorValue shared_secret;                    /**< Derived shared secret */

    /**
     * @brief Initialize a new authenticated session
     * @param kem ColorKEM instance
     * @param sig_scheme Signature scheme implementation
     * @param is_client True if this is a client session
     */
    void initialize(ColorKEM& kem, std::unique_ptr<SignatureScheme> sig_scheme, bool is_client);

    /**
     * @brief Generate a unique session ID
     */
    void generate_session_id();

    /**
     * @brief Validate session state transition
     * @param new_state The proposed new state
     * @return True if transition is valid
     */
    bool validate_state_transition(AuthKeyExchangeState new_state) const;
};

/**
 * @brief Authenticated Key Exchange Protocol API
 */
class AuthenticatedKeyExchangeProtocol {
private:
    ColorKEM& kem_;  /**< Reference to ColorKEM instance */

public:
    /**
     * @brief Construct protocol handler
     * @param kem ColorKEM instance to use for cryptographic operations
     */
    explicit AuthenticatedKeyExchangeProtocol(ColorKEM& kem) : kem_(kem) {}

    /**
     * @brief Initiate authenticated key exchange as client
     * @param session Session context (will be initialized)
     * @param sig_scheme Signature scheme for the session
     * @return SignedClientHello message to send
     */
    SignedClientHello initiate_exchange(AuthKeyExchangeSession& session,
                                       std::unique_ptr<SignatureScheme> sig_scheme);

    /**
     * @brief Process SignedClientHello as server
     * @param session Session context
     * @param client_hello Received SignedClientHello message
     * @param sig_scheme Signature scheme for verification
     * @return SignedServerHello message to send
     */
    SignedServerHello process_client_hello(AuthKeyExchangeSession& session,
                                          const SignedClientHello& client_hello,
                                          const SignatureScheme& sig_scheme);

    /**
     * @brief Process SignedServerHello as client
     * @param session Session context
     * @param server_hello Received SignedServerHello message
     * @param sig_scheme Signature scheme for verification
     * @return True if exchange completed successfully
     */
    bool process_server_hello(AuthKeyExchangeSession& session,
                              const SignedServerHello& server_hello,
                              const SignatureScheme& sig_scheme);

    /**
     * @brief Get the established shared secret
     * @param session Session context
     * @return Shared secret if exchange is complete, empty otherwise
     */
    ColorValue get_shared_secret(const AuthKeyExchangeSession& session) const;
};

} // namespace protocols
} // namespace clwe

#endif // AUTHENTICATED_KEY_EXCHANGE_HPP
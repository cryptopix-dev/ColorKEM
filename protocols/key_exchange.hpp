/**
 * @file key_exchange.hpp
 * @brief Basic Key Exchange Protocol using ColorKEM
 *
 * This header defines the structures and state machine for a basic
 * quantum-resistant key exchange protocol based on ColorKEM. The protocol
 * provides authenticated key establishment using lattice-based cryptography.
 *
 * Protocol Flow:
 * 1. Client sends ClientHello with its public key
 * 2. Server responds with ServerHello containing encapsulated shared secret
 * 3. Both parties derive the shared key from the exchange
 *
 * Security Analysis:
 * - Forward secrecy: Each exchange generates a unique shared secret
 * - Quantum resistance: Based on ML-KEM security assumptions
 * - Authentication: Implicit through KEM decapsulation (IND-CCA2 security)
 * - Key compromise impersonation resistance: Provided by KEM properties
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef KEY_EXCHANGE_HPP
#define KEY_EXCHANGE_HPP

#include "clwe/color_kem.hpp"
#include <vector>
#include <array>
#include <cstdint>

namespace clwe {
namespace protocols {

/**
 * @brief Protocol version identifier
 */
const uint16_t KEY_EXCHANGE_VERSION = 0x0100;

/**
 * @brief Message types for key exchange protocol
 */
enum class KeyExchangeMessageType : uint8_t {
    CLIENT_HELLO = 1,
    SERVER_HELLO = 2,
    ERROR = 255
};

/**
 * @brief Error codes for key exchange protocol
 */
enum class KeyExchangeError : uint8_t {
    SUCCESS = 0,
    INVALID_VERSION = 1,
    INVALID_PARAMETERS = 2,
    KEY_GENERATION_FAILED = 3,
    ENCAPSULATION_FAILED = 4,
    DECAPSULATION_FAILED = 5,
    PROTOCOL_VIOLATION = 6
};

/**
 * @brief ClientHello message structure
 *
 * Initiates the key exchange by sending the client's public key.
 * The server will use this to encapsulate a shared secret.
 */
struct ClientHello {
    uint16_t version;                    /**< Protocol version */
    std::array<uint8_t, 32> session_id;  /**< Unique session identifier */
    ColorPublicKey public_key;          /**< Client's ColorKEM public key */

    /**
     * @brief Serialize the message to bytes
     * @return Serialized message data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize from bytes
     * @param data Serialized message data
     * @param params CLWE parameters for deserialization
     * @return Deserialized ClientHello
     */
    static ClientHello deserialize(const std::vector<uint8_t>& data, const CLWEParameters& params);
};

/**
 * @brief ServerHello message structure
 *
 * Responds to ClientHello with the encapsulated shared secret.
 * The client can decapsulate this to obtain the shared key.
 */
struct ServerHello {
    uint16_t version;                    /**< Protocol version */
    std::array<uint8_t, 32> session_id;  /**< Session identifier (echoed) */
    ColorCiphertext ciphertext;          /**< Encapsulated shared secret */

    /**
     * @brief Serialize the message to bytes
     * @return Serialized message data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize from bytes
     * @param data Serialized message data
     * @param params CLWE parameters for deserialization
     * @return Deserialized ServerHello
     */
    static ServerHello deserialize(const std::vector<uint8_t>& data, const CLWEParameters& params);
};

/**
 * @brief Error message structure
 *
 * Sent when a protocol error occurs during key exchange.
 */
struct ErrorMessage {
    uint16_t version;                    /**< Protocol version */
    std::array<uint8_t, 32> session_id;  /**< Session identifier */
    KeyExchangeError error_code;        /**< Error code */
    std::string error_message;          /**< Human-readable error description */

    /**
     * @brief Serialize the message to bytes
     * @return Serialized message data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize from bytes
     * @param data Serialized message data
     * @return Deserialized ErrorMessage
     */
    static ErrorMessage deserialize(const std::vector<uint8_t>& data);
};

/**
 * @brief Key Exchange Protocol State Machine
 *
 * Defines the states for the key exchange protocol participants.
 * The state machine ensures proper protocol flow and prevents
 * replay attacks or protocol violations.
 */
enum class KeyExchangeState {
    INIT,                    /**< Initial state */
    SENT_CLIENT_HELLO,       /**< Client has sent ClientHello */
    RECEIVED_CLIENT_HELLO,   /**< Server has received ClientHello */
    SENT_SERVER_HELLO,       /**< Server has sent ServerHello */
    RECEIVED_SERVER_HELLO,   /**< Client has received ServerHello */
    ESTABLISHED,             /**< Key exchange completed successfully */
    ERROR                    /**< Error state */
};

/**
 * @brief Key Exchange Session Context
 *
 * Maintains the state and cryptographic material for a key exchange session.
 * This structure is used by both client and server implementations.
 */
struct KeyExchangeSession {
    KeyExchangeState state;              /**< Current protocol state */
    std::array<uint8_t, 32> session_id;  /**< Unique session identifier */
    CLWEParameters params;               /**< Cryptographic parameters */

    // Client-side data
    ColorPrivateKey client_private_key;  /**< Client's private key */
    ColorPublicKey client_public_key;    /**< Client's public key */

    // Server-side data
    ColorPublicKey server_public_key;    /**< Server's public key (if needed) */

    // Shared secret
    ColorValue shared_secret;            /**< Derived shared secret */

    /**
     * @brief Initialize a new session
     * @param kem ColorKEM instance for key operations
     * @param is_client True if this is a client session
     */
    void initialize(ColorKEM& kem, bool is_client);

    /**
     * @brief Generate a unique session ID
     */
    void generate_session_id();

    /**
     * @brief Validate session state transition
     * @param new_state The proposed new state
     * @return True if transition is valid
     */
    bool validate_state_transition(KeyExchangeState new_state) const;
};

/**
 * @brief Key Exchange Protocol API
 *
 * Provides high-level functions for performing key exchange.
 * These functions handle the protocol logic and state management.
 */
class KeyExchangeProtocol {
private:
    ColorKEM& kem_;  /**< Reference to ColorKEM instance */

public:
    /**
     * @brief Construct protocol handler
     * @param kem ColorKEM instance to use for cryptographic operations
     */
    explicit KeyExchangeProtocol(ColorKEM& kem) : kem_(kem) {}

    /**
     * @brief Initiate key exchange as client
     * @param session Session context (will be initialized)
     * @return ClientHello message to send
     */
    ClientHello initiate_exchange(KeyExchangeSession& session);

    /**
     * @brief Process ClientHello as server
     * @param session Session context
     * @param client_hello Received ClientHello message
     * @return ServerHello message to send
     */
    ServerHello process_client_hello(KeyExchangeSession& session, const ClientHello& client_hello);

    /**
     * @brief Process ServerHello as client
     * @param session Session context
     * @param server_hello Received ServerHello message
     * @return True if exchange completed successfully
     */
    bool process_server_hello(KeyExchangeSession& session, const ServerHello& server_hello);

    /**
     * @brief Get the established shared secret
     * @param session Session context
     * @return Shared secret if exchange is complete, empty otherwise
     */
    ColorValue get_shared_secret(const KeyExchangeSession& session) const;
};

} // namespace protocols
} // namespace clwe

#endif // KEY_EXCHANGE_HPP
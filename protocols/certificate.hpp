/**
 * @file certificate.hpp
 * @brief Certificate Protocol for Quantum-Resistant Authentication
 *
 * This header defines the structures and state machine for certificate-based
 * authentication in quantum-resistant protocols. Certificates bind identities
 * to ColorKEM public keys with quantum-resistant signatures.
 *
 * Certificate Structure:
 * - Subject identity and public key
 * - Issuer identity and signature
 * - Validity period and serial number
 * - Quantum-resistant signature scheme
 *
 * Security Analysis:
 * - Identity binding: Cryptographically binds identity to public key
 * - Quantum resistance: Uses post-quantum signature algorithms
 * - Certificate validation: Signature verification and chain validation
 * - Revocation support: Certificate revocation structures
 * - Forward security: Short-lived certificates with frequent renewal
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef CERTIFICATE_HPP
#define CERTIFICATE_HPP

#include "clwe/color_kem.hpp"
#include <vector>
#include <array>
#include <cstdint>
#include <string>
#include <chrono>

namespace clwe {
namespace protocols {

/**
 * @brief Protocol version identifier
 */
const uint16_t CERTIFICATE_VERSION = 0x0100;

/**
 * @brief Certificate types
 */
enum class CertificateType : uint8_t {
    END_ENTITY = 1,     /**< End-entity certificate */
    INTERMEDIATE = 2,   /**< Intermediate CA certificate */
    ROOT = 3            /**< Root CA certificate */
};

/**
 * @brief Key usage flags for certificates
 */
enum class KeyUsage : uint16_t {
    DIGITAL_SIGNATURE = 0x0001,    /**< Digital signature */
    KEY_AGREEMENT = 0x0002,        /**< Key agreement (KEM) */
    KEY_CERT_SIGN = 0x0004,        /**< Certificate signing */
    CRL_SIGN = 0x0008             /**< CRL signing */
};

/**
 * @brief Certificate serial number
 */
using CertificateSerial = std::array<uint8_t, 16>;

/**
 * @brief Certificate validity period
 */
struct ValidityPeriod {
    std::chrono::system_clock::time_point not_before;  /**< Certificate valid from */
    std::chrono::system_clock::time_point not_after;   /**< Certificate valid until */

    /**
     * @brief Check if certificate is currently valid
     * @return True if current time is within validity period
     */
    bool is_valid() const;

    /**
     * @brief Serialize validity period
     * @return Serialized data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize validity period
     * @param data Serialized data
     * @return Deserialized ValidityPeriod
     */
    static ValidityPeriod deserialize(const std::vector<uint8_t>& data);
};

/**
 * @brief Certificate subject/issuer identity
 */
struct CertificateIdentity {
    std::string common_name;           /**< Common name */
    std::string organization;          /**< Organization name */
    std::string organizational_unit;   /**< Organizational unit */
    std::string country;               /**< Country code */
    std::string state;                 /**< State/province */
    std::string locality;              /**< Locality/city */

    /**
     * @brief Serialize identity
     * @return Serialized data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize identity
     * @param data Serialized data
     * @return Deserialized CertificateIdentity
     */
    static CertificateIdentity deserialize(const std::vector<uint8_t>& data);
};

/**
 * @brief Quantum-Resistant Certificate Structure
 *
 * Represents a public key certificate that binds an identity to a
 * ColorKEM public key using quantum-resistant signatures.
 */
struct QuantumResistantCertificate {
    uint16_t version;                          /**< Certificate format version */
    CertificateSerial serial_number;           /**< Unique certificate serial */
    CertificateType cert_type;                 /**< Type of certificate */
    CertificateIdentity subject;               /**< Certificate subject */
    CertificateIdentity issuer;                /**< Certificate issuer */
    ValidityPeriod validity;                   /**< Validity period */
    uint16_t key_usage;                        /**< Key usage flags */
    uint16_t signature_algorithm;              /**< Signature algorithm ID */

    // Public key data
    ColorPublicKey public_key;                 /**< ColorKEM public key */
    std::vector<uint8_t> signature_public_key; /**< Signature verification key */

    // Signature
    std::vector<uint8_t> signature;            /**< Certificate signature */

    /**
     * @brief Serialize certificate to bytes
     * @return Serialized certificate data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize certificate from bytes
     * @param data Serialized certificate data
     * @param params CLWE parameters for deserialization
     * @return Deserialized certificate
     */
    static QuantumResistantCertificate deserialize(const std::vector<uint8_t>& data,
                                                  const CLWEParameters& params);

    /**
     * @brief Verify certificate signature
     * @param sig_scheme Signature scheme implementation
     * @param issuer_cert Issuer certificate (for chain validation)
     * @return True if signature is valid
     */
    bool verify_signature(const SignatureScheme& sig_scheme,
                         const QuantumResistantCertificate* issuer_cert = nullptr) const;

    /**
     * @brief Check certificate validity
     * @return True if certificate is valid (not expired, not revoked)
     */
    bool is_valid() const;

    /**
     * @brief Get certificate fingerprint (hash)
     * @return SHA-256 fingerprint of certificate
     */
    std::array<uint8_t, 32> get_fingerprint() const;
};

/**
 * @brief Certificate Revocation List entry
 */
struct CertificateRevocation {
    CertificateSerial serial_number;           /**< Revoked certificate serial */
    std::chrono::system_clock::time_point revocation_date; /**< Revocation time */
    uint8_t reason_code;                       /**< Revocation reason */

    /**
     * @brief Serialize revocation entry
     * @return Serialized data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize revocation entry
     * @param data Serialized data
     * @return Deserialized CertificateRevocation
     */
    static CertificateRevocation deserialize(const std::vector<uint8_t>& data);
};

/**
 * @brief Certificate Revocation List
 */
struct CertificateRevocationList {
    uint16_t version;                          /**< CRL format version */
    CertificateIdentity issuer;                /**< CRL issuer */
    std::chrono::system_clock::time_point this_update;     /**< CRL creation time */
    std::chrono::system_clock::time_point next_update;     /**< Next CRL update time */
    std::vector<CertificateRevocation> revoked_certs;      /**< Revoked certificates */
    std::vector<uint8_t> signature;            /**< CRL signature */

    /**
     * @brief Serialize CRL to bytes
     * @return Serialized CRL data
     */
    std::vector<uint8_t> serialize() const;

    /**
     * @brief Deserialize CRL from bytes
     * @param data Serialized CRL data
     * @return Deserialized CRL
     */
    static CertificateRevocationList deserialize(const std::vector<uint8_t>& data);

    /**
     * @brief Check if certificate is revoked
     * @param serial Certificate serial number
     * @return True if certificate is revoked
     */
    bool is_revoked(const CertificateSerial& serial) const;

    /**
     * @brief Verify CRL signature
     * @param sig_scheme Signature scheme implementation
     * @param issuer_cert Issuer certificate
     * @return True if signature is valid
     */
    bool verify_signature(const SignatureScheme& sig_scheme,
                         const QuantumResistantCertificate& issuer_cert) const;
};

/**
 * @brief Certificate Chain Validation Result
 */
struct CertificateValidationResult {
    bool valid;                    /**< Overall validation result */
    std::string error_message;     /**< Error message if validation failed */
    std::vector<std::string> warnings; /**< Validation warnings */

    CertificateValidationResult() : valid(false) {}
    CertificateValidationResult(bool v, const std::string& msg = "")
        : valid(v), error_message(msg) {}
};

/**
 * @brief Certificate Authority operations
 */
class CertificateAuthority {
private:
    QuantumResistantCertificate ca_cert_;      /**< CA certificate */
    std::vector<uint8_t> ca_private_key_;     /**< CA signature private key */
    std::unique_ptr<SignatureScheme> sig_scheme_; /**< Signature scheme */

public:
    /**
     * @brief Initialize CA with root certificate
     * @param ca_cert CA certificate
     * @param ca_private_key CA private key
     * @param sig_scheme Signature scheme
     */
    CertificateAuthority(const QuantumResistantCertificate& ca_cert,
                        const std::vector<uint8_t>& ca_private_key,
                        std::unique_ptr<SignatureScheme> sig_scheme);

    /**
     * @brief Issue a new certificate
     * @param subject Subject identity
     * @param public_key Subject's ColorKEM public key
     * @param sig_public_key Subject's signature public key
     * @param validity Certificate validity period
     * @param key_usage Key usage flags
     * @param cert_type Certificate type
     * @return Issued certificate
     */
    QuantumResistantCertificate issue_certificate(const CertificateIdentity& subject,
                                                 const ColorPublicKey& public_key,
                                                 const std::vector<uint8_t>& sig_public_key,
                                                 const ValidityPeriod& validity,
                                                 uint16_t key_usage,
                                                 CertificateType cert_type);

    /**
     * @brief Revoke a certificate
     * @param serial Certificate serial to revoke
     * @param reason Revocation reason
     * @param crl Current CRL to update
     * @return Updated CRL
     */
    CertificateRevocationList revoke_certificate(const CertificateSerial& serial,
                                                uint8_t reason,
                                                const CertificateRevocationList& crl);

    /**
     * @brief Get CA certificate
     * @return CA certificate
     */
    const QuantumResistantCertificate& get_ca_certificate() const { return ca_cert_; }
};

/**
 * @brief Certificate Validation Engine
 */
class CertificateValidator {
private:
    std::vector<QuantumResistantCertificate> trusted_cas_; /**< Trusted root CAs */
    std::unique_ptr<SignatureScheme> sig_scheme_; /**< Signature scheme */

public:
    /**
     * @brief Initialize validator with trusted CAs
     * @param trusted_cas List of trusted root CA certificates
     * @param sig_scheme Signature scheme for verification
     */
    CertificateValidator(const std::vector<QuantumResistantCertificate>& trusted_cas,
                        std::unique_ptr<SignatureScheme> sig_scheme);

    /**
     * @brief Validate certificate chain
     * @param cert Certificate to validate
     * @param intermediate_certs Intermediate certificates
     * @param crl Certificate revocation list
     * @return Validation result
     */
    CertificateValidationResult validate_certificate(const QuantumResistantCertificate& cert,
                                                    const std::vector<QuantumResistantCertificate>& intermediate_certs,
                                                    const CertificateRevocationList* crl = nullptr);

    /**
     * @brief Validate certificate chain with CRL check
     * @param cert Certificate to validate
     * @param intermediate_certs Intermediate certificates
     * @param crls Certificate revocation lists
     * @return Validation result
     */
    CertificateValidationResult validate_certificate(const QuantumResistantCertificate& cert,
                                                    const std::vector<QuantumResistantCertificate>& intermediate_certs,
                                                    const std::vector<CertificateRevocationList>& crls);
};

} // namespace protocols
} // namespace clwe

#endif // CERTIFICATE_HPP
/**
 * @file pkcs11_layer.hpp
 * @brief Basic PKCS#11 compatibility layer for ColorKEM HSM integration
 *
 * This header provides basic PKCS#11 structures and constants for
 * compatibility with standard HSM interfaces. It defines the minimal
 * set of structures needed for HSM operations.
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef PKCS11_LAYER_HPP
#define PKCS11_LAYER_HPP

#include <cstdint>
#include <vector>
#include <string>

namespace clwe {
namespace hsm {
namespace pkcs11 {

/**
 * @brief PKCS#11 return values
 */
enum CK_RV {
    CKR_OK = 0x00000000,                    /**< Operation successful */
    CKR_CANCEL = 0x00000001,                /**< Operation cancelled */
    CKR_HOST_MEMORY = 0x00000002,           /**< Insufficient host memory */
    CKR_SLOT_ID_INVALID = 0x00000003,       /**< Invalid slot ID */
    CKR_GENERAL_ERROR = 0x00000005,         /**< General error */
    CKR_FUNCTION_FAILED = 0x00000006,       /**< Function failed */
    CKR_ARGUMENTS_BAD = 0x00000007,         /**< Invalid arguments */
    CKR_NO_EVENT = 0x00000008,              /**< No event */
    CKR_NEED_TO_CREATE_THREADS = 0x00000009, /**< Need to create threads */
    CKR_CANT_LOCK = 0x0000000A,             /**< Can't lock */
    CKR_ATTRIBUTE_READ_ONLY = 0x00000010,   /**< Attribute read-only */
    CKR_ATTRIBUTE_SENSITIVE = 0x00000011,   /**< Attribute sensitive */
    CKR_ATTRIBUTE_TYPE_INVALID = 0x00000012, /**< Invalid attribute type */
    CKR_ATTRIBUTE_VALUE_INVALID = 0x00000013, /**< Invalid attribute value */
    CKR_DATA_INVALID = 0x00000020,          /**< Invalid data */
    CKR_DATA_LEN_RANGE = 0x00000021,        /**< Data length range */
    CKR_DEVICE_ERROR = 0x00000030,          /**< Device error */
    CKR_DEVICE_MEMORY = 0x00000031,         /**< Device memory */
    CKR_DEVICE_REMOVED = 0x00000032,        /**< Device removed */
    CKR_ENCRYPTED_DATA_INVALID = 0x00000040, /**< Invalid encrypted data */
    CKR_ENCRYPTED_DATA_LEN_RANGE = 0x00000041, /**< Encrypted data length range */
    CKR_FUNCTION_CANCELED = 0x00000050,     /**< Function canceled */
    CKR_FUNCTION_NOT_PARALLEL = 0x00000051, /**< Function not parallel */
    CKR_FUNCTION_NOT_SUPPORTED = 0x00000054, /**< Function not supported */
    CKR_KEY_HANDLE_INVALID = 0x00000060,    /**< Invalid key handle */
    CKR_KEY_SIZE_RANGE = 0x00000062,        /**< Key size range */
    CKR_KEY_TYPE_INCONSISTENT = 0x00000063, /**< Key type inconsistent */
    CKR_KEY_NOT_NEEDED = 0x00000064,        /**< Key not needed */
    CKR_KEY_CHANGED = 0x00000065,           /**< Key changed */
    CKR_KEY_NEEDED = 0x00000066,            /**< Key needed */
    CKR_KEY_INDIGESTIBLE = 0x00000067,      /**< Key not digestible */
    CKR_KEY_FUNCTION_NOT_PERMITTED = 0x00000068, /**< Key function not permitted */
    CKR_KEY_NOT_WRAPPABLE = 0x00000069,     /**< Key not wrappable */
    CKR_KEY_UNEXTRACTABLE = 0x0000006A,     /**< Key unextractable */
    CKR_MECHANISM_INVALID = 0x00000070,     /**< Invalid mechanism */
    CKR_MECHANISM_PARAM_INVALID = 0x00000071, /**< Invalid mechanism parameter */
    CKR_OBJECT_HANDLE_INVALID = 0x00000082, /**< Invalid object handle */
    CKR_OPERATION_ACTIVE = 0x00000090,      /**< Operation active */
    CKR_OPERATION_NOT_INITIALIZED = 0x00000091, /**< Operation not initialized */
    CKR_PIN_INCORRECT = 0x000000A0,         /**< Incorrect PIN */
    CKR_PIN_INVALID = 0x000000A1,           /**< Invalid PIN */
    CKR_PIN_LEN_RANGE = 0x000000A2,         /**< PIN length range */
    CKR_PIN_EXPIRED = 0x000000A3,           /**< PIN expired */
    CKR_PIN_LOCKED = 0x000000A4,            /**< PIN locked */
    CKR_SESSION_CLOSED = 0x000000B0,        /**< Session closed */
    CKR_SESSION_COUNT = 0x000000B1,         /**< Session count */
    CKR_SESSION_HANDLE_INVALID = 0x000000B3, /**< Invalid session handle */
    CKR_SESSION_PARALLEL_NOT_SUPPORTED = 0x000000B4, /**< Parallel sessions not supported */
    CKR_SESSION_READ_ONLY = 0x000000B5,     /**< Session read-only */
    CKR_SESSION_EXISTS = 0x000000B6,        /**< Session exists */
    CKR_SESSION_READ_ONLY_EXISTS = 0x000000B7, /**< Read-only session exists */
    CKR_SESSION_READ_WRITE_SO_EXISTS = 0x000000B8, /**< Read-write SO session exists */
    CKR_SIGNATURE_INVALID = 0x000000C0,     /**< Invalid signature */
    CKR_SIGNATURE_LEN_RANGE = 0x000000C1,   /**< Signature length range */
    CKR_TEMPLATE_INCOMPLETE = 0x000000D0,   /**< Template incomplete */
    CKR_TEMPLATE_INCONSISTENT = 0x000000D1, /**< Template inconsistent */
    CKR_TOKEN_NOT_PRESENT = 0x000000E0,     /**< Token not present */
    CKR_TOKEN_NOT_RECOGNIZED = 0x000000E1, /**< Token not recognized */
    CKR_TOKEN_WRITE_PROTECTED = 0x000000E2, /**< Token write-protected */
    CKR_UNWRAPPING_KEY_HANDLE_INVALID = 0x000000F0, /**< Invalid unwrapping key handle */
    CKR_UNWRAPPING_KEY_SIZE_RANGE = 0x000000F1, /**< Unwrapping key size range */
    CKR_UNWRAPPING_KEY_TYPE_INCONSISTENT = 0x000000F2, /**< Unwrapping key type inconsistent */
    CKR_USER_ALREADY_LOGGED_IN = 0x00000100, /**< User already logged in */
    CKR_USER_NOT_LOGGED_IN = 0x00000101,     /**< User not logged in */
    CKR_USER_PIN_NOT_INITIALIZED = 0x00000102, /**< User PIN not initialized */
    CKR_USER_TYPE_INVALID = 0x00000103,      /**< Invalid user type */
    CKR_USER_ANOTHER_ALREADY_LOGGED_IN = 0x00000104, /**< Another user logged in */
    CKR_USER_TOO_MANY_TYPES = 0x00000105,    /**< Too many user types */
    CKR_WRAPPED_KEY_INVALID = 0x00000110,    /**< Invalid wrapped key */
    CKR_WRAPPED_KEY_LEN_RANGE = 0x00000112, /**< Wrapped key length range */
    CKR_WRAPPING_KEY_HANDLE_INVALID = 0x00000113, /**< Invalid wrapping key handle */
    CKR_WRAPPING_KEY_SIZE_RANGE = 0x00000114, /**< Wrapping key size range */
    CKR_WRAPPING_KEY_TYPE_INCONSISTENT = 0x00000115, /**< Wrapping key type inconsistent */
    CKR_RANDOM_SEED_NOT_SUPPORTED = 0x00000120, /**< Random seed not supported */
    CKR_RANDOM_NO_RNG = 0x00000121,           /**< No RNG */
    CKR_DOMAIN_PARAMS_INVALID = 0x00000130,   /**< Invalid domain parameters */
    CKR_BUFFER_TOO_SMALL = 0x00000150,       /**< Buffer too small */
    CKR_SAVED_STATE_INVALID = 0x00000160,     /**< Invalid saved state */
    CKR_INFORMATION_SENSITIVE = 0x00000170,   /**< Information sensitive */
    CKR_STATE_UNSAVEABLE = 0x00000180,        /**< State unsaveable */
    CKR_CRYPTOKI_NOT_INITIALIZED = 0x00000190, /**< Cryptoki not initialized */
    CKR_CRYPTOKI_ALREADY_INITIALIZED = 0x00000191, /**< Cryptoki already initialized */
    CKR_MUTEX_BAD = 0x000001A0,              /**< Bad mutex */
    CKR_MUTEX_NOT_LOCKED = 0x000001A1,       /**< Mutex not locked */
    CKR_NEW_PIN_MODE = 0x000001A2,           /**< New PIN mode */
    CKR_NEXT_OTP = 0x000001A3,               /**< Next OTP */
    CKR_EXCEEDED_MAX_ITERATIONS = 0x000001B5, /**< Exceeded max iterations */
    CKR_FIPS_SELF_TEST_FAILED = 0x000001B6,  /**< FIPS self-test failed */
    CKR_LIBRARY_LOAD_FAILED = 0x000001B7,    /**< Library load failed */
    CKR_PIN_TOO_WEAK = 0x000001B8,           /**< PIN too weak */
    CKR_PUBLIC_KEY_INVALID = 0x000001B9,     /**< Invalid public key */
    CKR_FUNCTION_REJECTED = 0x00000200       /**< Function rejected */
};

/**
 * @brief PKCS#11 object class types
 */
enum CK_OBJECT_CLASS {
    CKO_DATA = 0x00000000,              /**< Data object */
    CKO_CERTIFICATE = 0x00000001,       /**< Certificate */
    CKO_PUBLIC_KEY = 0x00000002,        /**< Public key */
    CKO_PRIVATE_KEY = 0x00000003,       /**< Private key */
    CKO_SECRET_KEY = 0x00000004,        /**< Secret key */
    CKO_HW_FEATURE = 0x00000005,        /**< Hardware feature */
    CKO_DOMAIN_PARAMETERS = 0x00000006, /**< Domain parameters */
    CKO_MECHANISM = 0x00000007,         /**< Mechanism */
    CKO_OTP_KEY = 0x00000008,           /**< OTP key */
    CKO_VENDOR_DEFINED = 0x80000000     /**< Vendor defined */
};

/**
 * @brief PKCS#11 key types
 */
enum CK_KEY_TYPE {
    CKK_RSA = 0x00000000,               /**< RSA key */
    CKK_DSA = 0x00000001,               /**< DSA key */
    CKK_DH = 0x00000002,                /**< Diffie-Hellman key */
    CKK_ECDSA = 0x00000003,             /**< ECDSA key */
    CKK_EC = 0x00000003,                /**< EC key (alias) */
    CKK_X9_42_DH = 0x00000004,          /**< X9.42 Diffie-Hellman key */
    CKK_KEA = 0x00000005,               /**< KEA key */
    CKK_GENERIC_SECRET = 0x00000010,    /**< Generic secret key */
    CKK_RC2 = 0x00000011,               /**< RC2 key */
    CKK_RC4 = 0x00000012,               /**< RC4 key */
    CKK_DES = 0x00000013,               /**< DES key */
    CKK_DES2 = 0x00000014,              /**< DES2 key */
    CKK_DES3 = 0x00000015,              /**< DES3 key */
    CKK_CAST = 0x00000016,              /**< CAST key */
    CKK_CAST3 = 0x00000017,             /**< CAST3 key */
    CKK_CAST5 = 0x00000018,             /**< CAST5 key */
    CKK_CAST128 = 0x00000018,           /**< CAST128 key (alias) */
    CKK_RC5 = 0x00000019,               /**< RC5 key */
    CKK_IDEA = 0x0000001A,              /**< IDEA key */
    CKK_SKIPJACK = 0x0000001B,          /**< Skipjack key */
    CKK_BATON = 0x0000001C,             /**< Baton key */
    CKK_JUNIPER = 0x0000001D,           /**< Juniper key */
    CKK_CDMF = 0x0000001E,              /**< CDMF key */
    CKK_AES = 0x0000001F,               /**< AES key */
    CKK_BLOWFISH = 0x00000020,          /**< Blowfish key */
    CKK_TWOFISH = 0x00000021,           /**< Twofish key */
    CKK_VENDOR_DEFINED = 0x80000000     /**< Vendor defined */
};

/**
 * @brief PKCS#11 attribute types
 */
enum CK_ATTRIBUTE_TYPE {
    CKA_CLASS = 0x00000000,             /**< Object class */
    CKA_TOKEN = 0x00000001,             /**< Token object */
    CKA_PRIVATE = 0x00000002,           /**< Private object */
    CKA_LABEL = 0x00000003,             /**< Object label */
    CKA_APPLICATION = 0x00000010,       /**< Application */
    CKA_VALUE = 0x00000011,             /**< Value */
    CKA_OBJECT_ID = 0x00000012,         /**< Object ID */
    CKA_CERTIFICATE_TYPE = 0x00000080,  /**< Certificate type */
    CKA_ISSUER = 0x00000081,            /**< Issuer */
    CKA_SERIAL_NUMBER = 0x00000082,     /**< Serial number */
    CKA_AC_ISSUER = 0x00000083,         /**< AC issuer */
    CKA_OWNER = 0x00000084,             /**< Owner */
    CKA_ATTR_TYPES = 0x00000085,        /**< Attribute types */
    CKA_TRUSTED = 0x00000086,           /**< Trusted */
    CKA_CERTIFICATE_CATEGORY = 0x00000087, /**< Certificate category */
    CKA_JAVA_MIDP_SECURITY_DOMAIN = 0x00000088, /**< Java MIDP security domain */
    CKA_URL = 0x00000089,               /**< URL */
    CKA_HASH_OF_SUBJECT_PUBLIC_KEY = 0x0000008A, /**< Hash of subject public key */
    CKA_HASH_OF_ISSUER_PUBLIC_KEY = 0x0000008B,  /**< Hash of issuer public key */
    CKA_NAME_HASH_ALGORITHM = 0x0000008C, /**< Name hash algorithm */
    CKA_CHECK_VALUE = 0x00000090,        /**< Check value */
    CKA_KEY_TYPE = 0x00000100,          /**< Key type */
    CKA_SUBJECT = 0x00000101,           /**< Subject */
    CKA_ID = 0x00000102,                /**< ID */
    CKA_SENSITIVE = 0x00000103,         /**< Sensitive */
    CKA_ENCRYPT = 0x00000104,           /**< Encrypt */
    CKA_DECRYPT = 0x00000105,           /**< Decrypt */
    CKA_WRAP = 0x00000106,              /**< Wrap */
    CKA_UNWRAP = 0x00000107,            /**< Unwrap */
    CKA_SIGN = 0x00000108,              /**< Sign */
    CKA_SIGN_RECOVER = 0x00000109,      /**< Sign recover */
    CKA_VERIFY = 0x0000010A,            /**< Verify */
    CKA_VERIFY_RECOVER = 0x0000010B,    /**< Verify recover */
    CKA_DERIVE = 0x0000010C,            /**< Derive */
    CKA_START_DATE = 0x00000110,        /**< Start date */
    CKA_END_DATE = 0x00000111,          /**< End date */
    CKA_MODULUS = 0x00000120,           /**< Modulus */
    CKA_MODULUS_BITS = 0x00000121,      /**< Modulus bits */
    CKA_PUBLIC_EXPONENT = 0x00000122,   /**< Public exponent */
    CKA_PRIVATE_EXPONENT = 0x00000123,  /**< Private exponent */
    CKA_PRIME_1 = 0x00000124,           /**< Prime 1 */
    CKA_PRIME_2 = 0x00000125,           /**< Prime 2 */
    CKA_EXPONENT_1 = 0x00000126,        /**< Exponent 1 */
    CKA_EXPONENT_2 = 0x00000127,        /**< Exponent 2 */
    CKA_COEFFICIENT = 0x00000128,       /**< Coefficient */
    CKA_PRIME = 0x00000130,             /**< Prime */
    CKA_SUBPRIME = 0x00000131,          /**< Subprime */
    CKA_BASE = 0x00000132,              /**< Base */
    CKA_PRIME_BITS = 0x00000133,        /**< Prime bits */
    CKA_SUBPRIME_BITS = 0x00000134,     /**< Subprime bits */
    CKA_VALUE_BITS = 0x00000160,        /**< Value bits */
    CKA_VALUE_LEN = 0x00000161,         /**< Value length */
    CKA_EXTRACTABLE = 0x00000162,       /**< Extractable */
    CKA_LOCAL = 0x00000163,             /**< Local */
    CKA_NEVER_EXTRACTABLE = 0x00000164, /**< Never extractable */
    CKA_ALWAYS_SENSITIVE = 0x00000165,  /**< Always sensitive */
    CKA_KEY_GEN_MECHANISM = 0x00000166, /**< Key generation mechanism */
    CKA_MODIFIABLE = 0x00000170,        /**< Modifiable */
    CKA_ECDSA_PARAMS = 0x00000180,      /**< ECDSA parameters */
    CKA_EC_PARAMS = 0x00000180,         /**< EC parameters (alias) */
    CKA_EC_POINT = 0x00000181,          /**< EC point */
    CKA_SECONDARY_AUTH = 0x00000200,    /**< Secondary auth */
    CKA_AUTH_PIN_FLAGS = 0x00000201,    /**< Auth PIN flags */
    CKA_ALWAYS_AUTHENTICATE = 0x00000202, /**< Always authenticate */
    CKA_WRAP_WITH_TRUSTED = 0x00000210, /**< Wrap with trusted */
    CKA_WRAP_TEMPLATE = 0x00000211,     /**< Wrap template */
    CKA_UNWRAP_TEMPLATE = 0x00000212,   /**< Unwrap template */
    CKA_DERIVE_TEMPLATE = 0x00000213,   /**< Derive template */
    CKA_OTP_FORMAT = 0x00000220,        /**< OTP format */
    CKA_OTP_LENGTH = 0x00000221,        /**< OTP length */
    CKA_OTP_TIME_INTERVAL = 0x00000222, /**< OTP time interval */
    CKA_OTP_USER_FRIENDLY_MODE = 0x00000223, /**< OTP user friendly mode */
    CKA_OTP_CHALLENGE_REQUIREMENT = 0x00000224, /**< OTP challenge requirement */
    CKA_OTP_TIME_REQUIREMENT = 0x00000225, /**< OTP time requirement */
    CKA_OTP_COUNTER_REQUIREMENT = 0x00000226, /**< OTP counter requirement */
    CKA_OTP_PIN_REQUIREMENT = 0x00000227, /**< OTP PIN requirement */
    CKA_OTP_COUNTER = 0x0000022E,        /**< OTP counter */
    CKA_OTP_TIME = 0x0000022F,           /**< OTP time */
    CKA_OTP_USER_IDENTIFIER = 0x0000022A, /**< OTP user identifier */
    CKA_OTP_SERVICE_IDENTIFIER = 0x0000022B, /**< OTP service identifier */
    CKA_OTP_SERVICE_LOGO = 0x0000022C,   /**< OTP service logo */
    CKA_OTP_SERVICE_LOGO_TYPE = 0x0000022D, /**< OTP service logo type */
    CKA_GOSTR3410_PARAMS = 0x00000250,   /**< GOST R 34.10 parameters */
    CKA_GOSTR3411_PARAMS = 0x00000251,   /**< GOST R 34.11 parameters */
    CKA_GOST28147_PARAMS = 0x00000252,   /**< GOST 28147 parameters */
    CKA_HW_FEATURE_TYPE = 0x00000300,    /**< Hardware feature type */
    CKA_RESET_ON_INIT = 0x00000301,      /**< Reset on init */
    CKA_HAS_RESET = 0x00000302,          /**< Has reset */
    CKA_PIXEL_X = 0x00000400,            /**< Pixel X */
    CKA_PIXEL_Y = 0x00000401,            /**< Pixel Y */
    CKA_RESOLUTION = 0x00000402,         /**< Resolution */
    CKA_CHAR_ROWS = 0x00000403,          /**< Character rows */
    CKA_CHAR_COLUMNS = 0x00000404,       /**< Character columns */
    CKA_COLOR = 0x00000405,              /**< Color */
    CKA_BITS_PER_PIXEL = 0x00000406,     /**< Bits per pixel */
    CKA_CHAR_SETS = 0x00000407,          /**< Character sets */
    CKA_ENCODING_METHODS = 0x00000408,   /**< Encoding methods */
    CKA_MIME_TYPES = 0x00000409,         /**< MIME types */
    CKA_MECHANISM_TYPE = 0x00000500,     /**< Mechanism type */
    CKA_REQUIRED_CMS_ATTRIBUTES = 0x00000501, /**< Required CMS attributes */
    CKA_DEFAULT_CMS_ATTRIBUTES = 0x00000502,  /**< Default CMS attributes */
    CKA_SUPPORTED_CMS_ATTRIBUTES = 0x00000503, /**< Supported CMS attributes */
    CKA_ALLOWED_MECHANISMS = 0x00000600,  /**< Allowed mechanisms */
    CKA_VENDOR_DEFINED = 0x80000000       /**< Vendor defined */
};

/**
 * @brief PKCS#11 mechanism types
 */
enum CK_MECHANISM_TYPE {
    CKM_RSA_PKCS_KEY_PAIR_GEN = 0x00000000, /**< RSA key pair generation */
    CKM_RSA_PKCS = 0x00000001,           /**< RSA PKCS */
    CKM_RSA_9796 = 0x00000002,           /**< RSA 9796 */
    CKM_RSA_X_509 = 0x00000003,          /**< RSA X.509 */
    CKM_MD2_RSA_PKCS = 0x00000004,       /**< MD2 RSA PKCS */
    CKM_MD5_RSA_PKCS = 0x00000005,       /**< MD5 RSA PKCS */
    CKM_SHA1_RSA_PKCS = 0x00000006,      /**< SHA1 RSA PKCS */
    CKM_RIPEMD128_RSA_PKCS = 0x00000007, /**< RIPEMD128 RSA PKCS */
    CKM_RIPEMD160_RSA_PKCS = 0x00000008, /**< RIPEMD160 RSA PKCS */
    CKM_RSA_PKCS_OAEP = 0x00000009,      /**< RSA PKCS OAEP */
    CKM_RSA_X9_31_KEY_PAIR_GEN = 0x0000000A, /**< RSA X9.31 key pair generation */
    CKM_RSA_X9_31 = 0x0000000B,          /**< RSA X9.31 */
    CKM_SHA1_RSA_X9_31 = 0x0000000C,     /**< SHA1 RSA X9.31 */
    CKM_RSA_PKCS_PSS = 0x0000000D,       /**< RSA PKCS PSS */
    CKM_SHA1_RSA_PKCS_PSS = 0x0000000E,  /**< SHA1 RSA PKCS PSS */
    CKM_DSA_KEY_PAIR_GEN = 0x00000010,   /**< DSA key pair generation */
    CKM_DSA = 0x00000011,                /**< DSA */
    CKM_DSA_SHA1 = 0x00000012,           /**< DSA SHA1 */
    CKM_DH_PKCS_KEY_PAIR_GEN = 0x00000020, /**< DH PKCS key pair generation */
    CKM_DH_PKCS_DERIVE = 0x00000021,     /**< DH PKCS derive */
    CKM_X9_42_DH_KEY_PAIR_GEN = 0x00000030, /**< X9.42 DH key pair generation */
    CKM_X9_42_DH_DERIVE = 0x00000031,    /**< X9.42 DH derive */
    CKM_X9_42_DH_HYBRID_DERIVE = 0x00000032, /**< X9.42 DH hybrid derive */
    CKM_X9_42_MQV_DERIVE = 0x00000033,   /**< X9.42 MQV derive */
    CKM_SHA256_RSA_PKCS = 0x00000040,    /**< SHA256 RSA PKCS */
    CKM_SHA384_RSA_PKCS = 0x00000041,    /**< SHA384 RSA PKCS */
    CKM_SHA512_RSA_PKCS = 0x00000042,    /**< SHA512 RSA PKCS */
    CKM_SHA256_RSA_PKCS_PSS = 0x00000043, /**< SHA256 RSA PKCS PSS */
    CKM_SHA384_RSA_PKCS_PSS = 0x00000044, /**< SHA384 RSA PKCS PSS */
    CKM_SHA512_RSA_PKCS_PSS = 0x00000045, /**< SHA512 RSA PKCS PSS */
    CKM_SHA224_RSA_PKCS = 0x00000046,    /**< SHA224 RSA PKCS */
    CKM_SHA224_RSA_PKCS_PSS = 0x00000047, /**< SHA224 RSA PKCS PSS */
    CKM_RC2_KEY_GEN = 0x00000100,        /**< RC2 key generation */
    CKM_RC2_ECB = 0x00000101,            /**< RC2 ECB */
    CKM_RC2_CBC = 0x00000102,            /**< RC2 CBC */
    CKM_RC2_MAC = 0x00000103,            /**< RC2 MAC */
    CKM_RC2_MAC_GENERAL = 0x00000104,    /**< RC2 MAC general */
    CKM_RC2_CBC_PAD = 0x00000105,        /**< RC2 CBC pad */
    CKM_RC4_KEY_GEN = 0x00000110,        /**< RC4 key generation */
    CKM_RC4 = 0x00000111,                /**< RC4 */
    CKM_DES_KEY_GEN = 0x00000120,        /**< DES key generation */
    CKM_DES_ECB = 0x00000121,            /**< DES ECB */
    CKM_DES_CBC = 0x00000122,            /**< DES CBC */
    CKM_DES_MAC = 0x00000123,            /**< DES MAC */
    CKM_DES_MAC_GENERAL = 0x00000124,    /**< DES MAC general */
    CKM_DES_CBC_PAD = 0x00000125,        /**< DES CBC pad */
    CKM_DES2_KEY_GEN = 0x00000130,       /**< DES2 key generation */
    CKM_DES3_KEY_GEN = 0x00000131,       /**< DES3 key generation */
    CKM_DES3_ECB = 0x00000132,           /**< DES3 ECB */
    CKM_DES3_CBC = 0x00000133,           /**< DES3 CBC */
    CKM_DES3_MAC = 0x00000134,           /**< DES3 MAC */
    CKM_DES3_MAC_GENERAL = 0x00000135,   /**< DES3 MAC general */
    CKM_DES3_CBC_PAD = 0x00000136,       /**< DES3 CBC pad */
    CKM_CDMF_KEY_GEN = 0x00000140,       /**< CDMF key generation */
    CKM_CDMF_ECB = 0x00000141,           /**< CDMF ECB */
    CKM_CDMF_CBC = 0x00000142,           /**< CDMF CBC */
    CKM_CDMF_MAC = 0x00000143,           /**< CDMF MAC */
    CKM_CDMF_MAC_GENERAL = 0x00000144,   /**< CDMF MAC general */
    CKM_CDMF_CBC_PAD = 0x00000145,       /**< CDMF CBC pad */
    CKM_DES_OFB64 = 0x00000150,          /**< DES OFB64 */
    CKM_DES_OFB8 = 0x00000151,           /**< DES OFB8 */
    CKM_DES_CFB64 = 0x00000152,          /**< DES CFB64 */
    CKM_DES_CFB8 = 0x00000153,           /**< DES CFB8 */
    CKM_MD2 = 0x00000200,                /**< MD2 */
    CKM_MD2_HMAC = 0x00000201,           /**< MD2 HMAC */
    CKM_MD2_HMAC_GENERAL = 0x00000202,   /**< MD2 HMAC general */
    CKM_MD5 = 0x00000210,                /**< MD5 */
    CKM_MD5_HMAC = 0x00000211,           /**< MD5 HMAC */
    CKM_MD5_HMAC_GENERAL = 0x00000212,   /**< MD5 HMAC general */
    CKM_SHA_1 = 0x00000220,              /**< SHA-1 */
    CKM_SHA_1_HMAC = 0x00000221,         /**< SHA-1 HMAC */
    CKM_SHA_1_HMAC_GENERAL = 0x00000222, /**< SHA-1 HMAC general */
    CKM_RIPEMD128 = 0x00000230,          /**< RIPEMD128 */
    CKM_RIPEMD128_HMAC = 0x00000231,     /**< RIPEMD128 HMAC */
    CKM_RIPEMD128_HMAC_GENERAL = 0x00000232, /**< RIPEMD128 HMAC general */
    CKM_RIPEMD160 = 0x00000240,          /**< RIPEMD160 */
    CKM_RIPEMD160_HMAC = 0x00000241,     /**< RIPEMD160 HMAC */
    CKM_RIPEMD160_HMAC_GENERAL = 0x00000242, /**< RIPEMD160 HMAC general */
    CKM_SHA256 = 0x00000250,             /**< SHA256 */
    CKM_SHA256_HMAC = 0x00000251,        /**< SHA256 HMAC */
    CKM_SHA256_HMAC_GENERAL = 0x00000252, /**< SHA256 HMAC general */
    CKM_SHA224 = 0x00000255,             /**< SHA224 */
    CKM_SHA224_HMAC = 0x00000256,        /**< SHA224 HMAC */
    CKM_SHA224_HMAC_GENERAL = 0x00000257, /**< SHA224 HMAC general */
    CKM_SHA384 = 0x00000260,             /**< SHA384 */
    CKM_SHA384_HMAC = 0x00000261,        /**< SHA384 HMAC */
    CKM_SHA384_HMAC_GENERAL = 0x00000262, /**< SHA384 HMAC general */
    CKM_SHA512 = 0x00000270,             /**< SHA512 */
    CKM_SHA512_HMAC = 0x00000271,        /**< SHA512 HMAC */
    CKM_SHA512_HMAC_GENERAL = 0x00000272, /**< SHA512 HMAC general */
    CKM_SECURID_KEY_GEN = 0x00000280,    /**< SecurID key generation */
    CKM_SECURID = 0x00000281,            /**< SecurID */
    CKM_HOTP_KEY_GEN = 0x00000290,       /**< HOTP key generation */
    CKM_HOTP = 0x00000291,               /**< HOTP */
    CKM_ACTI = 0x000002A0,               /**< ACTI */
    CKM_ACTI_KEY_GEN = 0x000002A1,       /**< ACTI key generation */
    CKM_CAST_KEY_GEN = 0x00000300,       /**< CAST key generation */
    CKM_CAST_ECB = 0x00000301,           /**< CAST ECB */
    CKM_CAST_CBC = 0x00000302,           /**< CAST CBC */
    CKM_CAST_MAC = 0x00000303,           /**< CAST MAC */
    CKM_CAST_MAC_GENERAL = 0x00000304,   /**< CAST MAC general */
    CKM_CAST_CBC_PAD = 0x00000305,       /**< CAST CBC pad */
    CKM_CAST3_KEY_GEN = 0x00000310,      /**< CAST3 key generation */
    CKM_CAST3_ECB = 0x00000311,          /**< CAST3 ECB */
    CKM_CAST3_CBC = 0x00000312,          /**< CAST3 CBC */
    CKM_CAST3_MAC = 0x00000313,          /**< CAST3 MAC */
    CKM_CAST3_MAC_GENERAL = 0x00000314,  /**< CAST3 MAC general */
    CKM_CAST3_CBC_PAD = 0x00000315,      /**< CAST3 CBC pad */
    CKM_CAST5_KEY_GEN = 0x00000320,      /**< CAST5 key generation */
    CKM_CAST5_ECB = 0x00000321,          /**< CAST5 ECB */
    CKM_CAST5_CBC = 0x00000322,          /**< CAST5 CBC */
    CKM_CAST5_MAC = 0x00000323,          /**< CAST5 MAC */
    CKM_CAST5_MAC_GENERAL = 0x00000324,  /**< CAST5 MAC general */
    CKM_CAST5_CBC_PAD = 0x00000325,      /**< CAST5 CBC pad */
    CKM_RC5_KEY_GEN = 0x00000330,        /**< RC5 key generation */
    CKM_RC5_ECB = 0x00000331,            /**< RC5 ECB */
    CKM_RC5_CBC = 0x00000332,            /**< RC5 CBC */
    CKM_RC5_MAC = 0x00000333,            /**< RC5 MAC */
    CKM_RC5_MAC_GENERAL = 0x00000334,    /**< RC5 MAC general */
    CKM_RC5_CBC_PAD = 0x00000335,        /**< RC5 CBC pad */
    CKM_IDEA_KEY_GEN = 0x00000340,       /**< IDEA key generation */
    CKM_IDEA_ECB = 0x00000341,           /**< IDEA ECB */
    CKM_IDEA_CBC = 0x00000342,           /**< IDEA CBC */
    CKM_IDEA_MAC = 0x00000343,           /**< IDEA MAC */
    CKM_IDEA_MAC_GENERAL = 0x00000344,   /**< IDEA MAC general */
    CKM_IDEA_CBC_PAD = 0x00000345,       /**< IDEA CBC pad */
    CKM_GENERIC_SECRET_KEY_GEN = 0x00000350, /**< Generic secret key generation */
    CKM_CONCATENATE_BASE_AND_KEY = 0x00000360, /**< Concatenate base and key */
    CKM_CONCATENATE_BASE_AND_DATA = 0x00000362, /**< Concatenate base and data */
    CKM_CONCATENATE_DATA_AND_BASE = 0x00000363, /**< Concatenate data and base */
    CKM_XOR_BASE_AND_DATA = 0x00000364,   /**< XOR base and data */
    CKM_EXTRACT_KEY_FROM_KEY = 0x00000365, /**< Extract key from key */
    CKM_SSL3_PRE_MASTER_KEY_GEN = 0x00000370, /**< SSL3 pre-master key generation */
    CKM_SSL3_MASTER_KEY_DERIVE = 0x00000371, /**< SSL3 master key derive */
    CKM_SSL3_KEY_AND_MAC_DERIVE = 0x00000372, /**< SSL3 key and MAC derive */
    CKM_SSL3_MASTER_KEY_DERIVE_DH = 0x00000373, /**< SSL3 master key derive DH */
    CKM_TLS_PRE_MASTER_KEY_GEN = 0x00000374, /**< TLS pre-master key generation */
    CKM_TLS_MASTER_KEY_DERIVE = 0x00000375, /**< TLS master key derive */
    CKM_TLS_KEY_AND_MAC_DERIVE = 0x00000376, /**< TLS key and MAC derive */
    CKM_TLS_MASTER_KEY_DERIVE_DH = 0x00000377, /**< TLS master key derive DH */
    CKM_TLS_PRF = 0x00000378,             /**< TLS PRF */
    CKM_SSL3_MD5_MAC = 0x00000380,        /**< SSL3 MD5 MAC */
    CKM_SSL3_SHA1_MAC = 0x00000381,       /**< SSL3 SHA1 MAC */
    CKM_MD5_KEY_DERIVATION = 0x00000390,  /**< MD5 key derivation */
    CKM_MD5_KEY_DERIVATION_HMAC = 0x00000391, /**< MD5 key derivation HMAC */
    CKM_SHA1_KEY_DERIVATION = 0x00000392, /**< SHA1 key derivation */
    CKM_SHA1_KEY_DERIVATION_HMAC = 0x00000393, /**< SHA1 key derivation HMAC */
    CKM_SHA256_KEY_DERIVATION = 0x00000394, /**< SHA256 key derivation */
    CKM_SHA256_KEY_DERIVATION_HMAC = 0x00000395, /**< SHA256 key derivation HMAC */
    CKM_SHA384_KEY_DERIVATION = 0x00000396, /**< SHA384 key derivation */
    CKM_SHA384_KEY_DERIVATION_HMAC = 0x00000397, /**< SHA384 key derivation HMAC */
    CKM_SHA512_KEY_DERIVATION = 0x00000398, /**< SHA512 key derivation */
    CKM_SHA512_KEY_DERIVATION_HMAC = 0x00000399, /**< SHA512 key derivation HMAC */
    CKM_PBE_MD2_DES_CBC = 0x000003A0,      /**< PBE MD2 DES CBC */
    CKM_PBE_MD5_DES_CBC = 0x000003A1,      /**< PBE MD5 DES CBC */
    CKM_PBE_MD5_CAST_CBC = 0x000003A2,     /**< PBE MD5 CAST CBC */
    CKM_PBE_MD5_CAST3_CBC = 0x000003A3,    /**< PBE MD5 CAST3 CBC */
    CKM_PBE_MD5_CAST5_CBC = 0x000003A4,    /**< PBE MD5 CAST5 CBC */
    CKM_PBE_SHA1_CAST5_CBC = 0x000003A5,   /**< PBE SHA1 CAST5 CBC */
    CKM_PBE_SHA1_RC4_128 = 0x000003A6,     /**< PBE SHA1 RC4 128 */
    CKM_PBE_SHA1_RC4_40 = 0x000003A7,      /**< PBE SHA1 RC4 40 */
    CKM_PBE_SHA1_DES3_EDE_CBC = 0x000003A8, /**< PBE SHA1 DES3 EDE CBC */
    CKM_PBE_SHA1_DES2_EDE_CBC = 0x000003A9, /**< PBE SHA1 DES2 EDE CBC */
    CKM_PBE_SHA1_RC2_128_CBC = 0x000003AA, /**< PBE SHA1 RC2 128 CBC */
    CKM_PBE_SHA1_RC2_40_CBC = 0x000003AB,  /**< PBE SHA1 RC2 40 CBC */
    CKM_PKCS5_PBKD2 = 0x000003B0,         /**< PKCS5 PBKD2 */
    CKM_PBA_SHA1_WITH_SHA1_HMAC = 0x000003C0, /**< PBA SHA1 with SHA1 HMAC */
    CKM_WTLS_PRE_MASTER_KEY_GEN = 0x000003D0, /**< WTLS pre-master key generation */
    CKM_WTLS_MASTER_KEY_DERIVE = 0x000003D1, /**< WTLS master key derive */
    CKM_WTLS_MASTER_KEY_DERIVE_DH_ECC = 0x000003D2, /**< WTLS master key derive DH ECC */
    CKM_WTLS_PRF = 0x000003D3,             /**< WTLS PRF */
    CKM_WTLS_SERVER_KEY_AND_MAC_DERIVE = 0x000003D4, /**< WTLS server key and MAC derive */
    CKM_WTLS_CLIENT_KEY_AND_MAC_DERIVE = 0x000003D5, /**< WTLS client key and MAC derive */
    CKM_TLS10_MAC_SERVER = 0x000003D6,     /**< TLS10 MAC server */
    CKM_TLS10_MAC_CLIENT = 0x000003D7,     /**< TLS10 MAC client */
    CKM_TLS12_MAC = 0x000003D8,           /**< TLS12 MAC */
    CKM_TLS12_KDF = 0x000003D9,           /**< TLS12 KDF */
    CKM_TLS12_MASTER_KEY_DERIVE = 0x000003DA, /**< TLS12 master key derive */
    CKM_TLS12_KEY_AND_MAC_DERIVE = 0x000003DB, /**< TLS12 key and MAC derive */
    CKM_TLS12_MASTER_KEY_DERIVE_DH = 0x000003DC, /**< TLS12 master key derive DH */
    CKM_TLS12_KEY_SAFE_DERIVE = 0x000003DD, /**< TLS12 key safe derive */
    CKM_TLS_MAC = 0x000003DE,             /**< TLS MAC */
    CKM_TLS_KDF = 0x000003DF,             /**< TLS KDF */
    CKM_KEY_WRAP_LYNKS = 0x00000400,     /**< Key wrap LYNKS */
    CKM_KEY_WRAP_SET_OAEP = 0x00000401,  /**< Key wrap SET OAEP */
    CKM_CMS_SIG = 0x00000500,            /**< CMS signature */
    CKM_KIP_DERIVE = 0x00000510,         /**< KIP derive */
    CKM_KIP_WRAP = 0x00000511,           /**< KIP wrap */
    CKM_KIP_MAC = 0x00000512,            /**< KIP MAC */
    CKM_CAMELLIA_KEY_GEN = 0x00000550,   /**< Camellia key generation */
    CKM_CAMELLIA_ECB = 0x00000551,       /**< Camellia ECB */
    CKM_CAMELLIA_CBC = 0x00000552,       /**< Camellia CBC */
    CKM_CAMELLIA_MAC = 0x00000553,       /**< Camellia MAC */
    CKM_CAMELLIA_MAC_GENERAL = 0x00000554, /**< Camellia MAC general */
    CKM_CAMELLIA_CBC_PAD = 0x00000555,   /**< Camellia CBC pad */
    CKM_CAMELLIA_ECB_ENCRYPT_DATA = 0x00000556, /**< Camellia ECB encrypt data */
    CKM_CAMELLIA_CBC_ENCRYPT_DATA = 0x00000557, /**< Camellia CBC encrypt data */
    CKM_CAMELLIA_CTR = 0x00000558,       /**< Camellia CTR */
    CKM_ARIA_KEY_GEN = 0x00000560,       /**< ARIA key generation */
    CKM_ARIA_ECB = 0x00000561,           /**< ARIA ECB */
    CKM_ARIA_CBC = 0x00000562,           /**< ARIA CBC */
    CKM_ARIA_MAC = 0x00000563,           /**< ARIA MAC */
    CKM_ARIA_MAC_GENERAL = 0x00000564,   /**< ARIA MAC general */
    CKM_ARIA_CBC_PAD = 0x00000565,       /**< ARIA CBC pad */
    CKM_ARIA_ECB_ENCRYPT_DATA = 0x00000566, /**< ARIA ECB encrypt data */
    CKM_ARIA_CBC_ENCRYPT_DATA = 0x00000567, /**< ARIA CBC encrypt data */
    CKM_SEED_KEY_GEN = 0x00000650,       /**< SEED key generation */
    CKM_SEED_ECB = 0x00000651,           /**< SEED ECB */
    CKM_SEED_CBC = 0x00000652,           /**< SEED CBC */
    CKM_SEED_MAC = 0x00000653,           /**< SEED MAC */
    CKM_SEED_MAC_GENERAL = 0x00000654,   /**< SEED MAC general */
    CKM_SEED_CBC_PAD = 0x00000655,       /**< SEED CBC pad */
    CKM_SEED_ECB_ENCRYPT_DATA = 0x00000656, /**< SEED ECB encrypt data */
    CKM_SEED_CBC_ENCRYPT_DATA = 0x00000657, /**< SEED CBC encrypt data */
    CKM_SKIPJACK_KEY_GEN = 0x00001000,   /**< Skipjack key generation */
    CKM_SKIPJACK_ECB64 = 0x00001001,     /**< Skipjack ECB64 */
    CKM_SKIPJACK_CBC64 = 0x00001002,     /**< Skipjack CBC64 */
    CKM_SKIPJACK_OFB64 = 0x00001003,     /**< Skipjack OFB64 */
    CKM_SKIPJACK_CFB64 = 0x00001004,     /**< Skipjack CFB64 */
    CKM_SKIPJACK_CFB32 = 0x00001005,     /**< Skipjack CFB32 */
    CKM_SKIPJACK_CFB16 = 0x00001006,     /**< Skipjack CFB16 */
    CKM_SKIPJACK_CFB8 = 0x00001007,      /**< Skipjack CFB8 */
    CKM_SKIPJACK_WRAP = 0x00001008,      /**< Skipjack wrap */
    CKM_SKIPJACK_PRIVATE_WRAP = 0x00001009, /**< Skipjack private wrap */
    CKM_SKIPJACK_RELAYX = 0x0000100A,    /**< Skipjack relayX */
    CKM_KEA_KEY_PAIR_GEN = 0x00001010,   /**< KEA key pair generation */
    CKM_KEA_KEY_DERIVE = 0x00001011,     /**< KEA key derive */
    CKM_KEA_DERIVE = 0x00001012,         /**< KEA derive */
    CKM_FORTEZZA_TIMESTAMP = 0x00001020, /**< Fortezza timestamp */
    CKM_BATON_KEY_GEN = 0x00001030,      /**< Baton key generation */
    CKM_BATON_ECB128 = 0x00001031,       /**< Baton ECB128 */
    CKM_BATON_ECB96 = 0x00001032,        /**< Baton ECB96 */
    CKM_BATON_CBC128 = 0x00001033,       /**< Baton CBC128 */
    CKM_BATON_COUNTER = 0x00001034,      /**< Baton counter */
    CKM_BATON_SHUFFLE = 0x00001035,      /**< Baton shuffle */
    CKM_BATON_WRAP = 0x00001036,         /**< Baton wrap */
    CKM_ECDSA_KEY_PAIR_GEN = 0x00001040, /**< ECDSA key pair generation */
    CKM_EC_KEY_PAIR_GEN = 0x00001040,    /**< EC key pair generation (alias) */
    CKM_ECDSA = 0x00001041,              /**< ECDSA */
    CKM_ECDSA_SHA1 = 0x00001042,         /**< ECDSA SHA1 */
    CKM_ECDH1_DERIVE = 0x00001050,       /**< ECDH1 derive */
    CKM_ECDH1_COFACTOR_DERIVE = 0x00001051, /**< ECDH1 cofactor derive */
    CKM_ECMQV_DERIVE = 0x00001052,       /**< ECMQV derive */
    CKM_ECDH_AES_KEY_WRAP = 0x00001053,  /**< ECDH AES key wrap */
    CKM_RSA_AES_KEY_WRAP = 0x00001054,   /**< RSA AES key wrap */
    CKM_JUNIPER_KEY_GEN = 0x00001060,    /**< Juniper key generation */
    CKM_JUNIPER_ECB128 = 0x00001061,     /**< Juniper ECB128 */
    CKM_JUNIPER_CBC128 = 0x00001062,     /**< Juniper CBC128 */
    CKM_JUNIPER_COUNTER = 0x00001063,    /**< Juniper counter */
    CKM_JUNIPER_SHUFFLE = 0x00001064,    /**< Juniper shuffle */
    CKM_JUNIPER_WRAP = 0x00001065,       /**< Juniper wrap */
    CKM_FASTHASH = 0x00001070,           /**< FastHash */
    CKM_AES_KEY_GEN = 0x00001080,        /**< AES key generation */
    CKM_AES_ECB = 0x00001081,            /**< AES ECB */
    CKM_AES_CBC = 0x00001082,            /**< AES CBC */
    CKM_AES_MAC = 0x00001083,            /**< AES MAC */
    CKM_AES_MAC_GENERAL = 0x00001084,    /**< AES MAC general */
    CKM_AES_CBC_PAD = 0x00001085,        /**< AES CBC pad */
    CKM_AES_CTR = 0x00001086,            /**< AES CTR */
    CKM_AES_GCM = 0x00001087,            /**< AES GCM */
    CKM_AES_CCM = 0x00001088,            /**< AES CCM */
    CKM_AES_CTS = 0x00001089,            /**< AES CTS */
    CKM_AES_CMAC = 0x0000108A,           /**< AES CMAC */
    CKM_AES_CMAC_GENERAL = 0x0000108B,   /**< AES CMAC general */
    CKM_AES_XCBC_MAC = 0x0000108C,       /**< AES XCBC MAC */
    CKM_AES_XCBC_MAC_96 = 0x0000108D,    /**< AES XCBC MAC 96 */
    CKM_AES_GMAC = 0x0000108E,           /**< AES GMAC */
    CKM_BLOWFISH_KEY_GEN = 0x00001090,   /**< Blowfish key generation */
    CKM_BLOWFISH_CBC = 0x00001091,       /**< Blowfish CBC */
    CKM_TWOFISH_KEY_GEN = 0x00001092,    /**< Twofish key generation */
    CKM_TWOFISH_CBC = 0x00001093,        /**< Twofish CBC */
    CKM_AES_ECB_ENCRYPT_DATA = 0x00001094, /**< AES ECB encrypt data */
    CKM_AES_CBC_ENCRYPT_DATA = 0x00001095, /**< AES CBC encrypt data */
    CKM_GOSTR3410_KEY_PAIR_GEN = 0x00001200, /**< GOST R 34.10 key pair generation */
    CKM_GOSTR3410 = 0x00001201,          /**< GOST R 34.10 */
    CKM_GOSTR3410_WITH_GOSTR3411 = 0x00001202, /**< GOST R 34.10 with GOST R 34.11 */
    CKM_GOSTR3410_KEY_WRAP = 0x00001203, /**< GOST R 34.10 key wrap */
    CKM_GOSTR3410_DERIVE = 0x00001204,   /**< GOST R 34.10 derive */
    CKM_GOSTR3411 = 0x00001210,          /**< GOST R 34.11 */
    CKM_GOSTR3411_HMAC = 0x00001211,     /**< GOST R 34.11 HMAC */
    CKM_GOST28147_KEY_GEN = 0x00001220,  /**< GOST 28147 key generation */
    CKM_GOST28147_ECB = 0x00001221,      /**< GOST 28147 ECB */
    CKM_GOST28147 = 0x00001222,          /**< GOST 28147 */
    CKM_GOST28147_MAC = 0x00001223,      /**< GOST 28147 MAC */
    CKM_GOST28147_KEY_WRAP = 0x00001224, /**< GOST 28147 key wrap */
    CKM_DSA_PARAMETER_GEN = 0x00002000,  /**< DSA parameter generation */
    CKM_DH_PKCS_PARAMETER_GEN = 0x00002001, /**< DH PKCS parameter generation */
    CKM_X9_42_DH_PARAMETER_GEN = 0x00002002, /**< X9.42 DH parameter generation */
    CKM_DSA_PROBABILISTIC_PARAMETER_GEN = 0x00002003, /**< DSA probabilistic parameter generation */
    CKM_DSA_SHAWE_TAYLOR_PARAMETER_GEN = 0x00002004, /**< DSA Shawe-Taylor parameter generation */
    CKM_AES_OFB = 0x00002104,            /**< AES OFB */
    CKM_AES_CFB64 = 0x00002105,          /**< AES CFB64 */
    CKM_AES_CFB8 = 0x00002106,           /**< AES CFB8 */
    CKM_AES_CFB128 = 0x00002107,         /**< AES CFB128 */
    CKM_AES_CFB1 = 0x00002108,           /**< AES CFB1 */
    CKM_AES_KEY_WRAP = 0x00002109,       /**< AES key wrap */
    CKM_AES_KEY_WRAP_PAD = 0x0000210A,   /**< AES key wrap pad */
    CKM_RSA_PKCS_TPM_1_1 = 0x00004001,   /**< RSA PKCS TPM 1.1 */
    CKM_RSA_PKCS_OAEP_TPM_1_1 = 0x00004002, /**< RSA PKCS OAEP TPM 1.1 */
    CKM_VENDOR_DEFINED = 0x80000000      /**< Vendor defined */
};

/**
 * @brief Basic PKCS#11 attribute structure
 */
struct CK_ATTRIBUTE {
    CK_ATTRIBUTE_TYPE type;   /**< Attribute type */
    void* pValue;            /**< Pointer to attribute value */
    uint32_t ulValueLen;     /**< Length of attribute value */
};

/**
 * @brief PKCS#11 mechanism structure
 */
struct CK_MECHANISM {
    CK_MECHANISM_TYPE mechanism;  /**< Mechanism type */
    void* pParameter;            /**< Mechanism parameter */
    uint32_t ulParameterLen;     /**< Length of mechanism parameter */
};

/**
 * @brief PKCS#11 session handle
 */
typedef uint32_t CK_SESSION_HANDLE;

/**
 * @brief PKCS#11 object handle
 */
typedef uint32_t CK_OBJECT_HANDLE;

/**
 * @brief PKCS#11 slot ID
 */
typedef uint32_t CK_SLOT_ID;

/**
 * @brief PKCS#11 function list structure (simplified)
 */
struct CK_FUNCTION_LIST {
    uint32_t version;  /**< Version of function list */
    // Function pointers would go here in a full implementation
};

} // namespace pkcs11
} // namespace hsm
} // namespace clwe

#endif // PKCS11_LAYER_HPP
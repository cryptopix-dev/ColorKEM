#pragma once

#include <vector>
#include <string>
#include <clwe/color_kem.hpp>

namespace blockchain {

class KeyManagementInterface {
public:
    virtual ~KeyManagementInterface() = default;

    virtual std::vector<uint8_t> generateKeyPair() = 0;

    virtual std::vector<uint8_t> getPublicKey() const = 0;

    virtual std::vector<uint8_t> encapsulate(const std::vector<uint8_t>& publicKey) = 0;

    virtual std::vector<uint8_t> decapsulate(const std::vector<uint8_t>& ciphertext) = 0;

    virtual bool validateKey(const std::vector<uint8_t>& key) = 0;

    virtual std::string formatWalletAddress(const std::vector<uint8_t>& publicKey) = 0;

    virtual bool secureKeyStorage(const std::vector<uint8_t>& privateKey) = 0;
};

}
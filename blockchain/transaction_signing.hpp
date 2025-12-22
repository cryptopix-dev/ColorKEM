#pragma once

#include <vector>
#include <string>
#include <clwe/color_kem.hpp>

namespace blockchain {

class TransactionSigningInterface {
public:
    virtual ~TransactionSigningInterface() = default;

    virtual std::vector<uint8_t> deriveSigningKey(const std::vector<uint8_t>& kemSharedSecret) = 0;

    virtual std::vector<uint8_t> signTransaction(const std::vector<uint8_t>& transactionData, const std::vector<uint8_t>& signingKey) = 0;

    virtual bool verifyTransactionSignature(const std::vector<uint8_t>& transactionData, const std::vector<uint8_t>& signature, const std::vector<uint8_t>& publicKey) = 0;

    virtual std::vector<uint8_t> formatTransactionMessage(const std::string& rawTransaction) = 0;

    virtual bool checkReplayProtection(const std::vector<uint8_t>& transactionHash) = 0;
};

}
#pragma once

#include <vector>
#include <string>
#include <clwe/color_kem.hpp>

namespace blockchain {

class CrossChainCommunicationInterface {
public:
    virtual ~CrossChainCommunicationInterface() = default;

    virtual std::vector<uint8_t> establishCrossChainChannel(const std::string& targetChain) = 0;

    virtual std::vector<uint8_t> sendCrossChainMessage(const std::vector<uint8_t>& message) = 0;

    virtual bool verifyCrossChainMessage(const std::vector<uint8_t>& message, const std::vector<uint8_t>& proof) = 0;

    virtual std::vector<uint8_t> formatCrossChainMessage(const std::string& rawMessage) = 0;

    virtual bool ensureCrossChainSecurity(const std::vector<uint8_t>& channelData) = 0;
};

}
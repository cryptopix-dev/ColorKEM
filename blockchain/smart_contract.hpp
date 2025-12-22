#pragma once

#include <vector>
#include <string>
#include <clwe/color_kem.hpp>

namespace blockchain {

class SmartContractInterface {
public:
    virtual ~SmartContractInterface() = default;

    virtual std::vector<uint8_t> secureKeyExchange(const std::vector<uint8_t>& contractAddress) = 0;

    virtual std::vector<uint8_t> executeContractOperation(const std::vector<uint8_t>& operationData) = 0;

    virtual bool verifyContractState(const std::vector<uint8_t>& stateHash) = 0;

    virtual std::vector<uint8_t> formatContractMessage(const std::string& contractCall) = 0;

    virtual bool enforceContractSecurity(const std::vector<uint8_t>& contractCode) = 0;
};

}
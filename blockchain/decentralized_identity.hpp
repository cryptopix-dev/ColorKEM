#pragma once

#include <vector>
#include <string>
#include <clwe/color_kem.hpp>

namespace blockchain {

class DecentralizedIdentityInterface {
public:
    virtual ~DecentralizedIdentityInterface() = default;

    virtual std::string createDecentralizedIdentity(const std::vector<uint8_t>& publicKey) = 0;

    virtual bool authenticateIdentity(const std::string& did, const std::vector<uint8_t>& challenge) = 0;

    virtual std::vector<uint8_t> resolveIdentity(const std::string& did) = 0;

    virtual std::vector<uint8_t> formatIdentityMessage(const std::string& identityData) = 0;

    virtual bool verifyIdentityPrivacy(const std::vector<uint8_t>& identityAttributes) = 0;
};

}
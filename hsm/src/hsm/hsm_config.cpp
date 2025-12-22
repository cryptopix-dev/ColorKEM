/**
 * @file hsm_config.cpp
 * @brief Implementation of HSM configuration management
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 */

#include "hsm/hsm_config.hpp"

namespace clwe {
namespace hsm {

HSMConfig global_hsm_config;

void set_global_hsm_config(const HSMConfig& config) {
    global_hsm_config = config;
}

const HSMConfig& get_global_hsm_config() {
    return global_hsm_config;
}

} // namespace hsm
} // namespace clwe
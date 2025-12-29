/**
 * @file software_hsm.hpp
 * @brief Header for software HSM implementation
 *
 * @author ColorKEM Development Team
 * @version 1.0.0
 * @date 2024
 */

#ifndef SOFTWARE_HSM_HPP
#define SOFTWARE_HSM_HPP

#include "hsm_interface.hpp"
#include <memory>

namespace clwe {
namespace hsm {

/**
 * @brief Factory function to create a software HSM instance
 */
std::unique_ptr<HSMInterface> create_software_hsm();

} // namespace hsm
} // namespace clwe

#endif // SOFTWARE_HSM_HPP
#include "clwe/ntt_engine.hpp"
#include "clwe/ntt_scalar.hpp"
#include "clwe/utils.hpp"
#include <algorithm>
#include <stdexcept>

namespace clwe {

NTTEngine::NTTEngine(uint32_t q, uint32_t n)
    : q_(q), n_(n), log_n_(0), bitrev_(n) {

    if (!is_power_of_two(n)) {
        throw std::invalid_argument("NTT degree must be a power of 2");
    }

    // Calculate log_n_
    uint32_t temp = n;
    while (temp > 1) {
        temp >>= 1;
        log_n_++;
    }

    precompute_bitrev();
}

void NTTEngine::precompute_bitrev() {
    for (uint32_t i = 0; i < n_; ++i) {
        uint32_t rev = 0;
        for (uint32_t j = 0; j < log_n_; ++j) {
            rev |= ((i >> j) & 1) << (log_n_ - 1 - j);
        }
        bitrev_[i] = rev;
    }
}

void NTTEngine::bit_reverse(uint32_t* poly) const {
    std::vector<uint32_t> temp(n_);
    for (uint32_t i = 0; i < n_; ++i) {
        temp[bitrev_[i]] = poly[i];
    }
    std::copy(temp.begin(), temp.end(), poly);
}

void NTTEngine::copy_from_uint32(const uint32_t* coeffs, uint32_t* ntt_coeffs) const {
    std::copy(coeffs, coeffs + n_, ntt_coeffs);
}

void NTTEngine::copy_to_uint32(const uint32_t* ntt_coeffs, uint32_t* coeffs) const {
    std::copy(ntt_coeffs, ntt_coeffs + n_, coeffs);
}

// Forward declarations for concrete implementations
class ScalarNTTEngine;

std::unique_ptr<NTTEngine> create_optimal_ntt_engine(uint32_t q, uint32_t n) {
    return std::unique_ptr<NTTEngine>(new ScalarNTTEngine(q, n));
}

std::unique_ptr<NTTEngine> create_ntt_engine(SIMDSupport /*simd_support*/, uint32_t q, uint32_t n) {
    return std::unique_ptr<NTTEngine>(new ScalarNTTEngine(q, n));
}

} // namespace clwe
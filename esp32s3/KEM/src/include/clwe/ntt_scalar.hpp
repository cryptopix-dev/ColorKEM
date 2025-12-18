#ifndef NTT_SCALAR_HPP
#define NTT_SCALAR_HPP

#include "ntt_engine.hpp"
#include <cstdint>
#include <vector>

#ifdef ESP_PLATFORM
#include <esp_attr.h>
#endif

namespace clwe {

class ScalarNTTEngine : public NTTEngine {
private:
    std::vector<uint32_t> zetas_;       // Precomputed zetas
    std::vector<uint32_t> zetas_inv_;   // Inverse zetas

    // Montgomery reduction constants
    uint32_t montgomery_r_;       // 2^32 mod q
    uint32_t montgomery_r_inv_;   // R^(-1) mod q

    // Precompute zetas for NTT
    void precompute_zetas();

    // Scalar butterfly operations
    void IRAM_ATTR butterfly(uint32_t& a, uint32_t& b, uint32_t zeta) const;
    void IRAM_ATTR butterfly_inv(uint32_t& a, uint32_t& b, uint32_t zeta) const;

    // Modular reduction
    uint32_t IRAM_ATTR mod_reduce(uint32_t val) const;

    // Montgomery reduction
    uint32_t IRAM_ATTR montgomery_reduce(uint64_t val) const;

public:
    ScalarNTTEngine(uint32_t q, uint32_t n);
    ~ScalarNTTEngine() override = default;

    // Implement pure virtual methods
    void IRAM_ATTR ntt_forward(uint32_t* poly) const override;
    void IRAM_ATTR ntt_inverse(uint32_t* poly) const override;
    void IRAM_ATTR multiply(const uint32_t* a, const uint32_t* b, uint32_t* result) const override;

    SIMDSupport get_simd_support() const override { return SIMDSupport::NONE; }
};

} // namespace clwe

#endif // NTT_SCALAR_HPP
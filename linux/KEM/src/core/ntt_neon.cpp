#include "ntt_neon.hpp"
#include "utils.hpp"
#include <algorithm>
#include <iostream>
#include <cstring>

namespace clwe {

NEONNTTEngine::NEONNTTEngine(uint32_t q, uint32_t n)
    : NTTEngine(q, n), zetas_(nullptr), zetas_inv_(nullptr),
      montgomery_r_(0), montgomery_r_inv_(0) {

#ifdef HAVE_NEON
    // NEON: 4 uint32_t per uint32x4_t
    size_t neon_count = n / 4;
    zetas_ = new uint32x4_t[neon_count];
    zetas_inv_ = new uint32x4_t[neon_count];
#endif

    // Pre-compute Montgomery constants
    montgomery_r_ = (1ULL << 32) % q_;
    // QINV for q=3329 is 62209
    montgomery_r_inv_ = 62209;

    precompute_zetas();
}

NEONNTTEngine::~NEONNTTEngine() {
#ifdef HAVE_NEON
    if (zetas_) delete[] zetas_;
    if (zetas_inv_) delete[] zetas_inv_;
#endif
}

void NEONNTTEngine::precompute_zetas() {
    // Precompute primitive n-th root of unity
    uint32_t g = 17;  // Primitive root for q = 3329 (Kyber modulus)
    uint32_t zeta = mod_pow(g, (q_ - 1) / n_, q_);
    uint32_t zeta_mont = montgomery_reduce(zeta);

    std::vector<uint32_t> zetas_scalar(n_);
    std::vector<uint32_t> zetas_inv_scalar(n_);

    zetas_scalar[0] = 1;
    for (uint32_t i = 1; i < n_; ++i) {
        zetas_scalar[i] = montgomery_reduce(static_cast<uint64_t>(zetas_scalar[i-1]) * zeta_mont);
    }

    // Inverse zetas for inverse NTT
    uint32_t zeta_inv = mod_inverse(zeta, q_);
    uint32_t zeta_inv_mont = montgomery_reduce(zeta_inv);
    zetas_inv_scalar[0] = 1;
    for (uint32_t i = 1; i < n_; ++i) {
        zetas_inv_scalar[i] = montgomery_reduce(static_cast<uint64_t>(zetas_inv_scalar[i-1]) * zeta_inv_mont);
    }

#ifdef HAVE_NEON
    // Pack into NEON vectors (4 values per vector)
    for (uint32_t i = 0; i < n_; i += 4) {
        uint32_t vals[4];
        uint32_t vals_inv[4];
        for (int j = 0; j < 4 && i + j < n_; ++j) {
            vals[j] = zetas_scalar[i + j];
            vals_inv[j] = zetas_inv_scalar[i + j];
        }
        zetas_[i/4] = vld1q_u32(vals);
        zetas_inv_[i/4] = vld1q_u32(vals_inv);
    }
#endif
}

void NEONNTTEngine::butterfly_neon(uint32x4_t& a, uint32x4_t& b, uint32x4_t zeta) const {
#ifdef HAVE_NEON
    // NEON butterfly: a, b = a + b, (a - b) * zeta
    uint32x4_t sum = vaddq_u32(a, b);
    uint32x4_t diff = vsubq_u32(a, b);
    // Constant-time modular subtraction: if underflow, add q
    uint32x4_t underflow = vcltq_u32(a, b);
    uint32x4_t q_vec = vdupq_n_u32(q_);
    uint32x4_t add_q = vbslq_u32(underflow, q_vec, vdupq_n_u32(0));
    diff = vaddq_u32(diff, add_q);
    uint32x4_t prod = montgomery_multiply_neon(diff, zeta);
    a = sum;
    b = prod;
#endif
}

void NEONNTTEngine::butterfly_inv_neon(uint32x4_t& a, uint32x4_t& b, uint32x4_t zeta) const {
    // Same as forward for inverse (zeta is already inverse)
    butterfly_neon(a, b, zeta);
}

uint32x4_t NEONNTTEngine::mod_reduce_neon(uint32x4_t val) const {
#ifdef HAVE_NEON
    uint32x4_t q_vec = vdupq_n_u32(q_);
    uint32x4_t mask = vcgeq_u32(val, q_vec);
    uint32x4_t sub = vbslq_u32(mask, q_vec, vdupq_n_u32(0));
    return vsubq_u32(val, sub);
#else
    return val;  // Should not reach here
#endif
}

uint32_t NEONNTTEngine::montgomery_reduce(uint64_t val) const {
    // Montgomery reduction: optimized Kyber version
    uint64_t t = val * montgomery_r_inv_;
    t &= (1ULL << 32) - 1;
    t *= q_;
    return (val + t) >> 32;
}


uint32x4_t NEONNTTEngine::montgomery_reduce_neon(uint32x4_t val) const {
#ifdef HAVE_NEON
    uint32x4_t qinv = vdupq_n_u32(montgomery_r_inv_);
    uint32x4_t q = vdupq_n_u32(q_);
    uint32x4_t t = vmulq_u32(val, qinv);
    uint32x4_t m = vmulq_u32(t, q);
    uint64x2_t sum_low = vaddl_u32(vget_low_u32(val), vget_low_u32(m));
    uint64x2_t sum_high = vaddl_u32(vget_high_u32(val), vget_high_u32(m));
    uint32x2_t res_low = vshrn_n_u64(sum_low, 32);
    uint32x2_t res_high = vshrn_n_u64(sum_high, 32);
    return vcombine_u32(res_low, res_high);
#else
    return val;  // Should not reach here
#endif
}

uint32x4_t NEONNTTEngine::montgomery_multiply_neon(uint32x4_t a, uint32x4_t b) const {
#ifdef HAVE_NEON
    uint32x4_t prod = vmulq_u32(a, b);
    return montgomery_reduce_neon(prod);
#else
    return vmulq_u32(a, b);  // Should not reach here
#endif
}

void NEONNTTEngine::ntt_forward(uint32_t* poly) const {
#ifdef HAVE_NEON
    // NEON-optimized NTT implementation
    uint32_t m = 1;
    uint32_t k = n_ / 2;

    for (uint32_t stage = 0; stage < log_degree(); ++stage) {
        uint32_t j = 0;
        for (uint32_t i = 0; i < k; i += 4) {  // Process 4 butterflies at once
            if (i + 4 <= k) {
                // Load zeta values
                uint32x4_t zeta_vec = zetas_[j];

                // Load and process 4 pairs
                uint32x4_t a0 = vld1q_u32(&poly[i]);
                uint32x4_t b0 = vld1q_u32(&poly[i + k]);

                butterfly_neon(a0, b0, zeta_vec);

                vst1q_u32(&poly[i], a0);
                vst1q_u32(&poly[i + k], b0);
            } else {
                // Handle remaining elements with scalar operations
                for (uint32_t idx = i; idx < k; ++idx) {
                    uint32_t zeta = reinterpret_cast<const uint32_t*>(&zetas_[j/4])[j%4];
                    uint32_t sum = (poly[idx] + poly[idx + k]) % q_;
                    uint32_t diff = poly[idx] - poly[idx + k];
                    diff += ((diff >> 31) & q_);
                    uint32_t prod = montgomery_reduce(static_cast<uint64_t>(diff) * zeta);
                    poly[idx] = sum;
                    poly[idx + k] = prod;
                }
            }
            j += m;
        }
        m *= 2;
        k /= 2;

        // Modular reduction after each stage
        for (uint32_t i = 0; i < n_; i += 4) {
            uint32x4_t vec = vld1q_u32(&poly[i]);
            vec = mod_reduce_neon(vec);
            vst1q_u32(&poly[i], vec);
        }
    }
#else
    // Fallback to scalar implementation
    ntt_forward(poly);
#endif
}

void NEONNTTEngine::ntt_inverse(uint32_t* poly) const {
#ifdef HAVE_NEON
    // NEON inverse NTT
    uint32_t m = n_ / 2;
    uint32_t k = 1;

    for (uint32_t stage = 0; stage < log_degree(); ++stage) {
        uint32_t j = 0;
        for (uint32_t i = 0; i < k; i += 4) {
            if (i + 4 <= k) {
                uint32x4_t zeta_vec = zetas_inv_[j];
                uint32x4_t a0 = vld1q_u32(&poly[i]);
                uint32x4_t b0 = vld1q_u32(&poly[i + k]);

                butterfly_inv_neon(a0, b0, zeta_vec);

                vst1q_u32(&poly[i], a0);
                vst1q_u32(&poly[i + k], b0);
            } else {
                // Scalar fallback
                for (uint32_t idx = i; idx < k; ++idx) {
                    uint32_t zeta = reinterpret_cast<const uint32_t*>(&zetas_inv_[j/4])[j%4];
                    uint32_t sum = (poly[idx] + poly[idx + k]) % q_;
                    uint32_t diff = poly[idx] - poly[idx + k];
                    diff += ((diff >> 31) & q_);
                    uint32_t prod = montgomery_reduce(static_cast<uint64_t>(diff) * zeta);
                    poly[idx] = sum;
                    poly[idx + k] = prod;
                }
            }
            j += m;
        }
        m /= 2;
        k *= 2;

        // Modular reduction after each stage
        for (uint32_t i = 0; i < n_; i += 4) {
            uint32x4_t vec = vld1q_u32(&poly[i]);
            vec = mod_reduce_neon(vec);
            vst1q_u32(&poly[i], vec);
        }
    }

    // Scale by n^(-1) mod q
    uint32_t n_inv = mod_inverse(n_, q_);
    for (uint32_t i = 0; i < n_; ++i) {
        poly[i] = montgomery_reduce(static_cast<uint64_t>(poly[i]) * n_inv);
    }
#else
    // Fallback
    ntt_inverse(poly);
#endif
}

void NEONNTTEngine::multiply(const uint32_t* a, const uint32_t* b, uint32_t* result) const {
    // Copy inputs for NTT
    std::vector<uint32_t> a_ntt(n_);
    std::vector<uint32_t> b_ntt(n_);
    std::copy(a, a + n_, a_ntt.begin());
    std::copy(b, b + n_, b_ntt.begin());

    // Forward NTT
    ntt_forward(a_ntt.data());
    ntt_forward(b_ntt.data());

    // Pointwise multiplication with Montgomery reduction
    for (uint32_t i = 0; i < n_; i += 4) {
        if (i + 4 <= n_) {
            uint32x4_t a_vec = vld1q_u32(&a_ntt[i]);
            uint32x4_t b_vec = vld1q_u32(&b_ntt[i]);
            uint32x4_t prod = montgomery_multiply_neon(a_vec, b_vec);
            vst1q_u32(&result[i], prod);
        } else {
            for (uint32_t j = i; j < n_; ++j) {
                result[j] = montgomery_reduce(static_cast<uint64_t>(a_ntt[j]) * b_ntt[j]);
            }
        }
    }

    // Inverse NTT
    ntt_inverse(result);
}

} // namespace clwe
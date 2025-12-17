#include <iostream>
#include <chrono>
#include <vector>
#include <functional>
#include <algorithm>
#include "clwe/clwe.hpp"
#include "src/core/color_kem.hpp"
#include "src/core/cpu_features.hpp"
#include "src/core/performance_metrics.hpp"

using namespace clwe;

void benchmark_security_level(int security_level) {
    std::cout << "Security Level: " << security_level << "-bit" << std::endl;
    std::cout << "=====================================" << std::endl;

    clwe::CLWEParameters params(security_level);
    clwe::ColorKEM kem(params);

    // Generate keys and ciphertext for size measurements
    auto [public_key, private_key] = kem.keygen();
    auto [ciphertext, shared_secret] = kem.encapsulate(public_key);

    // Size calculations
    size_t public_key_size = public_key.serialize().size();
    size_t private_key_size = private_key.serialize().size();
    size_t ciphertext_size = ciphertext.serialize().size();
    size_t shared_secret_size = sizeof(ColorValue);

    // Memory and timing measurements using new API
    clwe::MemoryStats keygen_mem, encap_mem, decap_mem;
    clwe::TimingStats keygen_timing = clwe::PerformanceMetrics::time_operation_with_memory([&]() {
        auto [pk, sk] = kem.keygen();
    }, keygen_mem);

    clwe::TimingStats encap_timing = clwe::PerformanceMetrics::time_operation_with_memory([&]() {
        auto [ct, ss] = kem.encapsulate(public_key);
    }, encap_mem);

    clwe::TimingStats decap_timing = clwe::PerformanceMetrics::time_operation_with_memory([&]() {
        ColorValue recovered = kem.decapsulate(public_key, private_key, ciphertext);
    }, decap_mem);

    // CPU cycle measurements
    clwe::CycleStats keygen_cycles = clwe::PerformanceMetrics::time_operation_cycles([&]() {
        auto [pk, sk] = kem.keygen();
    });

    clwe::CycleStats encap_cycles = clwe::PerformanceMetrics::time_operation_cycles([&]() {
        auto [ct, ss] = kem.encapsulate(public_key);
    });

    clwe::CycleStats decap_cycles = clwe::PerformanceMetrics::time_operation_cycles([&]() {
        ColorValue recovered = kem.decapsulate(public_key, private_key, ciphertext);
    });

    // Calculate bandwidth (bytes transferred per second)
    double keygen_bandwidth = (public_key_size + private_key_size) / (keygen_timing.average_time / 1000000.0);
    double encap_bandwidth = (ciphertext_size + shared_secret_size) / (encap_timing.average_time / 1000000.0);
    double decap_bandwidth = (ciphertext_size + shared_secret_size) / (decap_timing.average_time / 1000000.0);

    // Overall calculations
    double total_kem_time = keygen_timing.average_time + encap_timing.average_time + decap_timing.average_time;
    double throughput = 1000000.0 / total_kem_time;
    uint64_t total_cycles = keygen_cycles.average_cycles + encap_cycles.average_cycles + decap_cycles.average_cycles;
    double cycles_per_second = total_cycles / (total_kem_time / 1000000.0);

    // Memory statistics
    size_t total_peak_memory = std::max({keygen_mem.peak_memory, encap_mem.peak_memory, decap_mem.peak_memory});
    size_t avg_memory = (keygen_mem.average_memory + encap_mem.average_memory + decap_mem.average_memory) / 3;

    // Display results
    std::cout << "=== TIMING METRICS ===" << std::endl;
    std::cout << "Key Generation:     " << keygen_timing.average_time << " μs" << std::endl;
    std::cout << "Encapsulation:      " << encap_timing.average_time << " μs" << std::endl;
    std::cout << "Decapsulation:      " << decap_timing.average_time << " μs" << std::endl;
    std::cout << "Total KEM Time:     " << total_kem_time << " μs" << std::endl;
    std::cout << "Throughput:         " << throughput << " operations/second" << std::endl;
    std::cout << std::endl;

    std::cout << "=== CPU CYCLE METRICS ===" << std::endl;
    std::cout << "KeyGen Cycles:      " << keygen_cycles.average_cycles << std::endl;
    std::cout << "Encap Cycles:       " << encap_cycles.average_cycles << std::endl;
    std::cout << "Decap Cycles:       " << decap_cycles.average_cycles << std::endl;
    std::cout << "Total Cycles:       " << total_cycles << std::endl;
    std::cout << "Cycles/Second:      " << cycles_per_second << std::endl;
    std::cout << std::endl;

    std::cout << "=== MEMORY USAGE METRICS ===" << std::endl;
    std::cout << "Peak Memory:        " << total_peak_memory / 1024.0 << " KB" << std::endl;
    std::cout << "Average Memory:     " << avg_memory / 1024.0 << " KB" << std::endl;
    std::cout << std::endl;

    std::cout << "=== STORAGE REQUIREMENTS ===" << std::endl;
    std::cout << "Public Key Size:    " << public_key_size << " bytes" << std::endl;
    std::cout << "Private Key Size:   " << private_key_size << " bytes" << std::endl;
    std::cout << "Ciphertext Size:    " << ciphertext_size << " bytes" << std::endl;
    std::cout << "Shared Secret Size: " << shared_secret_size << " bytes" << std::endl;
    std::cout << std::endl;

    std::cout << "=== BANDWIDTH METRICS ===" << std::endl;
    std::cout << "KeyGen Bandwidth:   " << keygen_bandwidth / 1024.0 << " KB/s" << std::endl;
    std::cout << "Encap Bandwidth:    " << encap_bandwidth / 1024.0 << " KB/s" << std::endl;
    std::cout << "Decap Bandwidth:    " << decap_bandwidth / 1024.0 << " KB/s" << std::endl;
    std::cout << std::endl;

    std::cout << "=== PERFORMANCE BREAKDOWN ===" << std::endl;
    std::cout << "Time Distribution:" << std::endl;
    std::cout << "  KeyGen: " << (keygen_timing.average_time / total_kem_time * 100) << "%" << std::endl;
    std::cout << "  Encap:  " << (encap_timing.average_time / total_kem_time * 100) << "%" << std::endl;
    std::cout << "  Decap:  " << (decap_timing.average_time / total_kem_time * 100) << "%" << std::endl;
    std::cout << std::endl;

    std::cout << "Cycle Distribution:" << std::endl;
    std::cout << "  KeyGen: " << (keygen_cycles.average_cycles / (double)total_cycles * 100) << "%" << std::endl;
    std::cout << "  Encap:  " << (encap_cycles.average_cycles / (double)total_cycles * 100) << "%" << std::endl;
    std::cout << "  Decap:  " << (decap_cycles.average_cycles / (double)total_cycles * 100) << "%" << std::endl;
    std::cout << std::endl;
}

int main() {
    std::cout << "🎨 CLWE Color KEM Timing Benchmark" << std::endl;
    std::cout << "===================================" << std::endl;

    
    CPUFeatures features = CPUFeatureDetector::detect();
    std::cout << "CPU: " << features.to_string() << std::endl;
    std::cout << std::endl;

    
    std::vector<int> security_levels = {128, 192, 256};

    for (int level : security_levels) {
        benchmark_security_level(level);
    }

    std::cout << "Benchmark completed successfully!" << std::endl;

    return 0;
}
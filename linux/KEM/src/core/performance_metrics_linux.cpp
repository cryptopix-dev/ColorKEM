#include "performance_metrics.hpp"
#include <chrono>
#include <fstream>
#include <unistd.h>
#include <algorithm>
#include <numeric>

#ifdef __x86_64__
#include <x86intrin.h>
#endif

namespace clwe {

// Linux-specific memory measurement using /proc/self/statm
MemoryStats PerformanceMetrics::get_memory_usage_impl() {
    std::ifstream statm("/proc/self/statm");
    if (!statm.is_open()) {
        return {0, 0, 0};
    }

    size_t pages = 0;
    statm >> pages;  // Total program size

    size_t page_size = getpagesize();
    size_t memory_kb = pages * page_size / 1024;

    // For current memory, we use the resident set size
    // This is a simplified implementation - in practice you'd track peak separately
    return {memory_kb * 1024, memory_kb * 1024, memory_kb * 1024};
}

// Linux-specific CPU cycle counting
uint64_t PerformanceMetrics::get_cpu_cycles_impl() {
#ifdef __x86_64__
    return __rdtsc();
#else
    // Fallback for non-x86 architectures
    return 0;
#endif
}

} // namespace clwe
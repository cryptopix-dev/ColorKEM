#define NOMINMAX
#include "performance_metrics.hpp"
#include <windows.h>
#include <psapi.h>
#include <intrin.h>

namespace clwe {

// Windows-specific memory measurement using PSAPI
MemoryStats PerformanceMetrics::get_memory_usage_impl() {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return {pmc.WorkingSetSize, pmc.PeakWorkingSetSize, pmc.WorkingSetSize};
    }

    return {0, 0, 0};
}

// Windows-specific CPU cycle counting
uint64_t PerformanceMetrics::get_cpu_cycles_impl() {
    return __rdtsc();
}

} // namespace clwe
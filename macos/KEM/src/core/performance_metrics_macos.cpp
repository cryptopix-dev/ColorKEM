#include "performance_metrics.hpp"
#include <mach/mach.h>
#include <mach/task_info.h>
#include <chrono>

namespace clwe {

// macOS-specific memory measurement using Mach APIs
MemoryStats PerformanceMetrics::get_memory_usage_impl() {
    task_basic_info_data_t info;
    mach_msg_type_number_t info_count = TASK_BASIC_INFO_COUNT;
    kern_return_t kr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &info_count);

    if (kr == KERN_SUCCESS) {
        return {info.resident_size, info.resident_size, info.resident_size};
    }

    return {0, 0, 0};
}

// macOS-specific CPU cycle counting
uint64_t PerformanceMetrics::get_cpu_cycles_impl() {
#ifdef __arm64__
    // For Apple Silicon
    return __builtin_arm_rsr64("cntvct_el0");
#elif defined(__x86_64__)
    // For Intel Macs
    return __builtin_readcyclecounter();
#else
    return 0;
#endif
}

} // namespace clwe
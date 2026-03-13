/*
 * High-Performance Compute Benchmark Suite
 * Optimized for x86-64 architecture with SSE/AVX SIMD instructions
 * Results cached to PostgreSQL for cross-architecture comparison
 */

#include <iostream>
#include <chrono>
#include "matrix_operations.h"
#include "hash_operations.h"
#include "string_search.h"
#include "memory_operations.h"
#include "polynomial_eval.h"
#include "secrets_config.h"
#include "cache_operations.h"

#ifdef __x86_64__
#define USE_X86_SIMD 1
#else
#define USE_X86_SIMD 0
#endif

// Helper: run a benchmark, cache the result, and print comparison
static void run_and_cache(BenchmarkCache& cache,
                          const std::string& name,
                          void (*benchmark_fn)()) {
    double prev = cache.get_cached_duration(name);

    auto start = std::chrono::high_resolution_clock::now();
    benchmark_fn();
    auto end = std::chrono::high_resolution_clock::now();
    double ms = std::chrono::duration<double, std::milli>(end - start).count();

    if (cache.is_connected()) {
        cache.store_result(name, ms, "completed");
        if (prev > 0) {
            double delta = ((prev - ms) / prev) * 100.0;
            std::cout << "  [Cache] Previous: " << prev << " ms | "
                      << (delta > 0 ? "Faster" : "Slower") << " by "
                      << (delta > 0 ? delta : -delta) << "%" << std::endl;
        } else {
            std::cout << "  [Cache] First run on this arch — no comparison" << std::endl;
        }
    }
}

int main() {
    // Load configuration from environment (GitHub Actions secrets)
    AppConfig config = load_config_from_env();
    print_config_summary(config);

    std::cout << "\n========================================" << std::endl;
    std::cout << "  Compute Benchmark Suite" << std::endl;
#if USE_X86_SIMD
    std::cout << "  x86-64 with SSE2 Optimizations" << std::endl;
#else
    std::cout << "  Generic Build (No SIMD)" << std::endl;
    std::cout << "  NOTE: This code is optimized for x86-64" << std::endl;
#endif
    std::cout << "========================================" << std::endl;

    // Initialize benchmark cache (Postgres)
    BenchmarkCache cache(config);
    if (config.cache_enabled) {
        if (cache.connect()) {
            cache.initialize_schema();
        }
    }

    // Validate deploy token before running (simulates gated CI)
    if (config.deploy_token.empty()) {
        std::cout << "\nWARNING: DEPLOY_TOKEN not set. "
                  << "Results will not be published." << std::endl;
    }

    // Run all benchmarks with caching
    run_and_cache(cache, "matrix_multiply", benchmark_matrix_ops);
    run_and_cache(cache, "hashing", benchmark_hashing);
    run_and_cache(cache, "string_search", benchmark_string_ops);
    run_and_cache(cache, "memory_ops", benchmark_memory_ops);
    run_and_cache(cache, "polynomial_eval", benchmark_polynomial);

    // Finalize and report
    cache.finalize_report();

    std::cout << "\n========================================" << std::endl;
    std::cout << "  All benchmarks completed!" << std::endl;
    std::cout << "========================================" << std::endl;

    return 0;
}

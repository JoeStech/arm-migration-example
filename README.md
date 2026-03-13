# Compute Benchmark Suite

A high-performance compute benchmark application optimized for x86-64 architecture with SSE2 SIMD instructions. Results are cached to PostgreSQL for cross-architecture comparison.

## Overview

This benchmark suite tests various compute-intensive operations including:
- Matrix multiplication (200x200 matrices)
- Cryptographic hashing (10MB data processing)
- String pattern matching (4.5M character text search)
- Memory operations (50MB copy operations)
- Polynomial evaluation (10M iterations)

Results are written to a PostgreSQL database so that runs across different architectures (x86 vs Arm) can be compared over time.

## Building with Docker

```bash
docker build -t benchmark-suite .
```

## Running

### Without database (standalone mode)

```bash
docker run --rm -e CACHE_ENABLED=false benchmark-suite
```

### With PostgreSQL caching

```bash
docker run --rm \
  -e BENCHMARK_DB_HOST=your-db-host \
  -e BENCHMARK_DB_PORT=5432 \
  -e BENCHMARK_DB_NAME=benchmarks \
  -e BENCHMARK_DB_USER=bench_user \
  -e BENCHMARK_DB_PASSWORD=your-password \
  -e REPORTING_API_KEY=your-api-key \
  -e DEPLOY_TOKEN=your-deploy-token \
  benchmark-suite
```

## GitHub Actions

The CI workflow (`.github/workflows/benchmark-ci.yaml`) runs benchmarks with a Postgres service container and caches results automatically.

### Required Secrets

Configure these in your repository settings under Settings > Secrets and variables > Actions:

| Secret | Description |
|--------|-------------|
| `BENCHMARK_DB_PASSWORD` | PostgreSQL password for the benchmark database |
| `REPORTING_API_KEY` | API key for external performance reporting service |
| `DEPLOY_TOKEN` | Authorization token for publishing benchmark results |

Without these secrets, the benchmarks still run but results are not persisted or published.

## Architecture Notes

- **Optimized for**: x86-64 architecture with SSE2 support
- **SIMD Instructions**: Uses SSE2 intrinsics (`__m128d`, `__m128i`) for vectorized operations
- **CPUID**: Uses inline assembly to detect CPU vendor on x86
- **Fallback**: Includes scalar fallback for non-x86 platforms
- **Database**: Uses libpq (PostgreSQL C client) for result caching
- **Secrets**: DB credentials and API keys loaded from environment variables (GitHub Actions secrets)

## Project Structure

- `main.cpp` — Entry point, benchmark orchestration, caching integration
- `matrix_operations.{h,cpp}` — Matrix multiplication with SSE2
- `hash_operations.{h,cpp}` — Hashing with SIMD acceleration
- `string_search.{h,cpp}` — String pattern matching using SSE2
- `memory_operations.{h,cpp}` — Fast memory copy operations
- `polynomial_eval.{h,cpp}` — Vectorized polynomial evaluation
- `secrets_config.{h,cpp}` — Environment-based configuration (secrets)
- `cache_operations.{h,cpp}` — PostgreSQL benchmark result caching
- `Dockerfile` — Container build with libpq
- `.github/workflows/benchmark-ci.yaml` — CI with Postgres service and secrets

## Migration Notes

When migrating this application to Arm:
1. The x86 SSE2 intrinsics need to be replaced with Arm NEON equivalents
2. The `cpuid` inline assembly in `cache_operations.cpp` is x86-specific
3. The PostgreSQL dependency (`libpq`) must be available for the target architecture
4. GitHub Actions secrets must be configured for any test environment
5. The Postgres service container in CI needs an Arm-compatible image

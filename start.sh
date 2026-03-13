#!/bin/bash

echo "Running compute benchmark suite..."
echo "====================================="

# Show architecture info
echo "Architecture: $(uname -m)"
echo "Kernel: $(uname -r)"

# Check if DB caching is configured
if [ -n "$BENCHMARK_DB_PASSWORD" ]; then
    echo "DB caching: enabled (${BENCHMARK_DB_HOST}:${BENCHMARK_DB_PORT})"
else
    echo "DB caching: disabled (no credentials)"
fi

echo "====================================="
./benchmark

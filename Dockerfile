FROM ubuntu:22.04

# Install build dependencies including libpq for PostgreSQL connectivity
RUN apt-get update && apt-get install -y \
    g++ \
    make \
    libpq-dev \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /app

# Copy all header files
COPY *.h ./

# Copy all C++ source files
COPY *.cpp ./

# Build the application with optimizations
# SSE2 intrinsics are used in the code for x86-64 platforms
# Links against libpq for PostgreSQL caching
RUN g++ -O2 -o benchmark \
    main.cpp \
    matrix_operations.cpp \
    hash_operations.cpp \
    string_search.cpp \
    memory_operations.cpp \
    polynomial_eval.cpp \
    secrets_config.cpp \
    cache_operations.cpp \
    -std=c++11 \
    -lpq

# Copy startup script
COPY start.sh .
RUN chmod +x start.sh

# Default environment (overridden by GitHub Actions secrets)
ENV CACHE_ENABLED=true
ENV BENCHMARK_DB_HOST=localhost
ENV BENCHMARK_DB_PORT=5432
ENV BENCHMARK_DB_NAME=benchmarks
ENV BENCHMARK_DB_USER=bench_user

# Run the application
CMD ["./start.sh"]

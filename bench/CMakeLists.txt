# Performance micro-benchmarks using Google Benchmark.
#
# Build:
#   cmake -B build-bench -DCMAKE_BUILD_TYPE=Release -DAETHER_BUILD_BENCHMARKS=ON
#   cmake --build build-bench -j$(nproc)
#
# Run:
#   ./build-bench/bench/aether_benchmarks
#   ./build-bench/bench/aether_benchmarks --benchmark_filter=RoundTrip
#   ./build-bench/bench/aether_benchmarks --benchmark_format=json

find_package(benchmark QUIET)
if(NOT benchmark_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        googlebenchmark
        GIT_REPOSITORY https://github.com/google/benchmark.git
        GIT_TAG        v1.9.1
    )
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googlebenchmark)
endif()

add_executable(aether_benchmarks
    bench_frameio.cpp
    bench_roundtrip.cpp
    bench_notify.cpp
    bench_comparison.cpp
)
target_link_libraries(aether_benchmarks PRIVATE aether benchmark::benchmark_main Threads::Threads)

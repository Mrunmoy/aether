# Fuzz targets require Clang with libFuzzer support.
# Build: cmake -B build-fuzz -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++
#        -DCMAKE_BUILD_TYPE=Debug -DAETHER_BUILD_FUZZ=ON -DAETHER_BUILD_LITE=ON
#        cmake --build build-fuzz
#
# Run:   ./build-fuzz/test/fuzz/fuzz_frameio test/fuzz/corpus/frameio -max_total_time=60
#         ./build-fuzz/test/fuzz/fuzz_aether_lite test/fuzz/corpus/aether_lite -max_total_time=60

# ── FrameIO harness ─────────────────────────────────────────────────
add_executable(fuzz_frameio fuzz_frameio.cpp)
target_compile_options(fuzz_frameio PRIVATE -fsanitize=fuzzer,address)
target_link_options(fuzz_frameio PRIVATE -fsanitize=fuzzer,address)
target_link_libraries(fuzz_frameio PRIVATE aether)

# ── aether-lite parser harness ──────────────────────────────────────
add_executable(fuzz_aether_lite fuzz_aether_lite.cpp)
target_compile_options(fuzz_aether_lite PRIVATE -fsanitize=fuzzer,address)
target_link_options(fuzz_aether_lite PRIVATE -fsanitize=fuzzer,address)
target_link_libraries(fuzz_aether_lite PRIVATE aether_lite)
target_include_directories(fuzz_aether_lite PRIVATE
    ${CMAKE_SOURCE_DIR}/aether-lite
)

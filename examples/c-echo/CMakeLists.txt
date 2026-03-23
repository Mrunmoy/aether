# c-echo — C API example
# Demonstrates using the Aether IPC C API from pure C code.

# ── Standalone SDK build (pkg-config) ────────────────────────────────
# cc -o echo_server echo_server.c $(pkg-config --cflags --libs aether)
# cc -o echo_client echo_client.c $(pkg-config --cflags --libs aether)

# ── In-tree build ────────────────────────────────────────────────────
cmake_minimum_required(VERSION 3.14)

# Enable C language for this subdirectory
enable_language(C)

add_executable(c_echo_server echo_server.c)
target_link_libraries(c_echo_server PRIVATE aether)
target_include_directories(c_echo_server PRIVATE ${CMAKE_SOURCE_DIR}/inc)

add_executable(c_echo_client echo_client.c)
target_link_libraries(c_echo_client PRIVATE aether)
target_include_directories(c_echo_client PRIVATE ${CMAKE_SOURCE_DIR}/inc)

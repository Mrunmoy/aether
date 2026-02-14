# Owns googletest and all test targets.
#
# If it hasn't been cloned, print a helpful message and skip.

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/googletest/CMakeLists.txt")
    message(STATUS "ms-ipc: test/vendor/googletest not found, skipping tests. "
                   "Run: git submodule update --init --recursive")
    return()
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
add_subdirectory(vendor/googletest)

# ── Core tests ────────────────────────────────────────────────────────
add_executable(ipc_tests
    PlatformTest.cpp
    ConnectionTest.cpp
    FrameIOTest.cpp
    ServiceBaseTest.cpp
    ClientBaseTest.cpp
)
target_link_libraries(ipc_tests PRIVATE ms-ipc GTest::gtest_main pthread)

# ── Code-gen server tests ─────────────────────────────────────────────
# Uses the generated server-side DeviceMonitor (inherits ServiceBase).
# Paired with a raw ClientBase to test the dispatch without needing the
# generated client in the same binary (both sides define ms::ipc::DeviceMonitor).
add_executable(codegen_server_tests
    CodeGenServerTest.cpp
    gen/server/DeviceMonitor.cpp
)
target_include_directories(codegen_server_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
)
target_link_libraries(codegen_server_tests PRIVATE ms-ipc GTest::gtest_main pthread)

# ── Code-gen client tests ─────────────────────────────────────────────
# Uses the generated client-side DeviceMonitor (inherits ClientBase).
# Paired with a raw ServiceBase stub to simulate the server protocol.
add_executable(codegen_client_tests
    CodeGenClientTest.cpp
    gen/client/DeviceMonitor.cpp
)
target_include_directories(codegen_client_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
)
target_link_libraries(codegen_client_tests PRIVATE ms-ipc GTest::gtest_main pthread)

include(GoogleTest)
gtest_discover_tests(ipc_tests)
gtest_discover_tests(codegen_server_tests)
gtest_discover_tests(codegen_client_tests)

add_executable(echo_server echo_server.cpp)
target_link_libraries(echo_server PRIVATE ms-ipc pthread)

add_executable(echo_client echo_client.cpp)
target_link_libraries(echo_client PRIVATE ms-ipc pthread)

# ── Code-gen examples (DeviceMonitor) ─────────────────────────────────
# Generated from example/DeviceMonitor.idl via tools/ipcgen.py.
# Server and client are separate executables because both sides define
# ms::ipc::DeviceMonitor (same class name, different base classes).

add_executable(codegen_server_tests
    CodeGenServerTest.cpp
    gen/server/DeviceMonitor.cpp
)
target_include_directories(codegen_server_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(codegen_server_tests PRIVATE ms-ipc GTest::gtest_main pthread)

add_executable(codegen_client_tests
    CodeGenClientTest.cpp
    gen/client/DeviceMonitor.cpp
)
target_include_directories(codegen_client_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(codegen_client_tests PRIVATE ms-ipc GTest::gtest_main pthread)

include(GoogleTest)
gtest_discover_tests(codegen_server_tests)
gtest_discover_tests(codegen_client_tests)

add_executable(echo_server echo_server.cpp)
target_link_libraries(echo_server PRIVATE aether Threads::Threads)

add_executable(echo_client echo_client.cpp)
target_link_libraries(echo_client PRIVATE aether Threads::Threads)

# ── Code-gen examples (DeviceMonitor) ─────────────────────────────────
# Generated from example/DeviceMonitor.idl via tools/ipcgen.py.
# Server and client are separate executables because both sides define
# aether::ipc::DeviceMonitor (same class name, different base classes).

add_executable(codegen_server_tests
    CodeGenServerTest.cpp
    gen/server/DeviceMonitor.cpp
)
target_include_directories(codegen_server_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(codegen_server_tests PRIVATE aether GTest::gtest_main Threads::Threads)

add_executable(codegen_client_tests
    CodeGenClientTest.cpp
    gen/client/DeviceMonitor.cpp
)
target_include_directories(codegen_client_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(codegen_client_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
if(WIN32)
    add_test(NAME codegen_server_tests COMMAND codegen_server_tests)
    set_tests_properties(codegen_server_tests PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:aether_shared>;$<TARGET_FILE_DIR:vortex_shared>"
    )

    add_test(NAME codegen_client_tests COMMAND codegen_client_tests)
    set_tests_properties(codegen_client_tests PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:aether_shared>;$<TARGET_FILE_DIR:vortex_shared>"
    )
else()
    gtest_discover_tests(codegen_server_tests)
    gtest_discover_tests(codegen_client_tests)
endif()

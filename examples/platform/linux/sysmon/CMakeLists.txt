# ── SysMon example (Linux /proc system monitor) ─────────────────────

add_executable(sysmon_device
    sysmon_device.cpp
    gen/server/SysMon.cpp
)
target_include_directories(sysmon_device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(sysmon_device PRIVATE aether Threads::Threads)

add_executable(sysmon_client
    sysmon_client.cpp
    gen/client/SysMon.cpp
)
target_include_directories(sysmon_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(sysmon_client PRIVATE aether Threads::Threads)

add_executable(sysmon_tests
    sysmon_test.cpp
    gen/server/SysMon.cpp
)
target_include_directories(sysmon_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(sysmon_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
if(AETHER_SANITIZERS)
    add_test(NAME sysmon_tests COMMAND sysmon_tests)
else()
    gtest_discover_tests(sysmon_tests)
endif()

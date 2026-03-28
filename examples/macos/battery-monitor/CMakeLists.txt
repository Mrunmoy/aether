add_executable(battery_device
    battery_device.cpp
    gen/server/BatteryMonitor.cpp
)
target_include_directories(battery_device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(battery_device PRIVATE aether Threads::Threads)

add_executable(battery_client
    battery_client.cpp
    gen/client/BatteryMonitor.cpp
)
target_include_directories(battery_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(battery_client PRIVATE aether Threads::Threads)

add_executable(battery_tests
    battery_test.cpp
    gen/server/BatteryMonitor.cpp
)
target_include_directories(battery_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(battery_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
if(WIN32)
    add_test(NAME battery_tests COMMAND battery_tests)
    set_tests_properties(battery_tests PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:aether_shared>\;$<TARGET_FILE_DIR:vortex_shared>"
    )
else()
    gtest_discover_tests(battery_tests)
endif()

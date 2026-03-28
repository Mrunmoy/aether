add_executable(ble_device
    ble_device.cpp
    gen/server/BlePeripheral.cpp
)
target_include_directories(ble_device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(ble_device PRIVATE aether Threads::Threads)

add_executable(ble_dashboard
    ble_dashboard.cpp
    gen/client/BlePeripheral.cpp
)
target_include_directories(ble_dashboard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(ble_dashboard PRIVATE aether Threads::Threads)

add_executable(ble_tests
    ble_test.cpp
    gen/server/BlePeripheral.cpp
)
target_include_directories(ble_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(ble_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
if(WIN32)
    add_test(NAME ble_tests COMMAND ble_tests)
    set_tests_properties(ble_tests PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:aether_shared>\;$<TARGET_FILE_DIR:vortex_shared>"
    )
else()
    gtest_discover_tests(ble_tests)
endif()

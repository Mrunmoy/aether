add_executable(ecu_device ecu_device.cpp gen/server/CanBusEcu.cpp)
target_include_directories(ecu_device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(ecu_device PRIVATE aether Threads::Threads)

add_executable(ecu_dashboard ecu_dashboard.cpp gen/client/CanBusEcu.cpp)
target_include_directories(ecu_dashboard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(ecu_dashboard PRIVATE aether Threads::Threads)

add_executable(ecu_tests ecu_test.cpp gen/server/CanBusEcu.cpp)
target_include_directories(ecu_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(ecu_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
if(WIN32)
    add_test(NAME ecu_tests COMMAND ecu_tests)
    set_tests_properties(ecu_tests PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:aether_shared>\;$<TARGET_FILE_DIR:vortex_shared>"
    )
else()
    gtest_discover_tests(ecu_tests)
endif()

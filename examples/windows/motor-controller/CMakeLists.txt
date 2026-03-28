add_executable(motor_device
    motor_device.cpp
    gen/server/MotorController.cpp
)
target_include_directories(motor_device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(motor_device PRIVATE aether Threads::Threads)

add_executable(motor_dashboard
    motor_dashboard.cpp
    gen/client/MotorController.cpp
)
target_include_directories(motor_dashboard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(motor_dashboard PRIVATE aether Threads::Threads)

add_executable(motor_tests
    motor_test.cpp
    gen/server/MotorController.cpp
)
target_include_directories(motor_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(motor_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
if(WIN32)
    add_test(NAME motor_tests COMMAND motor_tests)
    set_tests_properties(motor_tests PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:aether_shared>\;$<TARGET_FILE_DIR:vortex_shared>"
    )
else()
    gtest_discover_tests(motor_tests)
endif()

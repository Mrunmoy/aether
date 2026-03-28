add_executable(sensor_device
    sensor_device.cpp
    gen/server/SensorHub.cpp
)
target_include_directories(sensor_device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(sensor_device PRIVATE aether Threads::Threads)

add_executable(sensor_dashboard
    sensor_dashboard.cpp
    gen/client/SensorHub.cpp
)
target_include_directories(sensor_dashboard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(sensor_dashboard PRIVATE aether Threads::Threads)

add_executable(sensor_tests
    sensor_test.cpp
    gen/server/SensorHub.cpp
)
target_include_directories(sensor_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(sensor_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
gtest_discover_tests(sensor_tests)

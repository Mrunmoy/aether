add_executable(gpio_device gpio_device.cpp gen/server/GpioController.cpp)
target_include_directories(gpio_device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(gpio_device PRIVATE aether Threads::Threads)

add_executable(gpio_dashboard gpio_dashboard.cpp gen/client/GpioController.cpp)
target_include_directories(gpio_dashboard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(gpio_dashboard PRIVATE aether Threads::Threads)

add_executable(gpio_tests gpio_test.cpp gen/server/GpioController.cpp)
target_include_directories(gpio_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(gpio_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
gtest_discover_tests(gpio_tests)

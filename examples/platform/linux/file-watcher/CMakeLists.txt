add_executable(filewatcher_device
    filewatcher_device.cpp
    gen/server/FileWatcher.cpp
)
target_include_directories(filewatcher_device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(filewatcher_device PRIVATE aether Threads::Threads)

add_executable(filewatcher_client
    filewatcher_client.cpp
    gen/client/FileWatcher.cpp
)
target_include_directories(filewatcher_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(filewatcher_client PRIVATE aether Threads::Threads)

add_executable(filewatcher_tests
    filewatcher_test.cpp
    gen/server/FileWatcher.cpp
)
target_include_directories(filewatcher_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(filewatcher_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
gtest_discover_tests(filewatcher_tests)

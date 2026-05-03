add_executable(disk_device
    disk_device.cpp
    gen/server/DiskUsage.cpp
)
target_include_directories(disk_device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(disk_device PRIVATE aether Threads::Threads)

add_executable(disk_client
    disk_client.cpp
    gen/client/DiskUsage.cpp
)
target_include_directories(disk_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(disk_client PRIVATE aether Threads::Threads)

add_executable(disk_tests
    disk_test.cpp
    gen/server/DiskUsage.cpp
)
target_include_directories(disk_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(disk_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
if(WIN32 OR AETHER_SANITIZERS)
    add_test(NAME disk_tests COMMAND disk_tests)
    if(WIN32)
        set_tests_properties(disk_tests PROPERTIES
            ENVIRONMENT_MODIFICATION
                "PATH=path_list_prepend:$<TARGET_FILE_DIR:aether_shared>\;$<TARGET_FILE_DIR:vortex_shared>"
        )
    endif()
else()
    gtest_discover_tests(disk_tests)
endif()

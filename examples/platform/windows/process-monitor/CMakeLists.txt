add_executable(procmon_device procmon_device.cpp gen/server/ProcessMonitor.cpp)
target_include_directories(procmon_device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(procmon_device PRIVATE aether Threads::Threads)

add_executable(procmon_client procmon_client.cpp gen/client/ProcessMonitor.cpp)
target_include_directories(procmon_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(procmon_client PRIVATE aether Threads::Threads)

add_executable(procmon_tests procmon_test.cpp gen/server/ProcessMonitor.cpp)
target_include_directories(procmon_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(procmon_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
if(WIN32 OR AETHER_SANITIZERS)
    add_test(NAME procmon_tests COMMAND procmon_tests)
    if(WIN32)
        set_tests_properties(procmon_tests PROPERTIES
            ENVIRONMENT_MODIFICATION
                "PATH=path_list_prepend:$<TARGET_FILE_DIR:aether_shared>\;$<TARGET_FILE_DIR:vortex_shared>"
        )
    endif()
else()
    gtest_discover_tests(procmon_tests)
endif()

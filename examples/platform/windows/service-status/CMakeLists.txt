add_executable(svcstat_device svcstat_device.cpp gen/server/ServiceStatus.cpp)
target_include_directories(svcstat_device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(svcstat_device PRIVATE aether Threads::Threads)

add_executable(svcstat_client svcstat_client.cpp gen/client/ServiceStatus.cpp)
target_include_directories(svcstat_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(svcstat_client PRIVATE aether Threads::Threads)

add_executable(svcstat_tests svcstat_test.cpp gen/server/ServiceStatus.cpp)
target_include_directories(svcstat_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(svcstat_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
if(WIN32 OR AETHER_SANITIZERS)
    add_test(NAME svcstat_tests COMMAND svcstat_tests)
    if(WIN32)
        set_tests_properties(svcstat_tests PROPERTIES
            ENVIRONMENT_MODIFICATION
                "PATH=path_list_prepend:$<TARGET_FILE_DIR:aether_shared>\;$<TARGET_FILE_DIR:vortex_shared>"
        )
    endif()
else()
    gtest_discover_tests(svcstat_tests)
endif()

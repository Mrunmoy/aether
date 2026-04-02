add_executable(dsp_device dsp_device.cpp gen/server/AudioDsp.cpp)
target_include_directories(dsp_device PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(dsp_device PRIVATE aether Threads::Threads)

add_executable(dsp_dashboard dsp_dashboard.cpp gen/client/AudioDsp.cpp)
target_include_directories(dsp_dashboard PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(dsp_dashboard PRIVATE aether Threads::Threads)

add_executable(dsp_tests dsp_test.cpp gen/server/AudioDsp.cpp)
target_include_directories(dsp_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(dsp_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
if(WIN32)
    add_test(NAME dsp_tests COMMAND dsp_tests)
    set_tests_properties(dsp_tests PROPERTIES
        ENVIRONMENT_MODIFICATION
            "PATH=path_list_prepend:$<TARGET_FILE_DIR:aether_shared>\;$<TARGET_FILE_DIR:vortex_shared>"
    )
else()
    gtest_discover_tests(dsp_tests)
endif()

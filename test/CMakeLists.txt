# Owns googletest and all test targets.
#
# If it hasn't been cloned, print a helpful message and skip.

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/vendor/googletest/CMakeLists.txt")
    message(STATUS "aether: test/vendor/googletest not found, skipping tests. "
                   "Run: git submodule update --init --recursive")
    return()
endif()

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
add_subdirectory(vendor/googletest)

add_executable(ipc_tests
    PlatformTest.cpp
    ConnectionTest.cpp
    FrameIOTest.cpp
    ServiceBaseTest.cpp
    ClientBaseTest.cpp
    CApiTest.cpp
    EndianTest.cpp
    TransportClientBaseTest.cpp
)
target_link_libraries(ipc_tests PRIVATE aether GTest::gtest_main pthread)

include(GoogleTest)
gtest_discover_tests(ipc_tests)

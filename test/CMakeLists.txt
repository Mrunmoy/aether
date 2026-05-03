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
    ConnectionTest.cpp
    FrameIOTest.cpp
    ServiceBaseTest.cpp
    ClientBaseTest.cpp
    CApiTest.cpp
    EndianTest.cpp
    TransportClientBaseTest.cpp
)
if(WIN32)
    target_sources(ipc_tests PRIVATE PlatformWindowsTest.cpp)
else()
    target_sources(ipc_tests PRIVATE PlatformTest.cpp)
endif()
target_link_libraries(ipc_tests PRIVATE aether GTest::gtest_main Threads::Threads)

include(GoogleTest)
if(WIN32 OR AETHER_SANITIZERS)
    add_test(NAME ipc_tests COMMAND ipc_tests)
else()
    gtest_discover_tests(ipc_tests)
endif()

# ── Aether-Lite tests ───────────────────────────────────────────
if(TARGET aether_lite)
    add_executable(aether_lite_tests AetherLiteTest.cpp)
    target_link_libraries(aether_lite_tests PRIVATE aether_lite GTest::gtest_main)
    if(AETHER_SANITIZERS)
        add_test(NAME aether_lite_tests COMMAND aether_lite_tests)
    else()
        gtest_discover_tests(aether_lite_tests)
    endif()
endif()

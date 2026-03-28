add_subdirectory(echo)
add_subdirectory(c-echo)

if(NOT WIN32)
    add_subdirectory(serial-loopback)
endif()

# Qt5 is optional — only build exhaust-analyzer when Qt5Widgets is found.
find_package(Qt5 COMPONENTS Widgets QUIET)
if(Qt5_FOUND)
    add_subdirectory(exhaust-analyzer)
else()
    message(STATUS "aether: Qt5 not found — skipping exhaust-analyzer example")
endif()

if(NOT WIN32)
    add_subdirectory(serial-sensor)
endif()

# Platform examples — simulated device examples are cross-platform;
# OS-level examples are guarded inside each platform's CMakeLists.txt.
add_subdirectory(linux)
add_subdirectory(macos)
add_subdirectory(windows)

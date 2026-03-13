add_subdirectory(echo)

# Qt5 is optional — only build exhaust-analyzer when Qt5Widgets is found.
find_package(Qt5 COMPONENTS Widgets QUIET)
if(Qt5_FOUND)
    add_subdirectory(exhaust-analyzer)
else()
    message(STATUS "ms-ipc: Qt5 not found — skipping exhaust-analyzer example")
endif()

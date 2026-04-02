# Simulated embedded devices (cross-platform)
add_subdirectory(gpio-controller)
add_subdirectory(sensor-hub)

# OS-level examples (Linux only — use /proc, inotify)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_subdirectory(sysmon)
    add_subdirectory(file-watcher)
endif()

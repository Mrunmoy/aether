cmake_minimum_required(VERSION 3.14)

project(ms-ipc
    LANGUAGES CXX
    DESCRIPTION "IPC framework — UDS signaling, shared memory transport, FD passing"
)

# ── Library ──────────────────────────────────────────────────────────
add_library(ms-ipc src/PlatformLinux.cpp)
target_include_directories(ms-ipc PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(ms-ipc PUBLIC cxx_std_17)
target_link_libraries(ms-ipc PRIVATE pthread)

# ── Tests (only when building standalone) ────────────────────────────
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test)
endif()

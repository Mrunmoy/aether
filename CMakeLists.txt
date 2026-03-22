cmake_minimum_required(VERSION 3.14)

project(aether
    VERSION 1.1.0
    LANGUAGES CXX
    DESCRIPTION "IPC framework — UDS signaling, shared memory transport, FD passing"
)

# ── Dependencies ─────────────────────────────────────────────────────
add_subdirectory(deps/ms-ringbuffer)
add_subdirectory(deps/ms-runloop)

# ── Library ──────────────────────────────────────────────────────────
add_library(aether
    src/PlatformLinux.cpp
    src/Connection.cpp
    src/FrameIO.cpp
    src/ServiceBase.cpp
    src/ClientBase.cpp
)
target_include_directories(aether PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(aether PUBLIC cxx_std_17)
target_link_libraries(aether PUBLIC ms-ringbuffer ms-runloop PRIVATE pthread)

# ── Tests (only when building standalone) ────────────────────────────
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test)

    option(AETHER_BUILD_EXAMPLES "Build examples" OFF)
    if(AETHER_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()

cmake_minimum_required(VERSION 3.14)

project(aether
    VERSION 1.1.0
    LANGUAGES CXX
    DESCRIPTION "IPC framework — UDS signaling, shared memory transport, FD passing"
)

# ── Dependencies ─────────────────────────────────────────────────────
# Enable PIC globally so deps can also be linked into the shared library.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_subdirectory(deps/ouroboros)
add_subdirectory(deps/vortex)

# ── Static library ───────────────────────────────────────────────────
add_library(aether STATIC
    src/PlatformLinux.cpp
    src/Connection.cpp
    src/FrameIO.cpp
    src/ServiceBase.cpp
    src/ClientBase.cpp
    src/TransportClientBase.cpp
    src/CApi.cpp
)
target_include_directories(aether PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(aether PUBLIC cxx_std_17)
target_link_libraries(aether PUBLIC ouroboros vortex PRIVATE pthread)

# ── Shared library ───────────────────────────────────────────────────
add_library(aether_shared SHARED
    src/PlatformLinux.cpp
    src/Connection.cpp
    src/FrameIO.cpp
    src/ServiceBase.cpp
    src/ClientBase.cpp
    src/TransportClientBase.cpp
    src/CApi.cpp
)
target_include_directories(aether_shared PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/inc>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(aether_shared PUBLIC cxx_std_17)
target_link_libraries(aether_shared PRIVATE ouroboros vortex pthread)
set_target_properties(aether_shared PROPERTIES
    OUTPUT_NAME aether
    VERSION     ${PROJECT_VERSION}
    SOVERSION   ${PROJECT_VERSION_MAJOR}
)

# ── Install ──────────────────────────────────────────────────────────
include(GNUInstallDirs)

install(TARGETS aether aether_shared
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Only install the C API header for SDK consumers
install(FILES inc/aether_ipc.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

# pkg-config
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/aether.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/aether.pc
    @ONLY
)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/aether.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)

# CMake package config (hand-written, no EXPORT needed)
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/cmake/aetherConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/aetherConfig.cmake
    @ONLY
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/aetherConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)
install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/aetherConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/aetherConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/aether
)

# ── Tests (only when building standalone) ────────────────────────────
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test)

    option(AETHER_BUILD_EXAMPLES "Build examples" OFF)
    if(AETHER_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()

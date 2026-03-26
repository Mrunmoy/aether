cmake_minimum_required(VERSION 3.14)

project(aether
    VERSION 1.2.0
    LANGUAGES CXX
    DESCRIPTION "IPC framework — UDS signaling, shared memory transport, FD passing"
)

# ── Compiler warnings ────────────────────────────────────────────────
if(MSVC)
    add_compile_options(/W4 /WX)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -Werror)
endif()

# ── Dependencies ─────────────────────────────────────────────────────
# Enable PIC globally so deps can also be linked into the shared library.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
find_package(Threads REQUIRED)

# On Windows, place all .exe and .dll outputs in the same directory so
# test executables can find dependency DLLs (e.g. vortex.dll) at both
# build time (gtest_discover_tests) and ctest time.
if(WIN32)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif()

add_subdirectory(deps/ouroboros)
add_subdirectory(deps/vortex)

if(WIN32)
    set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif()

if(WIN32)
    set(AETHER_PKGCONFIG_THREADS_LIB "")
else()
    set(AETHER_PKGCONFIG_THREADS_LIB "-lpthread")
endif()

if(APPLE)
    set(AETHER_PLATFORM_SRC src/PlatformMac.cpp)
elseif(WIN32)
    set(AETHER_PLATFORM_SRC src/PlatformWindows.cpp)
else()
    set(AETHER_PLATFORM_SRC src/PlatformLinux.cpp)
endif()

# ── Static library ───────────────────────────────────────────────────
add_library(aether STATIC
    ${AETHER_PLATFORM_SRC}
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
target_link_libraries(aether PUBLIC ouroboros vortex Threads::Threads)

# ── Shared library ───────────────────────────────────────────────────
add_library(aether_shared SHARED
    ${AETHER_PLATFORM_SRC}
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
target_link_libraries(aether_shared PRIVATE ouroboros vortex Threads::Threads)
if(WIN32)
    set_target_properties(aether_shared PROPERTIES OUTPUT_NAME aether_shared)
else()
    set_target_properties(aether_shared PROPERTIES
        OUTPUT_NAME aether
        VERSION     ${PROJECT_VERSION}
        SOVERSION   ${PROJECT_VERSION_MAJOR}
    )
endif()

# ── Install ──────────────────────────────────────────────────────────
include(GNUInstallDirs)

install(TARGETS aether aether_shared
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
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

# ── Aether-Lite (C99 bare-metal embedded runtime) ────────────────
option(AETHER_BUILD_LITE "Build aether-lite C99 embedded runtime" OFF)
if(AETHER_BUILD_LITE)
    add_subdirectory(aether-lite)
endif()

# ── Fuzz testing (requires Clang with libFuzzer) ────────────────
option(AETHER_BUILD_FUZZ "Build libFuzzer harnesses" OFF)
if(AETHER_BUILD_FUZZ)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message(FATAL_ERROR "AETHER_BUILD_FUZZ requires Clang (for -fsanitize=fuzzer)")
    endif()
    add_subdirectory(test/fuzz)
endif()

# ── Performance benchmarks (Google Benchmark) ───────────────────
option(AETHER_BUILD_BENCHMARKS "Build performance micro-benchmarks" OFF)
if(AETHER_BUILD_BENCHMARKS)
    add_subdirectory(bench)
endif()

# ── Tests (only when building standalone) ────────────────────────────
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    enable_testing()
    add_subdirectory(test)

    option(AETHER_BUILD_EXAMPLES "Build examples" OFF)
    if(AETHER_BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()

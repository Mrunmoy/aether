cmake_minimum_required(VERSION 3.14)

enable_language(C)

add_library(aether_lite STATIC
    aether_lite_crc.c
    aether_lite_framing.c
    aether_lite_dispatch.c
)

target_include_directories(aether_lite PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
)

set_target_properties(aether_lite PROPERTIES
    C_STANDARD 99
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
)

target_compile_options(aether_lite PRIVATE
    -Wall -Wextra -Wpedantic -Werror
)

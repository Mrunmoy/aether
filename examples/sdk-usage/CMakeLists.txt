cmake_minimum_required(VERSION 3.14)
project(aether_sdk_example CXX)

# Point to the extracted SDK tarball:
#   cmake -B build -DAETHER_SDK=/path/to/aether-sdk-1.1.0-linux-x86_64
set(AETHER_SDK "${CMAKE_CURRENT_SOURCE_DIR}/../../" CACHE PATH
    "Path to the extracted Aether SDK directory")

# Server — uses generated server class
add_executable(temp_server
    temp_server.cpp
    gen/server/TemperatureSensor.cpp)
target_include_directories(temp_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${AETHER_SDK}/include)
target_link_libraries(temp_server ${AETHER_SDK}/lib/libaether.a stdc++ pthread)
target_compile_features(temp_server PRIVATE cxx_std_17)

# Client — uses generated client class
add_executable(temp_client
    temp_client.cpp
    gen/client/TemperatureSensor.cpp)
target_include_directories(temp_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${AETHER_SDK}/include)
target_link_libraries(temp_client ${AETHER_SDK}/lib/libaether.a stdc++ pthread)
target_compile_features(temp_client PRIVATE cxx_std_17)

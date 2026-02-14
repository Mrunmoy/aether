add_executable(echo_server echo_server.cpp)
target_link_libraries(echo_server PRIVATE ms-ipc pthread)

add_executable(echo_client echo_client.cpp)
target_link_libraries(echo_client PRIVATE ms-ipc pthread)

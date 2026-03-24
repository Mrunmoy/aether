if(NOT WIN32)
    add_executable(serial_loopback serial_loopback.cpp)
    target_link_libraries(serial_loopback PRIVATE aether util)
endif()

if(NOT WIN32 AND TARGET aether_lite)
    enable_language(C)

    add_executable(serial_sensor
        serial_sensor.cpp
        sensor_device.c
    )
    target_link_libraries(serial_sensor PRIVATE aether aether_lite util)
endif()

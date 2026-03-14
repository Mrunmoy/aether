set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

# ── Server (no Qt dependency) ────────────────────────────
add_executable(exhaust_server
    exhaust_server.cpp
    gen/server/ExhaustAnalyzer.cpp
)
target_include_directories(exhaust_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/server
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(exhaust_server PRIVATE ms-ipc pthread)

# ── Client (Qt5 Widgets) ─────────────────────────────────
add_executable(exhaust_client
    exhaust_client.cpp
    gen/client/ExhaustAnalyzer.cpp
)
target_include_directories(exhaust_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/gen/client
    ${CMAKE_CURRENT_SOURCE_DIR}/gen
)
target_link_libraries(exhaust_client PRIVATE ms-ipc Qt5::Widgets pthread)

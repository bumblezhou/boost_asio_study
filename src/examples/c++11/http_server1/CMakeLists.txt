include_directories(
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server1/
)

add_custom_exe(http_server1
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server1/main.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server1/mime_types.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server1/reply.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server1/request_handler.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server1/request_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server1/connection.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server1/connection_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server1/server.cpp
)
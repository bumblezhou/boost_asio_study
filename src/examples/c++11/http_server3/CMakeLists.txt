include_directories(
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server3/
)

add_custom_exe(http_server3
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server3/main.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server3/mime_types.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server3/reply.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server3/request_handler.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server3/request_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server3/connection.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server3/server.cpp
)
include_directories(
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server2/
)

add_custom_exe(http_server2
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server2/main.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server2/mime_types.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server2/reply.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server2/request_handler.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server2/request_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server2/connection.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server2/io_context_pool.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server2/server.cpp
)
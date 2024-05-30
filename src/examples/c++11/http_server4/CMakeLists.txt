include_directories(
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server4/
)

add_custom_exe(http_server4
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server4/main.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server4/mime_types.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server4/reply.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server4/file_handler.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server4/request_parser.cpp
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/http_server4/server.cpp
)
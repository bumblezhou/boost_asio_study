include_directories(
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/chat/
)

add_custom_exe(chat_client
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/chat/chat_client.cpp
)

add_custom_exe(chat_server
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/chat/chat_server.cpp
)
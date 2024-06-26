include_directories(
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/socks4/
)

add_custom_exe(request_with_proxy
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/socks4/request_with_proxy.cpp
)

add_custom_exe(request_without_proxy
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/socks4/request_without_proxy.cpp
)

add_custom_exe(socks5_server
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/socks4/socks5_server.cpp
)

add_custom_exe(socks5_client
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/socks4/socks5_client.cpp
)
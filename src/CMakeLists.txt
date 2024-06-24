macro(add_custom_exe TARGET_NAME)
    # message("+------------------------------------------------------------+")
    # message("| --> ARGV: <<< ${ARGV} >>>")
	message("+------------------------------------------------------------+")
    message("| --> TARGET_NAME: <<< ${TARGET_NAME} >>>")
	message("+------------------------------------------------------------+")
    add_executable(${ARGV}
    )
    target_include_directories(${TARGET_NAME} PRIVATE
        ${CMAKE_SOURCE_DIR}/include
    )
    target_link_libraries(${TARGET_NAME} PRIVATE
        pthread
        ssl
        crypto
        boost_thread
    )
    target_compile_features(${TARGET_NAME} PRIVATE
        cxx_std_17
    )
endmacro()

add_subdirectory("tutorial")
add_subdirectory("examples/c++11/http_server1")
add_subdirectory("examples/c++11/http_server2")
add_subdirectory("examples/c++11/http_server3")
add_subdirectory("examples/c++11/http_server4")
add_subdirectory("examples/c++11/chat")
add_subdirectory("examples/c++11/operations")
add_subdirectory("examples/c++14/deferred")
add_subdirectory("examples/c++11/socks4")
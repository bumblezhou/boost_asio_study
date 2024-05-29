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
    )
    target_compile_features(${TARGET_NAME} PRIVATE
        cxx_std_17
    )
endmacro()

add_custom_exe(timer_1_synchronously
    timer_1_synchronously.cpp
)

add_custom_exe(timer_2_asynchronously
    timer_2_asynchronously.cpp
)

add_custom_exe(timer_3_binding_arguments_to_a_handler
    timer_3_binding_arguments_to_a_handler.cpp
)

add_custom_exe(timer_4_using_member_fun_as_handler
    timer_4_using_member_fun_as_handler.cpp
)

add_custom_exe(timer_5_synchronising_completion_handlers
    timer_5_synchronising_completion_handlers.cpp
)

add_custom_exe(tcp_client
    daytime_1_a_sync_tcp_daytime_client.cpp
)

add_custom_exe(sync_tcp_server
    daytime_2_a_sync_tcp_daytime_server.cpp
)

add_custom_exe(async_tcp_server
    daytime_3_an_async_tcp_daytime_server.cpp
)

add_custom_exe(udp_client
    daytime_4_a_sync_udp_daytime_client.cpp
)

add_custom_exe(sync_udp_server
    daytime_5_a_sync_udp_daytime_server.cpp
)

add_custom_exe(async_udp_server
    daytime_6_an_async_udp_daytime_server.cpp
)

add_custom_exe(async_tcp_udp_server
    daytime_7_async_tcp_udb_daytime_server.cpp
)

add_subdirectory("examples/c++11/http_server1")
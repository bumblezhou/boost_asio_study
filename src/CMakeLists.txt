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


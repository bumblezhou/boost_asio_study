include_directories(
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/operations/
)

add_custom_exe(composed_1
    ${CMAKE_SOURCE_DIR}/src/examples/c++11/operations/composed_1.cpp
)
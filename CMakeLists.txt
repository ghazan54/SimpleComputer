cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(sc)

add_compile_options(
    -g

    -Wall

    -Wextra

    # -Werror
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

include_directories(include/)

add_subdirectory(src/main)
add_subdirectory(src/libs)

set_target_properties(main PROPERTIES OUTPUT_NAME simple_computer.out)

include_directories(thirdparty/)

add_subdirectory(src/test)

set_target_properties(test PROPERTIES OUTPUT_NAME test.out)

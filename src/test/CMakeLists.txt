project(testfile)

add_executable(test main.c test.c)

target_link_libraries(test ram)

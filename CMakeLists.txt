cmake_minimum_required(VERSION 3.0)
project(gnss_ins)

add_subdirectory(../cvis ${PROJECT_BINARY_DIR}/cvis)

add_executable(main
        main.c)
target_link_libraries(main
        cvis)
cmake_minimum_required(VERSION 3.0)
project(gnss_ins)

if (NOT TARGET cgeodesy)
    add_subdirectory(../cgeodesy ${PROJECT_BINARY_DIR}/cgeodesy)
endif()
if (NOT TARGET cvis)
    add_subdirectory(../cvis ${PROJECT_BINARY_DIR}/cvis)
endif()

add_executable(main
        main.c)
target_link_libraries(main
        cvis
        cgeodesy)
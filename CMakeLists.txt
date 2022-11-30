cmake_minimum_required(VERSION 3.0)
project(gnss_ins)

find_package(Eigen3 REQUIRED)
find_package(GTest REQUIRED)

add_executable(main
        main.cpp)

cmake_minimum_required(VERSION 3.10.0)
project(compendulum)
set(CMAKE_CXX_STANDARD 17)
add_executable(cmp_tests euler_cmp.hpp euler_cmp.cpp)
enable_testing()
add_test(cmp_tests cmp_tests)

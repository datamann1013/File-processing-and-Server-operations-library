cmake_minimum_required(VERSION 3.30)
project(C__APIRepo)

set(CMAKE_CXX_STANDARD 20)

add_library(C__APIRepo SHARED library.cpp )
add_subdirectory(Google_tests)

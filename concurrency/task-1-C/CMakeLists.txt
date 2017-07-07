cmake_minimum_required(VERSION 3.6)
project(task)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(HEADER solution.h)
set(SOURCE_FILES main.cpp solution.cpp)
add_executable(task ${SOURCE_FILES})
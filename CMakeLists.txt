cmake_minimum_required(VERSION 3.16)
project(Json)

set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "-Wall -O2")
#set(CMAKE_BUILD_TYPE "Release")
include_directories(. googletest/include googletest)
add_subdirectory(lib)
add_executable(Json json_test.cpp json_generator.cpp json_parser.cpp json_value.cpp json.cpp)
target_link_libraries(Json gtest gtest_main)
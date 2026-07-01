cmake_minimum_required(VERSION 3.16)

project(common LANGUAGES CXX)

add_library(common STATIC)

target_sources(common INTERFACE include/TaskData.hpp include/State.hpp include/tm_common.hpp)

target_sources(common PUBLIC src/tm_common.cpp)

target_include_directories(common PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)

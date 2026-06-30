cmake_minimum_required(VERSION 3.16)

project(common LANGUAGES CXX)

add_library(common INTERFACE)

target_sources(common INTERFACE TaskData.hpp State.hpp)

target_include_directories(common INTERFACE ${CMAKE_CURRENT_LIST_DIR})

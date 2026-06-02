cmake_minimum_required(VERSION 3.16)

project(common LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(common STATIC
  common.cpp
  common.h
)

target_compile_definitions(common PRIVATE COMMON_LIBRARY)

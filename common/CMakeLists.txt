cmake_minimum_required(VERSION 3.16)

project(common LANGUAGES CXX)

add_library(common STATIC
  common.h
)

target_link_libraries(common PRIVATE serial::serial)

target_compile_definitions(common PRIVATE COMMON_LIBRARY)

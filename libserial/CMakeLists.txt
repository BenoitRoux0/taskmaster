cmake_minimum_required(VERSION 3.30)

project(
        serial
        VERSION 0.1.0
        DESCRIPTION "A library that provide a json serializer using c++26 reflection"
        LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 26)
add_compile_options(-freflection)

add_library(serial)
add_library(serial::serial ALIAS serial)

target_include_directories(serial PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

target_sources(serial PUBLIC
        src/extractRawValues.cpp
        src/skipWhitespaces.cpp
)

target_include_directories(serial INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)

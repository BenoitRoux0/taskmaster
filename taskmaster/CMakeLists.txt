cmake_minimum_required(VERSION 3.19)
project(taskmaster LANGUAGES CXX)

file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.3/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=a609e875fd532b067174250f6abbc3dac22fe2d64869783fb1e80bda1625c844
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMAddPackage("gh:ToruNiina/toml11@4.4.0")

find_package(CURL REQUIRED)

add_executable(taskmaster
        main.cpp
        src/Cli.cpp
        include/Cli.hpp
        src/CommandParser.cpp
        include/CommandParser.hpp
        src/HttpClient.cpp
        include/HttpClient.hpp
        include/ClientConfig.hpp
)

target_include_directories(taskmaster PRIVATE include)

target_link_libraries(taskmaster
        PRIVATE
        readline
        CURL::libcurl
        common
        serial::serial
)

install(TARGETS taskmaster
        BUNDLE DESTINATION .
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

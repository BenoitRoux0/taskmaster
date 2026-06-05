cmake_minimum_required(VERSION 3.19)
project(taskmasterd LANGUAGES CXX)

file(
        DOWNLOAD
        https://github.com/cpm-cmake/CPM.cmake/releases/download/v0.42.3/CPM.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake
        EXPECTED_HASH SHA256=a609e875fd532b067174250f6abbc3dac22fe2d64869783fb1e80bda1625c844
)
include(${CMAKE_CURRENT_BINARY_DIR}/cmake/CPM.cmake)

CPMAddPackage("gh:ToruNiina/toml11@4.4.0")

FetchContent_Declare(llhttp
        URL "https://github.com/nodejs/llhttp/archive/refs/tags/release/v9.4.1.tar.gz")

set(LLHTTP_BUILD_SHARED_LIBS OFF CACHE INTERNAL "")
set(LLHTTP_BUILD_STATIC_LIBS ON CACHE INTERNAL "")
FetchContent_MakeAvailable(llhttp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../common ${CMAKE_CURRENT_BINARY_DIR}/common)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../libserial ${CMAKE_CURRENT_BINARY_DIR}/libserial)

add_executable(taskmasterd
    main.cpp
        src/Server.cpp
        src/Task.cpp
        src/TaskManager.cpp
        src/HttpSessionSocket.cpp
        src/HttpRequest.cpp
        src/HttpRequestBuilder.cpp
        src/HttpResponse.cpp
        src/HttpResponseBuilder.cpp
        src/AcceptSocket.cpp
        src/Socket.cpp
        src/Logger.cpp
        src/SignalSocket.cpp
        src/RunningTask.cpp
        src/RunningTaskId.cpp
)

target_compile_options(taskmasterd PUBLIC -Wall -Wextra -Werror)

target_include_directories(taskmasterd PRIVATE include)

target_link_libraries(taskmasterd
    PRIVATE
        toml11::toml11
        llhttp_static
        common
        serial::serial
)

install(TARGETS taskmasterd
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

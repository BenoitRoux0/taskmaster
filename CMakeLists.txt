cmake_minimum_required(VERSION 3.19)

option(BUILD_CLIENT "Enable building client" ON)
option(BUILD_DAEMON "Enable building daemon" ON)

project(TaskMaster)

set(CMAKE_CXX_STANDARD 23)
add_compile_options(-Wall -Wextra -Werror)
add_link_options(-Wl,-rpath=${GCC_PATH}/lib64)

if(BUILD_CLIENT)
    add_subdirectory(taskmaster)
endif()
if(BUILD_DAEMON)
    add_subdirectory(taskmasterd)
endif()

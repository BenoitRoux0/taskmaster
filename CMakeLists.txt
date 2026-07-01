cmake_minimum_required(VERSION 3.19)

option(BUILD_CLIENT "Enable building client" ON)
option(BUILD_DAEMON "Enable building daemon" ON)

project(TaskMaster)

set(CMAKE_CXX_STANDARD 26)
add_compile_options(-freflection)
add_link_options(-Wl,-rpath=${GCC_PATH}/lib64)

add_subdirectory(common)
add_subdirectory(libserial)

if(BUILD_CLIENT)
    add_subdirectory(taskmaster)
endif()
if(BUILD_DAEMON)
    add_subdirectory(taskmasterd)
endif()

cmake_minimum_required(VERSION 3.19)
project(taskmasterd LANGUAGES CXX)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core HttpServer Concurrent)
find_package(Qt6 REQUIRED COMPONENTS Core HttpServer Concurrent)

qt_standard_project_setup()

qt_add_executable(taskmasterd
    main.cpp
)

target_link_libraries(taskmasterd
    PRIVATE
        Qt::Core
)

install(TARGETS taskmasterd
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

cmake_minimum_required(VERSION 3.10)

project(logger_test)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_COMPILER g++)

include_directories(./inc)

aux_source_directory(./src SRC)

add_executable(loggertest main.cc ${SRC})

target_link_libraries(loggertest pthread)
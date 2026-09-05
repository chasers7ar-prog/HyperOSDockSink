cmake_minimum_required(VERSION 3.22.1)
project(dock_sink)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(dock_sink SHARED main.cpp)

find_library(log-lib log)
target_link_libraries(dock_sink ${log-lib})

target_compile_options(dock_sink PRIVATE -Wall -fvisibility=hidden)
target_link_options(dock_sink PRIVATE "-Wl,-z,max-page-size=16384")

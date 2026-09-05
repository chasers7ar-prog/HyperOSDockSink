cmake_minimum_required(VERSION 3.22.1)
project(dock_sink)

add_library(dock_sink SHARED main.cpp)

find_library(log-lib log)
target_link_libraries(dock_sink ${log-lib})

# Android 15+ 16KB page size
target_link_options(dock_sink PRIVATE "-Wl,-z,max-page-size=16384")

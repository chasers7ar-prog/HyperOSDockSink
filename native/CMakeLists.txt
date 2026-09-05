cmake_minimum_required(VERSION 3.22.1)
project(dock_sink)

add_library(dock_sink SHARED main.cpp)

find_library(log-lib log)
target_link_libraries(dock_sink ${log-lib})

# Android 15+ 16KB page size requirement
target_link_options(dock_sink PRIVATE "-Wl,-z,max-page-size=16384")

# Ensure we don't strip too aggressively during debug
set_target_properties(dock_sink PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

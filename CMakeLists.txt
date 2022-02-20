cmake_minimum_required(VERSION 3.9)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")

project(lpm-cli)

set(CMAKE_CXX_FLAGS "-std=c++2a")

include_directories(./src/library)

add_executable(lpm-cli src/main.cpp)
set_target_properties(lpm-cli PROPERTIES LINKER_LANGUAGE CXX OUTPUT_NAME lpm)

# Include the framework
add_subdirectory(src/library)

# Include source files
add_library(lpm-cli-src
    ./src/command.cpp
)

# Include command handlers
add_library(lpm-handlers
    ./src/handlers/install.cpp
)

target_link_libraries(lpm-cli PRIVATE lpm-lib lpm-cli-src lpm-handlers)
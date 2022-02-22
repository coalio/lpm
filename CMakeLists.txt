message(STATUS "Thanks for using lpm!\n")
cmake_minimum_required(VERSION 3.9)
set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "/usr/bin/g++")
set(CMAKE_CXX_FLAGS "-std=c++2a")

if (NOT BIN_INSTALL_DIR)
    set(BIN_INSTALL_DIR bin)
endif ()

project(lpm-cli)

include_directories(./src ./src/library)

# Specify executable and target properties
add_executable(lpm-cli src/main.cpp)
set_target_properties(
    lpm-cli
    PROPERTIES
    LINKER_LANGUAGE CXX
    OUTPUT_NAME lpm
)

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

# Add install target
install(TARGETS lpm-cli
    RUNTIME DESTINATION ${BIN_INSTALL_DIR}
)

# Link the libraries to the target
target_link_libraries(lpm-cli PRIVATE lpm-cli-src lpm-handlers lpm-lib)
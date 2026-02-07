# set minimum cmake version
cmake_minimum_required(VERSION 3.15)
project(web_server VERSION 1.0 LANGUAGES CXX)

# set compiler options
set(CMAKE_CXX_STANDARD 11)        
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)     # Enforce standard compliance

# Export compile commands for LSP
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Enable debugging symbols
# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_CXX_FLAGS_DEBUG "-g")

# add source files
set(SOURCES
    src/main.cpp
    src/handle_client.cpp
    src/network.cpp
    src/sig_utils.cpp
)

# create the executable
add_executable(web_server ${SOURCES})

# include directories
target_include_directories(web_server PRIVATE ${CMAKE_SOURCE_DIR}/include)

# had to add this so networking functions work
add_compile_definitions(_POSIX_C_SOURCE=200112L)

# optional: enable warnings and debugging flags
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(web_server PRIVATE -Wall -Wextra -Wpedantic)
endif()

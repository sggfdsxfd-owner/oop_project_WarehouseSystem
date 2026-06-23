cmake_minimum_required(VERSION 3.10)
project(WarehouseSystem)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Source files
file(GLOB SOURCES "src/*.cpp")

# Include directories
include_directories(src)

# Add executable
add_executable(WarehouseSystem ${SOURCES})

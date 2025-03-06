cmake_minimum_required(VERSION 3.10)
project(WirelessLocationProject)

# Set C++ standard (change if needed)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include the 'include' directory so we can find headers
include_directories(${PROJECT_SOURCE_DIR}/include)

# Gather source files from 'src'
file(GLOB SRC_FILES
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

# Create the executable
add_executable(wifiLocation ${SRC_FILES})

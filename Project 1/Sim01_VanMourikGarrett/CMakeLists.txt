cmake_minimum_required (VERSION 3.7.2)

project (Project_1)
# The version number.
set (Project_1_VERSION_MAJOR 0)
set (Project_1_VERSION_MINOR 1)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIB_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

set(CMAKE_CXX_STANDARD 11)  # enable C++11 standard

add_subdirectory(bin)
add_subdirectory(src)
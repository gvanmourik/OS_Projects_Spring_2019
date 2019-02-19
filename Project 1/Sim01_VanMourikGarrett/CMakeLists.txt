cmake_minimum_required (VERSION 3.9)

project (Project_1)
# The version number.
set (Project_1_VERSION_MAJOR 0)
set (Project_1_VERSION_MINOR 1)

# Set the output folder where your program will be created
set(CMAKE_BINARY_DIR ${CMAKE_SOURCE_DIR}/bin)
set(EXE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# set(LIB_OUTPUT_PATH ${CMAKE_BINARY_DIR})
# set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)

set(CMAKE_CXX_STANDARD 11)  # enable C++11 standard
# set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

add_subdirectory(bin)
add_subdirectory(src)
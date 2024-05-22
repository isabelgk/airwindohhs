cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
project(airwindohhs)

add_library(Airwindohhs INTERFACE)
target_include_directories(Airwindohhs INTERFACE include)

add_subdirectory(example)
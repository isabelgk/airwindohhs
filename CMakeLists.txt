cmake_minimum_required(VERSION 3.28)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

project(airwindohhs VERSION 0.1.0)

add_library(Airwindohhs INTERFACE)
add_library(Airwindohhs::Airwindohhs ALIAS Airwindohhs)
target_include_directories(Airwindohhs INTERFACE include)
target_compile_features(Airwindohhs INTERFACE cxx_std_17)

add_executable(dump_meta tools/dump_meta.cpp)
target_link_libraries(dump_meta PRIVATE Airwindohhs::Airwindohhs)

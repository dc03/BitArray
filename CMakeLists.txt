cmake_minimum_required(VERSION 3.10)

project(BitArray LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

add_executable(BitArrayTest src/main.cpp)
target_include_directories(BitArrayTest PUBLIC include/)

add_library(BitArray INTERFACE)
target_include_directories(BitArray INTERFACE include/)

cmake_minimum_required(VERSION 3.10)

project(BitArray LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

# Add the cmake folder so the FindSphinx module is found
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

add_executable(BitArrayTest src/main.cpp)
target_include_directories(BitArrayTest PUBLIC include/)

add_library(BitArray INTERFACE)
target_include_directories(BitArray INTERFACE include/)

add_subdirectory(doc)
cmake_minimum_required(VERSION 3.22)
project(Langulus_Core)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source SOURCE_FILES)
add_library(Langulus.Core INTERFACE)

target_include_directories(Langulus.Core INTERFACE include)

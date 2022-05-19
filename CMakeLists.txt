cmake_minimum_required(VERSION 3.22)
project(Langulus_Core)

# Check if this project is built as standalone, or a part of something else
if(PROJECT_IS_TOP_LEVEL)
    # We're building Core as standalone
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
endif()

# Build and install the Core
aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} SOURCE_FILES)
add_library(Langulus.Core INTERFACE)
target_include_directories(Langulus.Core INTERFACE include/)

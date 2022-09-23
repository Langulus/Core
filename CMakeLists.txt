cmake_minimum_required(VERSION 3.22)
project(Langulus_Core)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/source SOURCE_FILES)
add_library(Langulus.Core INTERFACE)

target_include_directories(Langulus.Core INTERFACE include)

STRING(REGEX MATCH "/Zc:trigraphs" result ${CMAKE_CXX_FLAGS})
IF(${result} MATCHES "/Zc:trigraphs")
    message(FATAL_ERROR "Langulus can't be built with trigraphs enabled")
ENDIF()

STRING(REGEX MATCH "-trigraphs" result ${CMAKE_CXX_FLAGS})
IF(${result} MATCHES "-trigraphs")
    message(FATAL_ERROR "Langulus can't be built with trigraphs enabled")
ENDIF()
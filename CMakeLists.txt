cmake_minimum_required(VERSION 3.24)
project(Langulus::Core)

add_library(LangulusCore INTERFACE)
target_compile_features(LangulusCore INTERFACE cxx_std_20)
target_include_directories(LangulusCore INTERFACE source)

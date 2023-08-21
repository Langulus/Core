cmake_minimum_required(VERSION 3.24)
project(LangulusCore
    VERSION         1.0.0
    DESCRIPTION     "Langulus core library"
    HOMEPAGE_URL    https://langulus.com
    LANGUAGES       CXX
)

add_library(LangulusCore INTERFACE)
target_compile_features(LangulusCore INTERFACE cxx_std_20)
target_include_directories(LangulusCore INTERFACE source)

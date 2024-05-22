cmake_minimum_required(VERSION 3.24)
project(LangulusCore
    VERSION         1.0.0
    DESCRIPTION     "Langulus core library"
    HOMEPAGE_URL    https://langulus.com
)

# Disable the FetchContent requests for langulus libraries from here on 
set(LANGULUS ON CACHE INTERNAL 
    "Defines the build tree as a part of Langulus,  \
    thus prevents the downloading of base Langulus libraries. \
    They should be in the build tree already" FORCE
)

add_library(LangulusCore INTERFACE)
target_compile_features(LangulusCore INTERFACE cxx_std_23)
target_include_directories(LangulusCore INTERFACE source)

message("Compiler: ${CMAKE_CXX_COMPILER_ID} - ${CMAKE_CXX_COMPILER_VERSION}")

if(CMAKE_CXX_FLAGS)
    # Make sure trigraphs are disabled                                  
    STRING(FIND ${CMAKE_CXX_FLAGS} "/Zc:trigraphs" TRIGRAPHS_ENABLED)
    if (${TRIGRAPHS_ENABLED} GREATER -1)
        message(FATAL_ERROR "Langulus can't be built with trigraphs enabled")
    endif()

    STRING(FIND ${CMAKE_CXX_FLAGS} "-trigraphs" TRIGRAPHS_ENABLED)
    if (${TRIGRAPHS_ENABLED} GREATER -1)
        message(FATAL_ERROR "Langulus can't be built with trigraphs enabled")
    endif()
endif()

if(MSVC)
    target_compile_options(LangulusCore INTERFACE /utf-8 /W3 /bigobj /EHsc /wd4180)
else()
    target_compile_options(LangulusCore INTERFACE -Wall -Wextra -Wno-comment
	                                              -fvisibility=hidden
												  -fvisibility-inlines-hidden)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    target_compile_options(LangulusCore INTERFACE -Wno-trigraphs)
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(LangulusCore INTERFACE -fmax-errors=5)
endif()

# Configure the features you require                                    
option(LANGULUS_SHARED_LIBRARIES 
    "Will build shared libraries (DLL/SO) if enabled, \
    true by default" ON)

if(LANGULUS_SHARED_LIBRARIES)
    set(LANGULUS_LIBRARY_TYPE SHARED CACHE INTERNAL "" FORCE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE INTERNAL "" FORCE)
else()
    set(LANGULUS_LIBRARY_TYPE STATIC CACHE INTERNAL "" FORCE)
endif()

option(LANGULUS_SAFE_MODE 
    "Overrides additional error checking and sanity checks, \
    incurs a serious runtime overhead, disabled by default" OFF)

option(LANGULUS_PARANOIA 
    "Make sure that any unused memory is nullified, \
    incurs a serious runtime overhead, disabled by default" OFF)

option(LANGULUS_TESTING 
    "Builds tests, disabled by default - Langulus tests are very exhaustive \
	and might increase your build time and RAM usage significantly" OFF)
    
option(LANGULUS_BENCHMARK
    "Enable benchmarking in tests, disabled by default" OFF)

option(LANGULUS_DEBUGGING 
    "Explicitly define debug mode, in the case that default flags are \
    not detected by some odd reason, disabled by default" OFF)

option(LANGULUS_FEATURE_NEWDELETE 
    "Overrides new/delete operators for anything statically linked to this library, \
    or provides LANGULUS_MONOPOLIZE_MEMORY() macro for you to use to override them, if dynamically linked. \
    This feature can be enabled only if LANGULUS_FEATURE_MANAGED_MEMORY is enabled" OFF)

option(LANGULUS_FEATURE_UNICODE
    "Provides UTF8, UTF16, UTF32 tools and conversions inside Text containers (WIP)" OFF)

option(LANGULUS_FEATURE_COMPRESSION
    "Links to zstd library and provides memory compression for use with containers (WIP)" OFF)

option(LANGULUS_FEATURE_ENCRYPTION
    "Allows for memory encryption for use with any container of your liking (WIP)" OFF)

option(LANGULUS_FEATURE_MANAGED_MEMORY
    "Memory will be pooled and recycled when freed, and you can safely push any kind of pointer, \
    as long as it was allocated by the memory manager, or by the overridden new/delete feature. Enabled by default" ON)

option(LANGULUS_FEATURE_MANAGED_REFLECTION
    "Reflections will be kept in a centralized location, when reflected, which speeds up \
    type comparisons, and allows you to dynamically modify the reflection at runtime. \
    This feature can be enabled only if LANGULUS_FEATURE_MANAGED_MEMORY is enabled. Enabled by default" ON)

option(LANGULUS_FEATURE_MEMORY_STATISTICS
    "Memory manager shall keep track of statistics, for the price of little overhead. Disabled by default" ON)

set(LANGULUS_ALIGNMENT 16 CACHE STRING "Default langulus alignment")
message(STATUS "[FEATURE] Alignment was set to ${LANGULUS_ALIGNMENT}")
target_compile_definitions(LangulusCore INTERFACE LANGULUS_ALIGNMENT=${LANGULUS_ALIGNMENT})

if(LANGULUS_DEBUGGING OR CMAKE_BUILD_TYPE MATCHES "Debug")
	message(STATUS "[FEATURE] Debug mode, all assertions are enabled")
    set(LANGULUS_SAFE_MODE ON CACHE BOOL "Debug mode detected" FORCE)
    set(LANGULUS_DEBUGGING ON CACHE BOOL "Debug mode detected" FORCE)
    set(LANGULUS_FEATURE_MEMORY_STATISTICS ON CACHE BOOL "Debug mode detected" FORCE)
	target_compile_definitions(LangulusCore INTERFACE LANGULUS_ASSERTION_LEVEL=1000)
else()
	message(STATUS "[FEATURE] Release mode, all assumption checks are disabled, optimization is enabled")
    if(MSVC)
        target_compile_options(LangulusCore INTERFACE /O2)
    else()
        target_compile_options(LangulusCore INTERFACE -O2)
    endif()
endif()

if(LANGULUS_TESTING)
	enable_testing()
    include(LangulusUtilities.cmake)
    fetch_external_module(
        Catch2
        GIT_REPOSITORY  https://github.com/catchorg/Catch2.git
        GIT_TAG         d4b0b34 #v2.x
    )
endif()

macro(reflect_option OPTION MESSAGE)
	if (${OPTION})
		message(STATUS "[FEATURE] ${MESSAGE} enabled (${OPTION})")
		target_compile_definitions(LangulusCore INTERFACE "$<$<BOOL:${OPTION}>:${OPTION}>")
	endif()
endmacro()

reflect_option(LANGULUS_SAFE_MODE "Safe mode")
reflect_option(LANGULUS_PARANOIA "Paranoid mode")
reflect_option(LANGULUS_TESTING "Tests")
reflect_option(LANGULUS_BENCHMARK "Benchmarking")
reflect_option(LANGULUS_FEATURE_NEWDELETE "New/Delete operator override")
reflect_option(LANGULUS_FEATURE_UNICODE "Unicode text containers")
reflect_option(LANGULUS_FEATURE_COMPRESSION "Compressable memory")
reflect_option(LANGULUS_FEATURE_ENCRYPTION "Encryptable memory")
reflect_option(LANGULUS_FEATURE_MANAGED_MEMORY "Managed memory")
reflect_option(LANGULUS_FEATURE_MANAGED_REFLECTION "Managed reflection")
reflect_option(LANGULUS_FEATURE_MEMORY_STATISTICS "Memory statistics")
reflect_option(LANGULUS_SHARED_LIBRARIES "Building shared libraries")

option(LANGULUS_LOGGER_ENABLE_FATALERRORS
    "Enables fatal error logging via Logger::Fatal()" ON)
option(LANGULUS_LOGGER_ENABLE_ERRORS
    "Enables error logging via Logger::Error()" ON)
option(LANGULUS_LOGGER_ENABLE_WARNINGS
    "Enables warning logging via Logger::Warning()" ON)
option(LANGULUS_LOGGER_ENABLE_INFOS
    "Enables information logging via Logger::Info()" ON)
option(LANGULUS_LOGGER_ENABLE_MESSAGES
    "Enables message logging via Logger::Message()" ON)
option(LANGULUS_LOGGER_ENABLE_PROMPTS
    "Enables prompts logging via Logger::Prompt(), as well as the possibility for console input" ON)
option(LANGULUS_LOGGER_ENABLE_VERBOSE
    "Enables verbose logging via Logger::Verbose()" ${LANGULUS_DEBUGGING})
option(LANGULUS_LOGGER_ENABLE_SPECIALS
    "Enables special message logging via Logger::Special()" ${LANGULUS_DEBUGGING})
option(LANGULUS_LOGGER_ENABLE_FLOWS
    "Enables flow logging via Logger::Flow()" ${LANGULUS_DEBUGGING})
option(LANGULUS_LOGGER_ENABLE_INPUTS
    "Enables input logging via Logger::Input()" ${LANGULUS_DEBUGGING})
option(LANGULUS_LOGGER_ENABLE_NETWORKS
    "Enables network logging via Logger::Network()" ${LANGULUS_DEBUGGING})
option(LANGULUS_LOGGER_ENABLE_OS
    "Enables OS logging via Logger::OS()" ${LANGULUS_DEBUGGING})

reflect_option(LANGULUS_LOGGER_ENABLE_FATALERRORS "Logger: fatal errors")
reflect_option(LANGULUS_LOGGER_ENABLE_ERRORS "Logger: regular errors")
reflect_option(LANGULUS_LOGGER_ENABLE_WARNINGS "Logger: warnings")
reflect_option(LANGULUS_LOGGER_ENABLE_INFOS "Logger: information messages")
reflect_option(LANGULUS_LOGGER_ENABLE_MESSAGES "Logger: user messages")
reflect_option(LANGULUS_LOGGER_ENABLE_PROMPTS "Logger: prompts and console input")
reflect_option(LANGULUS_LOGGER_ENABLE_VERBOSE "Logger: verbose messages")
reflect_option(LANGULUS_LOGGER_ENABLE_SPECIALS "Logger: special messages")
reflect_option(LANGULUS_LOGGER_ENABLE_FLOWS "Logger: flow messages")
reflect_option(LANGULUS_LOGGER_ENABLE_INPUTS "Logger: input event messages")
reflect_option(LANGULUS_LOGGER_ENABLE_NETWORKS "Logger: network messages")
reflect_option(LANGULUS_LOGGER_ENABLE_OS "Logger: operating system messages")
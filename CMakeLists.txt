cmake_minimum_required(VERSION 3.28)
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
    "Will build shared libraries (DLL/SO/WASM) if enabled, \
    true by default" ON)

if(LANGULUS_SHARED_LIBRARIES)
	set(LANGULUS_LIBRARY_TYPE SHARED CACHE INTERNAL "" FORCE)
    set(CMAKE_POSITION_INDEPENDENT_CODE ON CACHE INTERNAL "" FORCE)
	message("Building Langulus as shared library")
else()
    set(LANGULUS_LIBRARY_TYPE STATIC CACHE INTERNAL "" FORCE)
	message("Building Langulus as static library")
endif()

# Create a library dependent on build configuration						
function(add_langulus_library NAME)
	if (EMSCRIPTEN AND LANGULUS_SHARED_LIBRARIES)
		# When building for emscripten, we "fake" a shared library by	
		# creating an executable with exports and no entry point		
		# This "fake" shared library has to be linked in a specific way	
		# only from wasm MAIN_MODULEs, so that both LangulusCore		
		# interface is inherited, and the shared library is loaded in	
		# at startup. Here's an example:								
		#	target_link_libraries(LangulusLoggerTest PRIVATE			
		#		$<TARGET_FILE:LangulusLogger> LangulusLogger			
		#		^ 							  ^							
		#		+- Links the *.wasm file      +- Inherits interface		
		# https://github.com/emscripten-core/emscripten/issues/17804	
		add_executable(${NAME} ${ARGN})
		set_property(TARGET ${NAME} APPEND PROPERTY
			COMPILE_FLAGS "-sSIDE_MODULE --no-entry -fPIC")
		set_property(TARGET ${NAME} APPEND PROPERTY
			LINK_FLAGS "-sSIDE_MODULE -sWASM=1 --no-entry -fPIC")
		set_target_properties(${NAME} PROPERTIES
			ENABLE_EXPORTS ON
			SUFFIX ".wasm"
		)
	else()
		add_library(${NAME} ${LANGULUS_LIBRARY_TYPE} ${ARGN})
	endif()
endfunction()

# Create a shared module library										
function(add_langulus_mod NAME)
	if (EMSCRIPTEN)
		# When building for emscripten, we "fake" a shared library by	
		# creating an executable with exports and no entry point		
		# This "fake" shared library has to be linked in a specific way	
		# only from wasm MAIN_MODULEs, so that both LangulusCore		
		# interface is inherited, and the shared library is loaded in	
		# at startup (mainly used in mod tests). You don't have to link 
		# a mod at all, if you plan on dlopen-ing it at runtime. 		
		# All Langulus symbols will be dangling until you do that,		
		# hence the additional -sERROR_ON_UNDEFINED_SYMBOLS=0			
		add_executable(${NAME} ${ARGN})
		set_property(TARGET ${NAME} APPEND PROPERTY
			COMPILE_FLAGS "-sSIDE_MODULE --no-entry -fPIC")
		set_property(TARGET ${NAME} APPEND PROPERTY
			LINK_FLAGS "-sSIDE_MODULE -sWASM=1 --no-entry -fPIC -sERROR_ON_UNDEFINED_SYMBOLS=0")
		set_target_properties(${NAME} PROPERTIES
			ENABLE_EXPORTS ON
			SUFFIX ".so"
		)
	else()
		add_library(${NAME} SHARED ${ARGN})
	endif()

	# Lack of $<TARGET_FILE:Langulus> is INTENTIONAL					
	# - we wouldn't want a mod to load any base framework symbols		
	target_link_libraries(${NAME} PRIVATE Langulus)
endfunction()

# Create an executable													
function(add_langulus_app NAME)
    set(multiValueArgs SOURCES LIBRARIES DEPENDENCIES EMSCRIPTEN_COMPILE_FLAGS EMSCRIPTEN_LINK_FLAGS)
    cmake_parse_arguments(PARSE_ARGV 0 arg "" "" "${multiValueArgs}")
	add_executable(${NAME} ${arg_SOURCES})

	if (EMSCRIPTEN)
		# Pack all dependencies into a *.data file						
		foreach(ITEM ${arg_DEPENDENCIES})
			string(APPEND arg_EMSCRIPTEN_LINK_FLAGS
				" --preload-file ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${ITEM}.so@${ITEM}.so")
		endforeach()

		# attention: -fwasm-exception are currently supported in several
		# major web browsers, but may not be supported in all			
		# WebAssembly engines yet. Also -sASYNCIFY isn't compatible!	
		set_target_properties(${NAME} PROPERTIES
			COMPILE_FLAGS  "-sMAIN_MODULE -fwasm-exceptions ${arg_EMSCRIPTEN_COMPILE_FLAGS}"
			LINK_FLAGS     "-sMAIN_MODULE -sWASM=1 --emrun -sALLOW_MEMORY_GROWTH -fwasm-exceptions ${arg_EMSCRIPTEN_LINK_FLAGS}"
			SUFFIX         ".html"
		)

		# When building for emscripten, our shared libraries are "fake"	
		# and have to be linked in a specific way from wasm MAIN_MODULEs
		# - once by using the *.wasm file, and once by using the shared	
		# library target.												
		# Any *.so files on the other hand must be packed in a *.data	
		# file by using --preload-file with all the required mods, like	
		# so: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/LangulusMod*.so			
		foreach(ITEM ${arg_LIBRARIES})
			target_link_libraries(${NAME}
				PRIVATE	$<TARGET_FILE:${ITEM}>
				        ${ITEM}
			)
		endforeach()
	else()
		target_link_libraries(${NAME}
			PRIVATE	${arg_LIBRARIES})
	endif()

	foreach(ITEM ${arg_DEPENDENCIES})
		add_dependencies(${NAME} ${ITEM})
	endforeach()
endfunction()

# Create a test executable												
function(add_langulus_test NAME)
	add_langulus_app(${NAME} ${ARGN})
	target_link_libraries(${NAME} PRIVATE Catch2)
	add_test(NAME ${NAME} COMMAND ${NAME})
endfunction()

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
///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once

/// Sorry, Langulus is designed for at least C++20, will move to C++23        
/// when full compiler support is provided                                    
#if __cplusplus <= 201703L and not defined(_MSC_VER)
   #error Langulus requires at least a C++20 compliant compiler in order to build
#endif

#include <cstdint>
#include <source_location>

/// Safe mode enables assumption checks all over the code                     
/// High overhead, usually enabled only when testing in debug builds          
#if defined(LANGULUS_SAFE_MODE) or defined(LANGULUS_ASSERTION_LEVEL)
   #ifdef LANGULUS_ASSERTION_LEVEL
      #define LANGULUS_SAFE() LANGULUS_ASSERTION_LEVEL
   #else
      #define LANGULUS_SAFE() 1
   #endif
#else
   #define LANGULUS_SAFE() 0
#endif

#if LANGULUS_SAFE()
   #define IF_SAFE(a) a
   #define IF_UNSAFE(a)
#else
   #define IF_SAFE(a)
   #define IF_UNSAFE(a) a
#endif

/// Testing mode exposes some otherwise private functions                     
/// Overhead is unlikely                                                      
#ifdef LANGULUS_TESTING
   #undef LANGULUS_TESTING
   #define LANGULUS_TESTING() 1
   #define IF_LANGULUS_TESTING(a) a
   #define IF_NOT_LANGULUS_TESTING(a)
#else
   #define LANGULUS_TESTING() 0
   #define IF_LANGULUS_TESTING(a)
   #define IF_NOT_LANGULUS_TESTING(a) a
#endif

/// Benchmarking                                                              
/// Tests will become radically slower                                        
#ifdef LANGULUS_BENCHMARK
   #undef LANGULUS_BENCHMARK
   #define LANGULUS_BENCHMARK() 1
   #define IF_LANGULUS_BENCHMARK(a) a
   #define IF_NOT_LANGULUS_BENCHMARK(a)
#else
   #define LANGULUS_BENCHMARK() 0
   #define IF_LANGULUS_BENCHMARK(a)
   #define IF_NOT_LANGULUS_BENCHMARK(a) a
#endif

/// Paranoid mode introduces overhead, but zeroes any freed memory            
#ifdef LANGULUS_PARANOIA
   #undef LANGULUS_PARANOIA
   #define LANGULUS_PARANOID() 1
   #define IF_LANGULUS_PARANOID(a) a
   #define IF_NOT_LANGULUS_PARANOID(a)
#else
   #define LANGULUS_PARANOID() 0
   #define IF_LANGULUS_PARANOID(a)
   #define IF_NOT_LANGULUS_PARANOID(a) a
#endif

/// Detect debug builds                                                       
#if defined(LANGULUS_DEBUGGING) or (not defined(NDEBUG) \
                                or defined(DEBUG) \
                                or defined(_DEBUG) \
                                or defined(CB_DEBUG) \
                                or defined(QT_QML_DEBUG))
   #undef LANGULUS_DEBUGGING
   #define LANGULUS_DEBUG() 1
   #define DEBUGGERY(a) a
#else
   #define LANGULUS_DEBUG() 0
   #define DEBUGGERY(a)
#endif

/// Reflections will be registered in a centralized location, allowing for    
/// runtime type modification. Meta primitives will always be in the same     
/// place in memory regardless of translation unit, which significantly       
/// speeds up meta definition comparisons.                                    
/// Naming collisions will be detected upon type registration                 
/// Gives a significant overhead on program launch, no dependencies           
#ifdef LANGULUS_FEATURE_MANAGED_REFLECTION
   #undef LANGULUS_FEATURE_MANAGED_REFLECTION
   #define LANGULUS_FEATURE_MANAGED_REFLECTION() 1
   #define IF_LANGULUS_MANAGED_REFLECTION(a) a
   #define IF_NOT_LANGULUS_MANAGED_REFLECTION(a)
#else
   #define LANGULUS_FEATURE_MANAGED_REFLECTION() 0
   #define IF_LANGULUS_MANAGED_REFLECTION(a)
   #define IF_NOT_LANGULUS_MANAGED_REFLECTION(a) a
#endif

/// Memory allocations will be pooled, authority will be tracked,             
/// memory will be reused whenever possible, and you can also tweak           
/// runtime allocation strategies on per-type basis                           
/// Significantly improves performance, no dependencies                       
#ifdef LANGULUS_FEATURE_MANAGED_MEMORY
   #undef LANGULUS_FEATURE_MANAGED_MEMORY
   #define LANGULUS_FEATURE_MANAGED_MEMORY() 1
   #define IF_LANGULUS_MANAGED_MEMORY(a) a
   #define IF_NOT_LANGULUS_MANAGED_MEMORY(a)
#else
   #define LANGULUS_FEATURE_MANAGED_MEMORY() 0
   #define IF_LANGULUS_MANAGED_MEMORY(a)
   #define IF_NOT_LANGULUS_MANAGED_MEMORY(a) a
#endif

/// Memory manager shall keep track of statistics                             
/// Some overhead upon allocation/deallocation/reallocation                   
/// Some methods, like string null-termination will pick more memory-         
/// consitent, but less performant approaches (see Text::Terminate())         
#ifdef LANGULUS_FEATURE_MEMORY_STATISTICS
   #undef LANGULUS_FEATURE_MEMORY_STATISTICS
   #define LANGULUS_FEATURE_MEMORY_STATISTICS() 1
   #define IF_LANGULUS_MEMORY_STATISTICS(a) a
   #define IF_NOT_LANGULUS_MEMORY_STATISTICS(a)
#else
   #define LANGULUS_FEATURE_MEMORY_STATISTICS() 0
   #define IF_LANGULUS_MEMORY_STATISTICS(a)
   #define IF_NOT_LANGULUS_MEMORY_STATISTICS(a) a
#endif

/// Replace the default new-delete operators with custom ones                 
/// No overhead, no dependencies                                              
#ifdef LANGULUS_FEATURE_NEWDELETE
   #undef LANGULUS_FEATURE_NEWDELETE
   #define LANGULUS_FEATURE_NEWDELETE() 1
   #define IF_LANGULUS_NEWDELETE(a) a
   #define IF_NOT_LANGULUS_NEWDELETE(a)
#else
   #define LANGULUS_FEATURE_NEWDELETE() 0
   #define IF_LANGULUS_NEWDELETE(a) 
   #define IF_NOT_LANGULUS_NEWDELETE(a) a
#endif

/// Enables utf support and utilities for Text container                      
/// No runtime overhead                                                       
#ifdef LANGULUS_FEATURE_UNICODE
   #undef LANGULUS_FEATURE_UNICODE
   #define LANGULUS_FEATURE_UNICODE() 1
   #define IF_LANGULUS_UNICODE(a) a
   #define IF_NOT_LANGULUS_UNICODE(a)
#else
   #define LANGULUS_FEATURE_UNICODE() 0
   #define IF_LANGULUS_UNICODE(a) 
   #define IF_NOT_LANGULUS_UNICODE(a) a
#endif

/// Enable memory compression utilities for containers                        
/// Gives a bit of general runtime overhead, zstd will be linked              
#ifdef LANGULUS_FEATURE_COMPRESSION
   #undef LANGULUS_FEATURE_COMPRESSION
   #define LANGULUS_FEATURE_COMPRESSION() 1
   #define IF_LANGULUS_COMPRESSION(a) a
   #define IF_NOT_LANGULUS_COMPRESSION(a)
#else
   #define LANGULUS_FEATURE_COMPRESSION() 0
   #define IF_LANGULUS_COMPRESSION(a) 
   #define IF_NOT_LANGULUS_COMPRESSION(a) a
#endif

/// Enable memory encryption and decryption                                   
/// Gives a tiny runtime overhead, no dependencies                            
#ifdef LANGULUS_FEATURE_ENCRYPTION
   #undef LANGULUS_FEATURE_ENCRYPTION
   #define LANGULUS_FEATURE_ENCRYPTION() 1
   #define IF_LANGULUS_ENCRYPTION(a) a
   #define IF_NOT_LANGULUS_ENCRYPTION(a)
#else
   #define LANGULUS_FEATURE_ENCRYPTION() 0
   #define IF_LANGULUS_ENCRYPTION(a) 
   #define IF_NOT_LANGULUS_ENCRYPTION(a) a
#endif

/// Detect architecture                                                       
#if INTPTR_MAX == INT64_MAX
   #define LANGULUS_BITNESS() 64
#elif INTPTR_MAX == INT32_MAX
   #define LANGULUS_BITNESS() 32
#else
   #error Unknown pointer size
#endif

/// Detect compiler                                                           
#if defined(__GNUC__) and not defined(__clang__)
   // We're on a GNUC Compiler!                                         
   #define LANGULUS_COMPILER_GCC() 1
#else
   #define LANGULUS_COMPILER_GCC() 0
#endif

#if defined(_MSC_VER)
   // We're on a microsoft visual c++ compiler!                         
   #define LANGULUS_COMPILER_MSVC() 1
#else
   #define LANGULUS_COMPILER_MSVC() 0
#endif

#if defined(__clang__)
   // We're on a clang compiler!                                        
   #define LANGULUS_COMPILER_CLANG() 1
#else
   #define LANGULUS_COMPILER_CLANG() 0
#endif

#if defined(__MINGW32__) or defined(__MINGW64__) 
   // We're on a mingw compiler!                                        
   #define LANGULUS_COMPILER_MINGW() 1
#else
   #define LANGULUS_COMPILER_MINGW() 0
#endif

/// Checks if a given compiler is enabled                                     
#define LANGULUS_COMPILER(a) LANGULUS_COMPILER_##a()

/// Shamelessly stolen from boost and extended to my liking                   
/// Dumps the current function name                                           
#define LANGULUS_FUNCTION() std::source_location::current().function_name()

/// Utility macro, that turns its argument to a string literal (inner)        
#define LANGULUS_STRINGIFY_INNER(x)		#x

/// Utility macro, that turns its argument to a string literal                
#define LANGULUS_STRINGIFY(x)				LANGULUS_STRINGIFY_INNER(x)

/// Macro that generates a literal with the function name, file, and line     
#define LANGULUS_LOCATION() __FILE__ ":" LANGULUS_STRINGIFY(__LINE__)

#define LANGULUS_OS(a) LANGULUS_OS_##a()

#if defined(_WIN32) or defined(__CYGWIN__)
   #define LANGULUS_OS_WINDOWS() 1
#else 
   #define LANGULUS_OS_WINDOWS() 0
#endif

#if defined(__linux__)
   #define LANGULUS_OS_LINUX() 1
#else 
   #define LANGULUS_OS_LINUX() 0
#endif

#if defined(__ANDROID__)
   #define LANGULUS_OS_ANDROID() 1
#else 
   #define LANGULUS_OS_ANDROID() 0
#endif

#if defined(__APPLE__)
   #define LANGULUS_OS_MACOS() 1
#else 
   #define LANGULUS_OS_MACOS() 0
#endif

#if defined(__unix__)
   #define LANGULUS_OS_UNIX() 1
#else 
   #define LANGULUS_OS_UNIX() 0
#endif

#if defined(__FreeBSD__)
   #define LANGULUS_OS_FREEBSD() 1
#else 
   #define LANGULUS_OS_FREEBSD() 0
#endif

/// Shared object export/import attributes                                    
#ifdef LANGULUS_SHARED_LIBRARIES
   #if LANGULUS_COMPILER(GCC) or LANGULUS_COMPILER(CLANG)
      #if LANGULUS_OS(WINDOWS)
	      #define LANGULUS_EXPORT() __attribute__ ((dllexport))
	      #define LANGULUS_IMPORT() __attribute__ ((dllimport))
      #elif LANGULUS_OS(LINUX)
	      #define LANGULUS_EXPORT() __attribute__ ((visibility("default")))
	      #define LANGULUS_IMPORT() // requires -fvisibility=hidden      
      #else
         #error Not implemented
      #endif
   #elif LANGULUS_COMPILER(MSVC) or LANGULUS_COMPILER(MINGW)
	   #define LANGULUS_EXPORT() __declspec(dllexport)
	   #define LANGULUS_IMPORT() __declspec(dllimport)
   #else 
      #error Not implemented
   #endif
#else
   /// Shared library exports are disabled                                    
   #define LANGULUS_EXPORT()
   #define LANGULUS_IMPORT()
#endif

/// Useful for globally exporting everything, when building the framework     
#ifdef LANGULUS_EXPORT_ALL
   #define LANGULUS_API_ALL() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_ALL() LANGULUS_IMPORT()
#endif

/// Used to define imports/exports per module                                 
#define LANGULUS_API(a) LANGULUS_API_##a()

/// Make the rest of the code aware, that Langulus::Core has been included    
#define LANGULUS_LIBRARY_CORE() 1

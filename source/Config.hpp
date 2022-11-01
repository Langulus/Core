///                                                                           
/// Langulus::Core                                                            
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include <cstdint>

/// Sorry, Langulus is designed for at least C++20                            
#if __cplusplus <= 201703L && !defined(_MSC_VER)
   #error Langulus requires at least a C++20 compliant compiler in order to build
#endif

/// Safe mode enables assumption checks all over the code                     
/// High overhead, usually enabled only when testing in debug builds          
#if defined(LANGULUS_ENABLE_SAFE_MODE) || defined(LANGULUS_ENABLE_ASSERTION_LEVEL)
   #ifdef LANGULUS_ENABLE_ASSERTION_LEVEL
      #define LANGULUS_SAFE() LANGULUS_ENABLE_ASSERTION_LEVEL
   #else
      #define LANGULUS_SAFE() 1
   #endif
#else
   #define LANGULUS_SAFE() 0
#endif

#if LANGULUS_SAFE()
   #define SAFETY(a) a
   // Some functions throw only in safe mode                            
   #define SAFETY_NOEXCEPT()
#else
   #define SAFETY(a)
   // Some functions throw only in safe mode                            
   #define SAFETY_NOEXCEPT() noexcept
#endif

/// Testing mode exposes some otherwise private functions                     
/// Overhead is unlikely                                                      
#ifdef LANGULUS_ENABLE_TESTING
   #define LANGULUS_TESTING() 1
#else
   #define LANGULUS_TESTING() 0
#endif

#if LANGULUS_TESTING()
   #define TESTING(a) a
#else
   #define TESTING(a)
#endif

/// Paranoid mode introduces overhead, but zeroes any freed memory            
#ifdef LANGULUS_ENABLE_PARANOIA
   #define LANGULUS_PARANOID() 1
#else
   #define LANGULUS_PARANOID() 0
#endif

#if LANGULUS_PARANOID()
   #define PARANOIA(a) a
#else
   #define PARANOIA(a)
#endif

/// Detect debug builds                                                       
#if defined(DEBUG) || !defined(NDEBUG) || defined(_DEBUG) || defined(CB_DEBUG) || defined(QT_QML_DEBUG) || defined(LANGULUS_ENABLE_DEBUGGING)
   #define LANGULUS_DEBUG() 1
   #define DEBUGGERY(a) a
#else
   #define LANGULUS_DEBUG() 0
   #define DEBUGGERY(a)
#endif

#if LANGULUS_DEBUG()
   #define DEBUGGERY(a) a
#else
   #define DEBUGGERY(a)
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
#define LANGULUS_COMPILER_GCC() 0
#define LANGULUS_COMPILER_MSVC() 0
#define LANGULUS_COMPILER_CLANG() 0
#define LANGULUS_COMPILER_MINGW() 0

#if defined(__GNUC__) && !defined(__clang__)
   // We're on a GNUC Compiler!                                         
   #undef LANGULUS_COMPILER_GCC
   #define LANGULUS_COMPILER_GCC() 1
#elif defined(_MSC_VER)
   // We're on a microsoft visual c++ compiler!                         
   #undef LANGULUS_COMPILER_MSVC
   #define LANGULUS_COMPILER_MSVC() 1
#elif defined(__clang__)
   // We're on a clang compiler!                                        
   #undef LANGULUS_COMPILER_CLANG
   #define LANGULUS_COMPILER_CLANG() 1
#elif defined(__MINGW32__) || defined(__MINGW64__) 
   // We're on a mingw compiler!                                        
   #undef LANGULUS_COMPILER_MINGW
   #define LANGULUS_COMPILER_MINGW() 1
#else
   #error Unrecognized compiler
#endif

/// Checks if a given compiler is enabled                                     
#define LANGULUS_COMPILER(a) LANGULUS_COMPILER_##a()

/// Shamelessly stolen from boost and extended to my liking                   
/// Dumps the current function name                                           
#if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
   #define LANGULUS_FUNCTION() __PRETTY_FUNCTION__
#elif defined(__clang__)
   #define LANGULUS_FUNCTION() __PRETTY_FUNCTION__
#elif defined(__DMC__) && (__DMC__ >= 0x810)
   #define LANGULUS_FUNCTION() __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__) || defined(_MSC_VER)
   #define LANGULUS_FUNCTION() __FUNCSIG__
#elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
   #define LANGULUS_FUNCTION() __FUNCTION__
#elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
   #define LANGULUS_FUNCTION() __FUNC__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
   #define LANGULUS_FUNCTION() __func__
#elif defined(__cplusplus) && (__cplusplus >= 201103)
   #define LANGULUS_FUNCTION() __func__
#else
   #error Not implemented
#endif

/// Utility macro, that turns its argument to a string literal (inner)        
#define LANGULUS_STRINGIFY_INNER(x)		#x

/// Utility macro, that turns its argument to a string literal                
#define LANGULUS_STRINGIFY(x)				LANGULUS_STRINGIFY_INNER(x)

/// Macro that generates a literal with the function name, file, and line     
#define LANGULUS_LOCATION() __FILE__ ":" LANGULUS_STRINGIFY(__LINE__)

#define LANGULUS_OS(a) LANGULUS_OS_##a()

#if defined(_WIN32) || defined(__CYGWIN__)
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
#ifdef LANGULUS_BUILD_SHARED_LIBRARIES
   #if LANGULUS_COMPILER(GCC) || LANGULUS_COMPILER(CLANG)
      #if LANGULUS_OS(WINDOWS)
	      #define LANGULUS_EXPORT() __attribute__ ((dllexport))
	      #define LANGULUS_IMPORT() __attribute__ ((dllimport))
      #elif LANGULUS_OS(LINUX)
	      #define LANGULUS_EXPORT() __attribute__ ((visibility("default")))
	      #define LANGULUS_IMPORT() // requires -fvisibility=hidden      
      #else
         #error Not implemented
      #endif
   #elif LANGULUS_COMPILER(MSVC) || LANGULUS_COMPILER(MINGW)
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
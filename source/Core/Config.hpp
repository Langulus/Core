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
#if defined(LANGULUS_ENABLE_SAFE_MODE) or defined(LANGULUS_ENABLE_ASSERTION_LEVEL)
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
   // Some functions are constexpr only when safe mode is disabled      
   #define SAFETY_CONSTEXPR()
#else
   #define SAFETY(a)
   // Some functions throw only in safe mode                            
   #define SAFETY_NOEXCEPT() noexcept
   // Some functions are constexpr only when safe mode is disabled      
   #define SAFETY_CONSTEXPR() constexpr
#endif

/// Testing mode exposes some otherwise private functions                     
/// Overhead is unlikely                                                      
#ifdef LANGULUS_ENABLE_TESTING
   #define LANGULUS_TESTING() 1
   #define IF_LANGULUS_TESTING(a) a
   #define IF_NOT_LANGULUS_TESTING(a)
#else
   #define LANGULUS_TESTING() 0
   #define IF_LANGULUS_TESTING(a)
   #define IF_NOT_LANGULUS_TESTING(a) a
#endif

/// Paranoid mode introduces overhead, but zeroes any freed memory            
#ifdef LANGULUS_ENABLE_PARANOIA
   #define LANGULUS_PARANOID() 1
   #define IF_LANGULUS_PARANOID(a) a
   #define IF_NOT_LANGULUS_PARANOID(a)
#else
   #define LANGULUS_PARANOID() 0
   #define IF_LANGULUS_PARANOID(a)
   #define IF_NOT_LANGULUS_PARANOID(a) a
#endif

/// Detect debug builds                                                       
#if defined(LANGULUS_ENABLE_DEBUGGING) or defined(DEBUG) \
                                       or defined(_DEBUG) \
                                       or defined(CB_DEBUG) \
                                       or defined(QT_QML_DEBUG)
   #define LANGULUS_DEBUG() 1
   #define DEBUGGERY(a) a
#else
   #define LANGULUS_DEBUG() 0
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
#if defined(__GNUC__) && !defined(__clang__)
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

#if defined(__MINGW32__) || defined(__MINGW64__) 
   // We're on a mingw compiler!                                        
   #define LANGULUS_COMPILER_MINGW() 1
#else
   #define LANGULUS_COMPILER_MINGW() 0
#endif

/// Checks if a given compiler is enabled                                     
#define LANGULUS_COMPILER(a) LANGULUS_COMPILER_##a()

/// Shamelessly stolen from boost and extended to my liking                   
/// Dumps the current function name                                           
#if defined(__GNUC__) or (defined(__MWERKS__) and (__MWERKS__ >= 0x3000)) or (defined(__ICC) and (__ICC >= 600)) or defined(__ghs__)
   #define LANGULUS_FUNCTION() __PRETTY_FUNCTION__
#elif defined(__clang__)
   #define LANGULUS_FUNCTION() __PRETTY_FUNCTION__
#elif defined(__DMC__) and (__DMC__ >= 0x810)
   #define LANGULUS_FUNCTION() __PRETTY_FUNCTION__
#elif defined(__FUNCSIG__) or defined(_MSC_VER)
   #define LANGULUS_FUNCTION() __FUNCSIG__
#elif (defined(__INTEL_COMPILER) and (__INTEL_COMPILER >= 600)) or (defined(__IBMCPP__) and (__IBMCPP__ >= 500))
   #define LANGULUS_FUNCTION() __FUNCTION__
#elif defined(__BORLANDC__) and (__BORLANDC__ >= 0x550)
   #define LANGULUS_FUNCTION() __FUNC__
#elif defined(__STDC_VERSION__) and (__STDC_VERSION__ >= 199901)
   #define LANGULUS_FUNCTION() __func__
#elif defined(__cplusplus) and (__cplusplus >= 201103)
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
#ifdef LANGULUS_BUILD_SHARED_LIBRARIES
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

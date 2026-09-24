///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <cstdint>   // INTPTR_MAX == INT64_MAX can't work without it   

/// Sorry, Langulus is designed for at least C++23                            
#if __cplusplus < 202302L
   #error "Langulus requires at least a C++23 compliant compiler in order to build"
#endif

/// These macros seem evil, but read this:                                    
/// https://www.foonathan.net/2020/09/move-forward/                           
/// static_cast to rvalue reference                                           
#define LglsMov(...) static_cast<::std::remove_reference_t<decltype(__VA_ARGS__)>&&>(__VA_ARGS__)

/// static_cast to identity                                                   
/// The extra && aren't necessary as discussed above, but make it more robust 
/// in case it's used with a non-reference.                                   
#define LglsFwd(...) static_cast<decltype(__VA_ARGS__)&&>(__VA_ARGS__)

/// MARK: CMake options                                                       
/// Safe mode enables assumption checks all over the code                     
/// High overhead, usually enabled only when testing in debug builds          
#if defined(LANGULUS_OPTION_SAFE_MODE) or defined(LANGULUS_ASSERTION_LEVEL)
   #ifdef LANGULUS_ASSERTION_LEVEL
      #define LANGULUS_SAFE() LANGULUS_ASSERTION_LEVEL
   #else
      #define LANGULUS_SAFE() 2
   #endif
#else
   #define LANGULUS_SAFE() 0
#endif

#if LANGULUS_SAFE()
   #define IF_SAFE(a)      a
   #define IF_UNSAFE(a)    
#else
   #define IF_SAFE(a)      
   #define IF_UNSAFE(a)    a
#endif

/// Adds the appropriate noexcept specifiers for functions that throw only    
/// in safe builds                                                            
#define assumptious IF_UNSAFE(noexcept)

/// Testing mode exposes some otherwise private functions                     
/// Overhead is unlikely                                                      
#ifdef LANGULUS_OPTION_TESTING
   #define LANGULUS_TESTING() 1
   #define IF_LANGULUS_TESTING(a)         a
   #define IF_NOT_LANGULUS_TESTING(a)     
#else
   #define LANGULUS_TESTING() 0
   #define IF_LANGULUS_TESTING(a)         
   #define IF_NOT_LANGULUS_TESTING(a)     a
#endif

/// Benchmarking                                                              
/// Tests will become radically slower                                        
#ifdef LANGULUS_OPTION_BENCHMARK
   #define LANGULUS_BENCHMARK() 1
   #define IF_LANGULUS_BENCHMARK(a)       a
   #define IF_NOT_LANGULUS_BENCHMARK(a)   LANGULUS(NOOP)
#else
   #define LANGULUS_BENCHMARK() 0
   #define IF_LANGULUS_BENCHMARK(a)       LANGULUS(NOOP)
   #define IF_NOT_LANGULUS_BENCHMARK(a)   a
#endif

/// Profiling                                                                 
#ifdef LANGULUS_OPTION_PROFILING
   #define LANGULUS_PROFILING() 1
   #define IF_LANGULUS_PROFILING(a)       a
   #define IF_NOT_LANGULUS_PROFILING(a)   LANGULUS(NOOP)
#else
   #define LANGULUS_PROFILING() 0
   #define IF_LANGULUS_PROFILING(a)       LANGULUS(NOOP)
   #define IF_NOT_LANGULUS_PROFILING(a)   a
#endif

/// Stacktracing                                                              
#ifdef LANGULUS_OPTION_STACKTRACE
   #define LANGULUS_STACKTRACE() 1
   #define IF_LANGULUS_STACKTRACE(a)      a
   #define IF_NOT_LANGULUS_STACKTRACE(a)  LANGULUS(NOOP)
#else
   #define LANGULUS_STACKTRACE() 0
   #define IF_LANGULUS_STACKTRACE(a)      LANGULUS(NOOP)
   #define IF_NOT_LANGULUS_STACKTRACE(a)  a
#endif

/// Forced type-erasure                                                       
#ifdef LANGULUS_OPTION_FORCE_TYPE_ERASURE
   #define LANGULUS_FORCE_TYPE_ERASURE() 1
   #define IF_LANGULUS_FORCE_TYPE_ERASURE(a)      a
   #define IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(a)
#else
   #define LANGULUS_FORCE_TYPE_ERASURE() 0
   #define IF_LANGULUS_FORCE_TYPE_ERASURE(a)
   #define IF_NOT_LANGULUS_FORCE_TYPE_ERASURE(a)  a
#endif

/// Paranoid mode introduces overhead, but zeroes any freed memory            
#ifdef LANGULUS_OPTION_PARANOIA
   #define LANGULUS_PARANOID() 1
   #define IF_LANGULUS_PARANOID(a)        a
   #define IF_NOT_LANGULUS_PARANOID(a)    LANGULUS(NOOP)
#else
   #define LANGULUS_PARANOID() 0
   #define IF_LANGULUS_PARANOID(a)        LANGULUS(NOOP)
   #define IF_NOT_LANGULUS_PARANOID(a)    a
#endif

/// Detect debug builds                                                       
#if defined(LANGULUS_OPTION_DEBUGGING) or (not defined(NDEBUG) \
                                       or defined(DEBUG) \
                                       or defined(_DEBUG) \
                                       or defined(CB_DEBUG) \
                                       or defined(QT_QML_DEBUG))
   #define LANGULUS_DEBUG()   1
   #define DEBUGGERY(a)       a
#else
   #define LANGULUS_DEBUG()   0
   #define DEBUGGERY(a)       LANGULUS(NOOP)
#endif

/// Reflections will be registered in a centralized location, allowing for    
/// runtime type modification. Meta primitives will always be in the same     
/// place in memory regardless of translation unit, which significantly       
/// speeds up meta definition comparisons.                                    
/// Naming collisions will be detected upon type registration                 
/// Gives a significant overhead on program launch, no dependencies           
#ifdef LANGULUS_OPTION_MANAGED_REFLECTION
   #define LANGULUS_FEATURE_MANAGED_REFLECTION()   1
   #define IF_LANGULUS_MANAGED_REFLECTION(a)       a
   #define IF_NOT_LANGULUS_MANAGED_REFLECTION(a)   LANGULUS(NOOP)
#else
   #define LANGULUS_FEATURE_MANAGED_REFLECTION()   0
   #define IF_LANGULUS_MANAGED_REFLECTION(a)       LANGULUS(NOOP)
   #define IF_NOT_LANGULUS_MANAGED_REFLECTION(a)   a
#endif

/// Memory allocations will be pooled, authority will be tracked,             
/// memory will be reused whenever possible, and you can also tweak           
/// runtime allocation strategies on per-type basis                           
/// Significantly improves performance, no dependencies                       
#ifdef LANGULUS_OPTION_MANAGED_MEMORY
   #define LANGULUS_FEATURE_MANAGED_MEMORY()    1
   #define IF_LANGULUS_MANAGED_MEMORY(a)        a
   #define IF_NOT_LANGULUS_MANAGED_MEMORY(a)    LANGULUS(NOOP)
#else
   #define LANGULUS_FEATURE_MANAGED_MEMORY()    0
   #define IF_LANGULUS_MANAGED_MEMORY(a)        LANGULUS(NOOP)
   #define IF_NOT_LANGULUS_MANAGED_MEMORY(a)    a
#endif

/// Memory manager shall keep track of statistics                             
/// Some overhead upon allocation/deallocation/reallocation                   
/// Some methods, like string null-termination will pick more memory-         
/// consitent, but less performant approaches (see Text::Terminate())         
#ifdef LANGULUS_OPTION_MEMORY_STATISTICS
   #define LANGULUS_FEATURE_MEMORY_STATISTICS() 1
   #define IF_LANGULUS_MEMORY_STATISTICS(a)     a
   #define IF_NOT_LANGULUS_MEMORY_STATISTICS(a) LANGULUS(NOOP)
#else
   #define LANGULUS_FEATURE_MEMORY_STATISTICS() 0
   #define IF_LANGULUS_MEMORY_STATISTICS(a)     LANGULUS(NOOP)
   #define IF_NOT_LANGULUS_MEMORY_STATISTICS(a) a
#endif

/// Replace the default new-delete operators with custom ones                 
/// No overhead, no dependencies                                              
#ifdef LANGULUS_OPTION_NEWDELETE
   #define LANGULUS_FEATURE_NEWDELETE()   1
   #define IF_LANGULUS_NEWDELETE(a)       a
   #define IF_NOT_LANGULUS_NEWDELETE(a)   LANGULUS(NOOP)
#else
   #define LANGULUS_FEATURE_NEWDELETE()   0
   #define IF_LANGULUS_NEWDELETE(a)       LANGULUS(NOOP)
   #define IF_NOT_LANGULUS_NEWDELETE(a)   a
#endif

/// Enables utf support and utilities for Text container                      
/// No runtime overhead                                                       
#ifdef LANGULUS_OPTION_UNICODE
   #define LANGULUS_FEATURE_UNICODE()     1
   #define IF_LANGULUS_UNICODE(a)         a
   #define IF_NOT_LANGULUS_UNICODE(a)     LANGULUS(NOOP)
#else
   #define LANGULUS_FEATURE_UNICODE()     0
   #define IF_LANGULUS_UNICODE(a)         LANGULUS(NOOP)
   #define IF_NOT_LANGULUS_UNICODE(a)     a
#endif

/// Enable memory compression utilities for containers                        
/// Gives a bit of general runtime overhead, zstd will be linked              
#ifdef LANGULUS_OPTION_COMPRESSION
   #define LANGULUS_FEATURE_COMPRESSION() 1
   #define IF_LANGULUS_COMPRESSION(a)     a
   #define IF_NOT_LANGULUS_COMPRESSION(a) LANGULUS(NOOP)
#else
   #define LANGULUS_FEATURE_COMPRESSION() 0
   #define IF_LANGULUS_COMPRESSION(a)     LANGULUS(NOOP)
   #define IF_NOT_LANGULUS_COMPRESSION(a) a
#endif

/// Enable memory encryption and decryption                                   
/// Gives a tiny runtime overhead, no dependencies                            
#ifdef LANGULUS_OPTION_ENCRYPTION
   #define LANGULUS_FEATURE_ENCRYPTION()  1
   #define IF_LANGULUS_ENCRYPTION(a)      a
   #define IF_NOT_LANGULUS_ENCRYPTION(a)  LANGULUS(NOOP)
#else
   #define LANGULUS_FEATURE_ENCRYPTION()  0
   #define IF_LANGULUS_ENCRYPTION(a)      LANGULUS(NOOP)
   #define IF_NOT_LANGULUS_ENCRYPTION(a)  a
#endif

/// Enable logging                                                            
/// Depends on libfmt                                                         
#ifdef LANGULUS_OPTION_LOGGING
   #define LANGULUS_FEATURE_LOGGING()  1
   #define IF_LANGULUS_LOGGING(a)      a
   #define IF_NOT_LANGULUS_LOGGING(a)  LANGULUS(NOOP)
#else
   #define LANGULUS_FEATURE_LOGGING()  0
   #define IF_LANGULUS_LOGGING(a)      LANGULUS(NOOP)
   #define IF_NOT_LANGULUS_LOGGING(a)  a
#endif


/// MARK: Compiler                                                            
/// Detect compiler                                                           
#if defined(__GNUC__) and not defined(__clang__)
   // We're on a GNUC Compiler!                                         
   #if __GNUC__ < 14 or (__GNUC__ == 14 and __GNUC_MINOR__ < 2)
      #error "Langulus can only be built with GCC 14.2 or above"
   #endif
   #define LANGULUS_COMPILER_GCC() 1
   #define LANGULUS_EBCO
#else
   #define LANGULUS_COMPILER_GCC() 0
#endif

#if defined(__clang__) and not LANGULUS_COMPILER_GCC() and defined(_MSC_VER)
   // We're on a clang-cl compiler!                                     
   #if __clang_major__ < 19
      #error "Langulus can only be built with Clang 19 or above"
   #endif
   #define LANGULUS_COMPILER_CLANG_CL() 1
   #define LANGULUS_EBCO __declspec(empty_bases)
#else
   #define LANGULUS_COMPILER_CLANG_CL() 0
#endif

#if defined(__clang__) and not LANGULUS_COMPILER_GCC()
   // We're on a clang compiler!                                        
   // @attention this can be enabled together with clang-cl!            
   #if __clang_major__ < 19
      #error "Langulus can only be built with Clang 19 or above"
   #endif
   #define LANGULUS_COMPILER_CLANG() 1
   #ifndef LANGULUS_EBCO
      #define LANGULUS_EBCO
   #endif
#else
   #define LANGULUS_COMPILER_CLANG() 0
#endif

#if defined(_MSC_VER) and not defined(__clang__)
   // We're on a microsoft visual c++ compiler                          
   #if _MSC_VER < 1950
      #error "Can only be built with MSVC v145 or above"
   #endif
   #define LANGULUS_COMPILER_MSVC() 1
   #define LANGULUS_EBCO __declspec(empty_bases)
#else
   #define LANGULUS_COMPILER_MSVC() 0
#endif

#if defined(__wasm__)
   // We're on a web assembly compiler!                                 
   #define LANGULUS_COMPILER_WASM() 1
   #define LANGULUS_EBCO
#else
   #define LANGULUS_COMPILER_WASM() 0
#endif

#if defined(__MINGW32__) or defined(__MINGW64__) 
   // We're on a mingw compiler!                                        
   #define LANGULUS_COMPILER_MINGW() 1
   #define LANGULUS_EBCO __declspec(empty_bases)
#else
   #define LANGULUS_COMPILER_MINGW() 0
#endif

/// Checks if a given compiler is enabled                                     
#define LANGULUS_COMPILER(a) LANGULUS_COMPILER_##a()

/// Here's an alternative well-tested solution for now:                       
/// Shamelessly stolen from boost and extended to my liking                   
/// Dumps the current function name                                           
#if defined(__GNUC__) or (defined(__MWERKS__) and (__MWERKS__ >= 0x3000)) or (defined(__ICC) and (__ICC >= 600)) or defined(__ghs__)
   #define LANGULUS_FUNCTION() __PRETTY_FUNCTION__
#elif defined(__clang__) or defined(__wasm__)
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
   #error "LANGULUS_FUNCTION not implemented"
#endif

/// Utility macro that turns its argument to a string literal (inner)         
#define LANGULUS_STRINGIFY_INNER(x) #x

/// Utility macro that turns its argument to a string literal                 
#define LANGULUS_STRINGIFY(x) LANGULUS_STRINGIFY_INNER(x)

/// Macro that generates a literal with the file and line                     
#define LANGULUS_LOCATION() __FILE__ ":" LANGULUS_STRINGIFY(__LINE__)
#define HERE() LANGULUS_LOCATION()

#define LANGULUS_OS(a) LANGULUS_OS_##a()

/// MARK: OS                                                                  
/// Detect operating system                                                   
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
#ifdef LANGULUS_OPTION_SHARED_LIBRARIES
   #if LANGULUS_COMPILER(GCC) or LANGULUS_COMPILER(CLANG) or LANGULUS_COMPILER(WASM)
      #if LANGULUS_OS(WINDOWS)
         #define LANGULUS_EXPORT() __attribute__ ((dllexport))
         #define LANGULUS_IMPORT() __attribute__ ((dllimport))
      #else
         #define LANGULUS_EXPORT() __attribute__ ((visibility("default")))
         #define LANGULUS_IMPORT() // requires -fvisibility=hidden      
      #endif
   #elif LANGULUS_COMPILER(MSVC) or LANGULUS_COMPILER(MINGW)
      #define LANGULUS_EXPORT() __declspec(dllexport)
      #define LANGULUS_IMPORT() __declspec(dllimport)
   #else 
      #error "Compiler not implemented"
   #endif
#else
   /// Shared library exports are disabled                                    
   #define LANGULUS_EXPORT()
   #define LANGULUS_IMPORT()
#endif

/// Useful for globally exporting everything when building the framework      
#ifdef LANGULUS_EXPORT_ALL
   #define LANGULUS_API_ALL() LANGULUS_EXPORT()
#else
   #define LANGULUS_API_ALL() LANGULUS_IMPORT()
#endif

/// Used to define imports/exports per module                                 
#define LANGULUS_API(a) LANGULUS_API_##a()

/// Make the rest of the code aware that Langulus::Core has been included     
#define LANGULUS_LIBRARY_CORE() 1

/// All non-argument macros should use this facility                          
/// https://www.fluentcpp.com/2019/05/28/better-macros-better-flags/          
#define LANGULUS(a) LANGULUS_##a()

/// Checks if a library is included                                           
#define LANGULUS_LIBRARY(a) LANGULUS_LIBRARY_##a()

/// Checks if a feature is enabled                                            
#define LANGULUS_FEATURE(a) LANGULUS_FEATURE_##a()

/// No-op for empty macros, forces coder to add a semicolon to avoid          
/// obscure errors                                                            
#define LANGULUS_NOOP() ((void)0)

/// MARK: Pragmas                                                             
#if LANGULUS_COMPILER(MSVC)
   #define LglsNoSideEffects
   #define LglsPure
   #define LglsCompilerSpecificAssume(a, ...) __assume(a)

   /// Force no inlining                                                      
   #define LANGULUS_NOINLINE() __declspec(noinline)

   /// Force inlining, even on debug builds                                   
   #define LANGULUS_ALWAYS_INLINED() __forceinline

   #if LANGULUS(DEBUG)
      #define LANGULUS_INLINED() inline
   #else
      /// Force always inlining - significantly increases build time!         
      #define LANGULUS_INLINED() __forceinline
   #endif

   #define LglsDisableWarningPush  __pragma(warning( push ))
   #define LglsDisableWarningPop   __pragma(warning( pop )) 
   #define LglsDisableWarning(W)   __pragma(warning( disable : W ))
   #define LglsDisableWarning_SelfAssign
   #define LglsDisableWarning_UnusedLocalTypedef
   #define LglsDisableWarning_NonTemplateFriend
#else
   #define LglsNoSideEffects __attribute__((const))
   #define LglsPure __attribute__((pure))
   #define LglsCompilerSpecificAssume(a, ...) [[assume(static_cast<bool>(a))]]

   /// Force no inlining                                                      
   #define LANGULUS_NOINLINE() __attribute__((noinline))

   /// Force inlining, even on debug builds                                   
   #define LANGULUS_ALWAYS_INLINED() __attribute__((always_inline)) inline

   #if LANGULUS(DEBUG)
      #define LANGULUS_INLINED() inline
   #else
      /// Force always inlining - significantly increases build time!         
      #define LANGULUS_INLINED() __attribute__((always_inline)) inline
   #endif

   #define LglsDoPragma(X)         _Pragma(#X)
   #define LglsDisableWarningPush  LglsDoPragma(GCC diagnostic push)
   #define LglsDisableWarningPop   LglsDoPragma(GCC diagnostic pop)
   #define LglsDisableWarning(W)   LglsDoPragma(GCC diagnostic ignored #W)
   #if LANGULUS_COMPILER(CLANG)
      #define LglsDisableWarning_SelfAssign LglsDisableWarning(-Wself-assign-overloaded)
      #define LglsDisableWarning_UnusedLocalTypedef LglsDisableWarning(-Wunused-local-typedef)
      #define LglsDisableWarning_NonTemplateFriend
   #else
      #define LglsDisableWarning_SelfAssign
      #define LglsDisableWarning_UnusedLocalTypedef
      #define LglsDisableWarning_NonTemplateFriend LglsDisableWarning(-Wnon-template-friend)
   #endif
#endif

#ifndef LANGULUS_ALIGNMENT
   /// The default memory alignment in bytes                                  
   #define LANGULUS_ALIGNMENT 16
#endif

#ifndef LANGULUS_MIN_ALLOC
   /// The smallest possible allocation in bytes                              
   ///   @attention should be greater or equal to the alignment               
   #define LANGULUS_MIN_ALLOC LANGULUS_ALIGNMENT
#endif

#ifndef LANGULUS_MIN_POOL
   /// The smallest possible memory pool size in bytes                        
   ///   @attention should be greater than the minimal allocation             
   #define LANGULUS_MIN_POOL 1024*1024
#endif

#ifndef LANGULUS_HASHSIZE
   /// The size of the hashes, in bits                                        
   #define LANGULUS_HASHSIZE 32
#endif

#if LANGULUS_HASHSIZE != 32 and LANGULUS_HASHSIZE != 64
   #error "Unsupported hash size"
#endif

#ifndef LANGULUS_FPU
   /// The size of the default real number representation. Same type is used  
   /// to determine the default type of number literals in Flow scripts       
   #define LANGULUS_FPU 32
#endif

#if LANGULUS_FPU == 16 or LANGULUS_FPU == 128
   #include <stdfloat>
#endif

/// Detect architecture                                                       
#if INTPTR_MAX == INT64_MAX
   #define LANGULUS_BITNESS() 64
#elif INTPTR_MAX == INT32_MAX
   #define LANGULUS_BITNESS() 32
#else
   #error Unknown architecture
#endif

namespace Langulus
{
   /// MARK: Yup                                                              
   /// Equivalent to ::std::true_type, but without the silliness              
   struct Yup {
      using CTTI_ReflectAs = void;
      static constexpr bool Enabled = true;
   };

   /// Equivalent to ::std::false_type or ::std::true_type, depending on arg  
   /// Can carry a constant with itself                                       
   template<bool CONDITION>
   struct Maybe {
      using CTTI_ReflectAs = void;
      static constexpr bool Enabled = CONDITION;
   };

   /// Equivalent to ::std::false_type, but without the silliness             
   struct No {
      using CTTI_ReflectAs = void;
      static constexpr bool Enabled = false;
   };

   /// Same as ::std::declval, but more intuitively named                     
   template<class T>
   T&& Fake() { static_assert(false, "Calling Fake is ill-formed"); }

   /// The default floating point type, depends on configuration              
   #if LANGULUS_FPU == 16
      using Real = float16_t;
   #elif LANGULUS_FPU == 32
      using Real = float;
      static_assert(sizeof(Real) == 4);
   #elif LANGULUS_FPU == 64
      using Real = double;
      static_assert(sizeof(Real) == 8);
   #elif LANGULUS_FPU == 128
      using Real = float128_t;
   #else
      #error "Unsupported real number size"
   #endif

   /// The size of a void* in bytes, depends on architecture                  
   constexpr size_t Byteness = sizeof(void*);

   /// The size of a void* in bits, depends on architecture                   
   constexpr size_t Bitness = Byteness * 8;

   /// The default alignment, configure via LANGULUS_ALIGNMENT                
   constexpr uintptr_t Alignment = LANGULUS_ALIGNMENT;
   static_assert(Alignment >= Byteness);

   /// The default allocation size, configure via LANGULUS_MIN_ALLOC          
   constexpr size_t MinimalAllocation = LANGULUS_MIN_ALLOC;
   static_assert(MinimalAllocation >= Alignment);
   
   /// The smallest possible pool size, configure via LANGULUS_MIN_POOL       
   constexpr size_t MinimalPoolSize = LANGULUS_MIN_POOL;
   static_assert(MinimalPoolSize > MinimalAllocation);
}

#define LglsFake(...) ::Langulus::Fake<__VA_ARGS__>()

/// Syntax sugar for calling pesky templated lambdas                          
#define LglsLamb(a, ...) a.template operator()<__VA_ARGS__>()
#define LglsLambStatic(a, ...) ::std::decay_t<decltype(a)>::template operator()<__VA_ARGS__>()

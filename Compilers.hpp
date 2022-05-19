#pragma once

/// Sorry, Langulus is designed for at least C++20 									
#if __cplusplus <= 201703L && !defined(_MSC_VER)
	#error Langulus requires at least a C++20 compliant compiler in order to build
#endif

/// Detect debug builds																			
#if defined(DEBUG) || !defined(NDEBUG) || defined(_DEBUG) || defined(CB_DEBUG) || defined(QT_QML_DEBUG)
	#define LANGULUS_DEBUG() 1
	#define DEBUGGERY(a) a
#else
	#define LANGULUS_DEBUG() 0
	#define DEBUGGERY(a)
#endif

/// LANGULUS(NOINLINE) attribute																
#ifdef _MSC_VER
	#define LANGULUS_NOINLINE() __declspec(noinline)
#else
	#define LANGULUS_NOINLINE() __attribute__((noinline))
#endif

/// LANGULUS(LIKELY) and LANGULUS(UNLIKELY) attributes								
#ifdef _MSC_VER
	#define LANGULUS_LIKELY(condition) condition
	#define LANGULUS_UNLIKELY(condition) condition
#else
	#define LANGULUS_LIKELY(condition) __builtin_expect(condition, 1)
	#define LANGULUS_UNLIKELY(condition) __builtin_expect(condition, 0)
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
#if defined(__GNUC__)
	// We're on a GNUC Compiler!														
	#define LANGULUS_COMPILER_GCC() 1
	#define LANGULUS_COMPILER_MSVC() 0
	#define LANGULUS_COMPILER_CLANG() 0
	#define LANGULUS_COMPILER_MINGW() 0
#elif defined(_MSC_VER)
	// We're on a microsoft visual c++ compiler!									
	#define LANGULUS_COMPILER_GCC() 0
	#define LANGULUS_COMPILER_MSVC() 1
	#define LANGULUS_COMPILER_CLANG() 0
	#define LANGULUS_COMPILER_MINGW() 0
#elif defined(__clang__)
	// We're on a clang compiler!														
	#define LANGULUS_COMPILER_GCC() 0
	#define LANGULUS_COMPILER_MSVC() 0
	#define LANGULUS_COMPILER_CLANG() 1
	#define LANGULUS_COMPILER_MINGW() 0
#elif defined(__MINGW32__) || defined(__MINGW64__) 
	// We're on a mingw compiler!														
	#define LANGULUS_COMPILER_GCC() 0
	#define LANGULUS_COMPILER_MSVC() 0
	#define LANGULUS_COMPILER_CLANG() 0
	#define LANGULUS_COMPILER_MINGW() 1
#else
	#error Unrecognized compiler
#endif

#define LANGULUS_COMPILER(a) LANGULUS_COMPILER_##a()

/// Debuggery macro for convenience															
#if LANGULUS_DEBUG()
	#define DEBUGGERY(a) a
#else
	#define DEBUGGERY(a)
#endif

/// Shamelessly stolen from boost and extended to my liking							
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
	#define LANGULUS_FUNCTION() "(unknown)"
#endif

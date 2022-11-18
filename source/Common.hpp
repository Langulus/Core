///                                                                           
/// Langulus::Core                                                            
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Config.hpp"
#include <type_traits>
#include <typeinfo>
#include <cstddef>
#include <functional>
#include <span>
#include <string_view>
#include <limits>
#include <concepts>
#include <bit>
#include <cstring>

/// All non-argument macros should use this facility                          
/// https://www.fluentcpp.com/2019/05/28/better-macros-better-flags/          
#define LANGULUS(a) LANGULUS_##a()

/// Checks if a library is included                                           
#define LANGULUS_LIBRARY(a) LANGULUS_LIBRARY_##a()

/// Checks if a feature is enabled                                            
#define LANGULUS_FEATURE(a) LANGULUS_FEATURE_##a()

/// Convenience macro, for tagging unused declarations                        
#define UNUSED() [[maybe_unused]]

/// Convenience macro, for tagging non-discardable returns                    
#define NOD() [[nodiscard]]

/// Likely/Unlikely attributes                                                
#define LIKELY() [[likely]]
#define UNLIKELY() [[unlikely]]

/// Trigger a static assert (without condition)                               
/// This form is required in order of it to work in 'if constexpr - else'     
/// https://stackoverflow.com/questions/38304847                              
#define LANGULUS_ERROR(text) []<bool flag = false>() { static_assert(flag, "LANGULUS ERROR: " text); }()

#ifdef _MSC_VER
   /// Force no inlining                                                      
   #define LANGULUS_NOINLINE() __declspec(noinline)
   /// Force always inlining                                                  
   #define LANGULUS_ALWAYSINLINE() __forceinline
#else
   /// Force no inlining                                                      
   #define LANGULUS_NOINLINE() __attribute__((noinline))
   /// Force always inlining                                                  
   #define LANGULUS_ALWAYSINLINE() __attribute__((always_inline)) inline
#endif

namespace Langulus
{

   ///                                                                        
   ///   Fundamental types, some of them across multiple submodules           
   ///                                                                        
   namespace Flow
   {
      class Verb;
   }

   namespace RTTI
   {
      struct Member;
      struct Base;
      struct Ability;
      struct Meta;
      struct MetaData;
      struct MetaVerb;
      struct MetaTrait;
      struct MetaConst;

      using DMeta = const MetaData*;
      using TMeta = const MetaTrait*;
      using VMeta = const MetaVerb*;
      using CMeta = const MetaConst*;
   }

   namespace Anyness
   {
      class Block;
      class Trait;
      class Any;
      class Map;
   }

   /// Type for counting things, that depends on architecture                 
   using Count = ::std::size_t;
   constexpr Count CountMax = ::std::numeric_limits<Count>::max();

   /// Type for counting bytes, that depends on architecture                  
   using Size = ::std::size_t;
   constexpr Size SizeMax = ::std::numeric_limits<Size>::max();

   /// Type for offsetting pointers, depends on architecture                  
   using Offset = ::std::size_t;
   constexpr Offset OffsetMax = ::std::numeric_limits<Offset>::max();

   /// Type that holds a hash, depends on architecture                        
   struct Hash {
      ::std::size_t mHash {};

      explicit constexpr operator bool() const noexcept { return mHash != 0; }
      constexpr bool operator == (const Hash&) const noexcept = default;
   }; 

   /// Type for wrapping a function                                           
   template<class T>
   using TFunctor = ::std::function<T>;

   /// Type for a character                                                   
   using Letter = char;

   /// Type for wrapping a compile-time string                                
   using Token = ::std::basic_string_view<Letter>;

   /// Integer equivalent to a pointer, depends on architecture               
   using Pointer = ::std::uintptr_t;

   /// Single precision float                                                 
   using Float = float;

   /// Double precision float                                                 
   using Double = double;

   /// The default floating point type, depends on configuration              
   #if defined(LANGULUS_FPU_FLOAT)
      using Real = Float;
   #elif defined(LANGULUS_FPU_DOUBLE)
      using Real = Double;
   #else
      using Real = Float;
   #endif


   /// Check endianness at compile-time                                       
   /// True if the architecture uses big/mixed endianness                     
   constexpr bool BigEndianMachine = ::std::endian::native == ::std::endian::big;
   /// True if the architecture uses little/mixed endianness                  
   constexpr bool LittleEndianMachine = ::std::endian::native == ::std::endian::little;

   #ifndef LANGULUS_ALIGNMENT
      #define LANGULUS_ALIGNMENT 16
   #endif

   /// The default alignment, depends on configuration and enabled SIMD       
   constexpr Size Alignment = LANGULUS_ALIGNMENT;

   /// The bitness                                                            
   constexpr Size Bitness = LANGULUS(BITNESS);

   /// Same as ::std::declval, but conveniently named                         
   template<class T>
   ::std::add_rvalue_reference_t<T> Uneval() noexcept {
      LANGULUS_ERROR("Calling Uneval is ill-formed");
   }

   /// Same as ::std::declval, but deduces type via argument                  
   template<class T>
   ::std::add_rvalue_reference_t<T> Uneval(T) noexcept {
      LANGULUS_ERROR("Calling Uneval is ill-formed");
   }

   namespace Inner
   {
      /// This function declaration is used to decompose a lambda             
      /// You can use it to extract the argument type of the lambda, using    
      /// decltype on the return type                                         
      /// Handles functors, member/standing function pointers, lambdas        
      template<class R, class F, class A>
      A GetFunctionArgument(R(F::*)(A) const) {
         LANGULUS_ERROR("Calling GetFunctionArgument is ill-formed");
      }
      template<class R, class F, class A>
      A GetFunctionArgument(R(F::*)(A)) {
         LANGULUS_ERROR("Calling GetFunctionArgument is ill-formed");
      }
      template<class R, class A>
      A GetFunctionArgument(R(*)(A)) {
         LANGULUS_ERROR("Calling GetFunctionArgument is ill-formed");
      }
      template<class F>
      decltype(GetFunctionArgument(&F::operator())) GetFunctionArgument(F) {
         LANGULUS_ERROR("Calling GetFunctionArgument is ill-formed");
      }
   }

   /// Get the type of the argument of a function                             
   ///   @tparam F - anything invokable, like functor/member function/lambda  
   template<class F>
   using ArgumentOf = decltype(Inner::GetFunctionArgument(Uneval<F>()));

   /// Get the return type of a function                                      
   ///   @tparam F - anything invokable, like functor/member function/lambda  
   template<class F>
   using ReturnOf = decltype((Uneval<F>()) (Uneval<ArgumentOf<F>>()));

   /// Remove a reference from type                                           
   template<class T>
   using Deref = ::std::remove_reference_t<T>;

   /// Remove a pointer from type                                             
   template<class T>
   using Deptr = ::std::remove_pointer_t<T>;

   /// Remove a const/volatile from a type                                    
   template<class T>
   using Decvq = ::std::remove_cv_t<T>;

   /// Remove an array extent from a type                                     
   template<class T>
   using Deext = ::std::remove_extent_t<T>;

   /// Strip a typename to its root type, removing qualifiers/pointers/etc.   
   /// Note that this strips only 1D array, one reference, one pointer...     
   /// You can chain multiple Decay<Decay<T>> if not sure                     
   template<class T>
   using Decay = Decvq<Deptr<Deext<Deref<T>>>>;

   /// A namespace dedicated for Compile Time checks and ConcepTs             
   namespace CT
   {
         
      ///                                                                     
      ///   Concepts                                                          
      ///                                                                     
      /// True if decayed T1 matches all decayed T2                           
      ///   @attention ignores type density and cv-qualifications             
      template<class T1, class... T2>
      concept Same = ((::std::same_as<Decay<T1>, Decay<T2>>) && ...);

      /// True if T1 matches exactly T2, including density and cv-qualifiers  
      template<class T1, class... T2>
      concept Exact = ((::std::same_as<T1, T2>) && ...);

      /// True if decayed T1 matches any of decayed T2                        
      ///   @attention ignores type density and cv-qualifications             
      template<class T1, class... T2>
      concept SameAsOneOf = ((::std::same_as<Decay<T1>, Decay<T2>>) || ...);

      /// True if T1 matches exactly one of T2, including density and cvq     
      template<class T1, class... T2>
      concept ExactAsOneOf = ((::std::same_as<T1, T2>) || ...);

      /// True if T is an array (has an extent with [])                       
      /// Sometimes a reference hides the pointer/extent, hence the deref     
      template<class... T>
      concept Array = (::std::is_bounded_array_v<Deref<T>> && ...);

      /// True if T is a pointer (or has an extent with [])                   
      /// Sometimes a reference hides the pointer/extent, hence the deref     
      template<class... T>
      concept Sparse = ((::std::is_pointer_v<Deref<T>> || Array<T>) && ...);

      /// True if T is not a pointer (and has no extent with [])              
      template<class... T>
      concept Dense = (!Sparse<T> && ...);

      /// Check if type is constant-qualified                                 
      template<class... T>
      concept Constant = (::std::is_const_v<Deptr<Deref<T>>> && ...);

      /// Check if type is not constant-qualified                             
      template<class... T>
      concept Mutable = (!Constant<T> && ...);

      /// Check if type is signed (either sparse or dense)                    
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class... T>
      concept Signed = (::std::is_signed_v<Decay<T>> && ...);

      /// Check if type is signed and dense                                   
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class... T>
      concept DenseSigned = ((Signed<T> && Dense<T>) && ...);

      /// Check if type is signed and sparse                                  
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class... T>
      concept SparseSigned = ((Signed<T> && Sparse<T>) && ...);

      /// Check if type is unsigned (either sparse or dense)                  
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class... T>
      concept Unsigned = (!Signed<T> && ...);

      /// Check if type is unsigned and dense                                 
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class... T>
      concept DenseUnsigned = ((Unsigned<T> && Dense<T>) && ...);

      /// Check if type is unsigned and sparse                                
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class... T>
      concept SparseUnsigned = ((Unsigned<T> && Sparse<T>) && ...);

      /// Built-in boolean concept (either sparse or dense)                   
      template<class... T>
      concept BuiltinBool = (Same<T, bool> && ...);

      /// Built-in character concept (either sparse or dense)                 
      template<class... T>
      concept BuiltinCharacter = ((SameAsOneOf<T, char, unsigned char, char8_t, char16_t, char32_t, wchar_t>) && ...);

      /// Built-in integer number concept (either sparse or dense)            
      ///   @attention excludes boolean types and char types                  
      template<class... T>
      concept BuiltinInteger = ((::std::integral<Decay<T>> && !BuiltinBool<T> && !BuiltinCharacter<T>) && ...);

      /// Check if type is any signed integer (either sparse or dense)			
      template<class... T>
      concept BuiltinSignedInteger = ((BuiltinInteger<T> && Signed<T>) && ...);

      /// Check if type is any unsigned integer (either sparse or dense)		
      template<class... T>
      concept BuiltinUnsignedInteger = ((BuiltinInteger<T> && Unsigned<T>) && ...);

      /// Built-in real number concept (either sparse or dense)               
      template<class... T>
      concept BuiltinReal = (::std::floating_point<Decay<T>> && ...);

      /// Built-in number concept (either sparse or dense)							
      ///   @attention excludes boolean types and char types                  
      template<class... T>
      concept BuiltinNumber = ((BuiltinInteger<T> || BuiltinReal<T>) && ...);

      namespace Inner
      {
         template<class T, class U>
         concept Sortable = requires(Decay<T> t, Decay<U> u) {
            { t < u } -> Exact<bool>;
            { t > u } -> Exact<bool>;
         };
      }

      /// Sortable concept for any decayed T and U, with an adequate <, >,    
      /// or combined <=> operator                                            
      template<class T, class... U>
      concept Sortable = (Inner::Sortable<T, U> && ...);
   
      namespace Inner
      {
         template<class T, class U>
         concept Comparable = ::std::equality_comparable_with<Decay<T>, Decay<U>>;
      }

      /// Equality comparable concept for any decayed T and U, with an        
      /// adequate == operator                                                
      template<class T, class... U>
      concept Comparable = (Inner::Comparable<T, U> && ...);

      namespace Inner
      {
         template<class FROM, class TO>
         concept Convertible = requires(const FROM& from) {
            static_cast<TO>(from);
         };
      }

      /// Convertible concept                                                 
      /// Checks if a static_cast is possible between the provided types      
      template<class FROM, class... TO>
      concept Convertible = (Inner::Convertible<FROM, TO> && ...);

      /// Check if T is a fundamental type (either sparse or dense)           
      template<class... T>
      concept Fundamental = (::std::is_fundamental_v<Decay<T>> && ...);

      /// Check if T is an arithmetic type (either sparse or dense)           
      template<class... T>
      concept Arithmetic = (::std::is_arithmetic_v<Decay<T>> && ...);

      /// Check if type is moved (is an rvalue)                               
      template<class... T>
      concept Moved = (::std::is_rvalue_reference_v<T> && ...);

      /// Check if the decayed T is default-constructible                     
      template<class... T>
      concept Defaultable = (requires { Decay<T>{}; } && ...);

      template<class... T>
      concept DefaultableNoexcept = Defaultable<T...> && (noexcept(T{}) && ...);
   
      /// Check if the decayed T is copy-constructible                        
      template<class... T>
      concept CopyMakable = (requires (const Decay<T>& a) { 
         Decay<T> {a};
      } && ...);

      template<class... T>
      concept CopyMakableNoexcept = CopyMakable<T...> && (noexcept(T{Uneval<const T&>()}) && ...);
         
      /// Check if the decayed T is move-constructible                        
      template<class... T>
      concept MoveMakable = (requires (Decay<T>&& a) { 
         Decay<T> {::std::forward<Decay<T>>(a)};
      } && ...);

      template<class... T>
      concept MoveMakableNoexcept = MoveMakable<T...> && (noexcept(T{Uneval<T&&>()}) && ...);
      
      /// Check if the decayed T is destructible                              
      template<class... T>
      concept Destroyable = (::std::destructible<Decay<T>> && ...);
   
      namespace Inner
      {
         template<class T>
         concept Clonable = requires (Decay<T> a) {
            {a.Clone()} -> Exact<Decay<T>>;
         };
      }

      /// Check if the decayed T is clonable                                  
      template<class... T>
      concept Clonable = (Inner::Clonable<T> && ...);

      namespace Inner
      {
         template<class T>
         concept Referencable = requires (const Decay<T> a) {
            {a.Keep()};
            {a.Free()} -> Exact<Count>;
            {a.GetReferences()} -> Exact<Count>;
         };
      }

      /// Check if the decayed T is referencable                              
      template<class... T>
      concept Referencable = (Inner::Referencable<T> && ...);

      /// Check if the decayed T is copy-assignable                           
      template<class... T>
      concept Copyable = (::std::copyable<Decay<T>> && ...);
      template<class... T>
      concept CopyableNoexcept = Copyable<T...> && (noexcept(Uneval<T&>() = Uneval<const T&>()) && ...);
         
      /// Check if the decayed T is move-assignable                           
      template<class... T>
      concept Movable = (::std::movable<Decay<T>> && ...);
      template<class... T>
      concept MovableNoexcept = Movable<T...> && (noexcept(Uneval<T&>() = Uneval<T&&>()) && ...);
         
      /// Check if T is swappable                                             
      template<class... T>
      concept Swappable = (::std::is_swappable_v<T> && ...);
      template<class... T>
      concept SwappableNoexcept = (::std::is_nothrow_swappable_v<T> && ...);
         
      namespace Inner
      {
         template<class T>
         concept Resolvable = requires (Decay<T> a) {
            {a.GetType()} -> Exact<RTTI::DMeta>;
            {a.GetBlock()} -> Exact<Anyness::Block>;
         };
      }

      /// Check if the decayed T is resolvable at runtime                     
      template<class... T>
      concept Resolvable = (Inner::Resolvable<T> && ...);

      namespace Inner
      {
         template<class T>
         concept Hashable = requires (Decay<T> a) {
            {a.GetHash()} -> Exact<Hash>;
         };
      }

      /// Check if the decayed T has custom GetHash() method                  
      template<class... T>
      concept Hashable = (Inner::Hashable<T> && ...);

      /// Check if the decayed T inherits BASE                                
      template<class T, class... BASE>
      concept DerivedFrom = (::std::derived_from<Decay<T>, Decay<BASE>> && ...);
   
      /// Check if type has no reference/pointer/extent, etc.                 
      template<class... T>
      concept Decayed = ((Dense<T> && !::std::is_reference_v<T> && Mutable<T>) && ...);
   
      /// Check if a function encapsulated in a lambda is a constexpr         
      /// Leverages that lambda expressions can be constexpr as of C++17      
      /// https://stackoverflow.com/questions/55288555                        
      template<class Lambda, int = (Lambda {}(), 0)>
      constexpr bool IsConstexpr(Lambda) { return true; }
      constexpr bool IsConstexpr(...) { return false; }
      
      /// Check if decayed T is either move-constructible, or move-assignable 
      template<class... T>
      concept MakableOrMovable = ((MoveMakable<T> || Movable<T>) && ...);

      /// Check if decayed T is either move-constructible, or move-assignable 
      /// (both being noexcept)                                               
      template<class... T>
      concept MakableOrMovableNoexcept = ((MoveMakableNoexcept<T> || MovableNoexcept<T>) && ...);

      /// Check if decayed T is either copy-constructible, or copy-assignable 
      template<class... T>
      concept MakableOrCopyable = ((CopyMakable<T> || Copyable<T>) && ...);

      /// Check if decayed T is either copy-constructible, or copy-assignable 
      /// (both being noexcept)                                               
      template<class... T>
      concept MakableOrCopyableNoexcept = ((CopyMakableNoexcept<T> || CopyableNoexcept<T>) && ...);
         
      /// Check if type is a dense void                                       
      template<class... T>
      concept Void = (::std::is_void_v<T> && ...);

      /// A reflected data type is any type that is not a dense void          
      template<class... T>
      concept Data = (!Void<T> && ...);

   } // namespace Langulus::CT

   /// Pick between two types, based on a condition                           
   template<bool CONDITION, class TRUETYPE, class FALSETYPE>
   using Conditional = ::std::conditional_t<CONDITION, TRUETYPE, FALSETYPE>;

   /// Get the extent of an array, or 1 if T is not an array                  
   template<class T>
   constexpr Count ExtentOf = CT::Array<T> ? ::std::extent_v<Deref<T>> : 1;

} // namespace Langulus

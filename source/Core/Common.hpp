///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Config.hpp"
#include "Size.hpp"
#include <type_traits>
#include <typeinfo>
#include <functional>
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
#define LANGULUS_ERROR(text) []<bool flag = false>() { \
      static_assert(flag, "LANGULUS ERROR: " text); \
   }()

/// Exploits [[deprecated("warning")]] to log template instantiations         
#define LANGULUS_TEMPLATE() [[deprecated("template intantiation")]]

/// Checks if code is executed at compile-time                                
///   @attention must be followed by {...}                                    
/// TODO when we transition to C++23, we should replace                       
/// if (std::is_constant_evaluated()) statements with `if consteval` ones     
#define IF_CONSTEXPR() if (std::is_constant_evaluated())

#if LANGULUS_COMPILER(MSVC)
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
#else
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
#endif

namespace Langulus
{

   ///                                                                        
   ///   Fundamental types, some of them across multiple libraries            
   ///                                                                        
   namespace Flow
   {

      struct Verb;

      template<class VERB>
      struct TVerb;

      template<class VERB, bool NOEXCEPT>
      struct ArithmeticVerb;
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
   }

   namespace A
   {
      struct Block;
   }

   namespace Anyness
   {
      template<class TYPE = void>
      struct Block;

      class Neat;
   }

   /// Type for counting things, depends on architecture                      
   using Count = ::std::size_t;
   constexpr Count CountMax = ::std::numeric_limits<Count>::max();

   /// Type for offsetting pointers, depends on architecture                  
   using Offset = ::std::size_t;
   constexpr Offset OffsetMax = ::std::numeric_limits<Offset>::max();

   /// Type that holds a hash - size depends on architecture                  
   struct Hash {
      static constexpr bool CTTI_POD = true;
      static constexpr bool CTTI_Nullifiable = true;

      ::std::size_t mHash {};

      LANGULUS(ALWAYS_INLINED)
      explicit constexpr operator bool() const noexcept {
         return mHash != 0;
      }

      constexpr Hash() noexcept = default;
      constexpr Hash(const Hash&) noexcept = default;

      LANGULUS(ALWAYS_INLINED)
      constexpr Hash(::std::size_t a) noexcept
         : mHash {a} {}

      constexpr bool operator == (const Hash&) const noexcept = default;

      constexpr Hash& operator = (const Hash&) noexcept = default;
   };

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

   LANGULUS(ALWAYS_INLINED)
   consteval Real operator "" _real(unsigned long long n) noexcept {
      return static_cast<Real>(n);
   }
   
   LANGULUS(ALWAYS_INLINED)
   consteval Real operator "" _real(long double n) noexcept {
      return static_cast<Real>(n);
   }



   /// Check endianness at compile-time                                       
   /// True if the architecture uses big/mixed endianness                     
   constexpr bool BigEndianMachine = ::std::endian::native == ::std::endian::big;
   /// True if the architecture uses little/mixed endianness                  
   constexpr bool LittleEndianMachine = ::std::endian::native == ::std::endian::little;

   #ifndef LANGULUS_ALIGNMENT
      #define LANGULUS_ALIGNMENT 16
   #endif
   
   /// The default alignment, depends on configuration and enabled SIMD       
   constexpr Offset Alignment = LANGULUS_ALIGNMENT;
   #define LANGULUS_ALIGNED() alignas(::Langulus::Alignment)

   /// The bitness                                                            
   constexpr Offset Bitness = LANGULUS(BITNESS);

   /// Same as ::std::declval, but conveniently named                         
   template<class T>
   ::std::add_rvalue_reference_t<T> Fake() noexcept {
      LANGULUS_ERROR("Calling Fake is ill-formed");
   }

   /// Same as ::std::declval, but deduces type via argument                  
   template<class T>
   ::std::add_rvalue_reference_t<T> Fake(T) noexcept {
      LANGULUS_ERROR("Calling Fake is ill-formed");
   }

   namespace Inner
   {
      /// Type used to detect unavailable stuff all over                      
      struct Unsupported {};

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
   using ArgumentOf = decltype(Inner::GetFunctionArgument(Fake<F>()));

   /// Get the return type of a function                                      
   ///   @tparam F - anything invokable, like functor/member function/lambda  
   template<class F>
   using ReturnOf = decltype((Fake<F>()) (Fake<ArgumentOf<F>>()));

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
   using Deext = ::std::remove_extent_t<Deref<T>>;

   namespace Inner
   {

      template<class T>
      consteval auto NestedDecay() {
         using Stripped = Decvq<Deptr<Deext<T>>>;
         if constexpr (::std::same_as<T, Stripped>)
            return static_cast<Stripped*>(nullptr);
         else
            return NestedDecay<Stripped>();
      }

      template<class T1, class T2>
      consteval bool NestedSimilar() {
         using Stripped1 = Decvq<Deref<T1>>;
         using Stripped2 = Decvq<Deref<T2>>;
         if constexpr (::std::same_as<Stripped1, Stripped2>)
            return true;
         else if constexpr (::std::is_pointer_v<Stripped1>
                        and ::std::is_pointer_v<Stripped2>)
            return NestedSimilar<::std::remove_pointer_t<Stripped1>,
                                 ::std::remove_pointer_t<Stripped2>>();
         else return false;
      }

   } // namespace Langulus::Inner

   /// Strip a typename to its origin type, removing qualifiers/pointers/etc. 
   /// This strongly guarantees, that it strips EVERYTHING, including nested  
   /// pointers, extents, etc.                                                
   template<class T>
   using Decay = Deptr<decltype(Inner::NestedDecay<T>())>;

   /// A namespace dedicated for Compile Time checks and ConcepTs             
   namespace CT
   {
         
      ///                                                                     
      ///   Concepts                                                          
      ///                                                                     

      /// Check if all T are complete (defined), by exploiting sizeof         
      /// Usefulness of this is limited to the first instantiation, and       
      /// that is how it is used upon reflection by RTTI. Any other use is    
      /// undefined and might produce wrong results on some compilers.        
      /// Thankfully, most modern compilers do detect, if a definition        
      /// changes between completeness checks, so it is unlikely to cause any 
      /// real harm: https://stackoverflow.com/questions/21119281             
      template<class...T>
      concept Complete = sizeof...(T) > 0
          and ((sizeof(T) == sizeof(T)) and ...);

      /// True if decayed T1 matches all decayed TN types                     
      ///   @attention ignores type density and cv-qualifications             
      template<class T1, class...TN>
      concept Same = sizeof...(TN) == 0
           or (::std::same_as<Decay<T1>, Decay<TN>> and ...);

      /// True if unqualified T1 matches all unqualified TN types             
      ///   @attention ignores cv-qualifications only                         
      template<class T1, class...TN>
      concept Similar = sizeof...(TN) == 0
           or (Langulus::Inner::NestedSimilar<T1, TN>() and ...);

      /// True if T1 matches exactly all the provided TN, including           
      /// density and cv-qualifiers                                           
      template<class T1, class...TN>
      concept Exact = sizeof...(TN) == 0
           or (::std::same_as<T1, TN> and ...);

      /// True if decayed T1 matches at least one of the decayed TN           
      ///   @attention ignores type density and cv-qualifications             
      template<class T1, class...TN>
      concept SameAsOneOf = sizeof...(TN) == 0
           or (::std::same_as<Decay<T1>, Decay<TN>> or ...);

      /// True if unqualified T1 matches at least one of the unqualified TN   
      ///   @attention ignores cv-qualifications only                         
      template<class T1, class...TN>
      concept SimilarAsOneOf = sizeof...(TN) == 0
           or (Langulus::Inner::NestedSimilar<T1, TN>() or ...);

      /// True if T1 matches exactly at least one of the TN, including        
      /// density and cv-qualifications                                       
      template<class T1, class...TN>
      concept ExactAsOneOf = sizeof...(TN) == 0
           or (::std::same_as<T1, TN> or ...);

      /// Check if any T is the built-in one that signifies lack of support   
      template<class...T>
      concept Unsupported = sizeof...(T) > 0
          and (Same<::Langulus::Inner::Unsupported, T> or ...);

      /// Check if all T are supported                                        
      template<class...T>
      concept Supported = sizeof...(T) > 0
          and ((not Unsupported<T>) and ...);

      /// True if all T are arrays (have extents with [])                     
      template<class...T>
      concept Array = sizeof...(T) > 0
          and (::std::is_bounded_array_v<Deref<T>> and ...);

      /// Check whether all T are raw function signatures                     
      template<class...T>
      concept Function = sizeof...(T) > 0
          and (::std::is_function_v<T> and ...);

      /// True if all T are pointers (or have extents with [])                
      template<class...T>
      concept Sparse = sizeof...(T) > 0
          and ((::std::is_pointer_v<Deref<T>> or Array<T>) and ...);

      /// True if all T are not pointers (and has no extent with [])          
      /// Each T is still allowed to be a reference                           
      template<class...T>
      concept Dense = sizeof...(T) > 0
          and ((not Sparse<T>) and ...);

      /// Check if all T are aggregates                                       
      template<class...T>
      concept Aggregate = sizeof...(T) > 0
          and (::std::is_aggregate_v<Deref<T>> and ...);

      /// Check if all T are not aggregates                                   
      template<class...T>
      concept NotAggregate = sizeof...(T) > 0
          and ((not ::std::is_aggregate_v<Deref<T>>) and ...);

      /// Check if all T are constant-qualified                               
      template<class...T>
      concept Constant = sizeof...(T) > 0
          and (::std::is_const_v<Deref<T>> and ...);

      /// Check if all T are volatile-qualified                               
      template<class...T>
      concept Volatile = sizeof...(T) > 0
          and (::std::is_volatile_v<Deref<T>> and ...);

      /// Check if all T are either const- or volatile-qualified              
      template<class...T>
      concept Convoluted = sizeof...(T) > 0
          and ((Constant<T> or Volatile<T>) and ...);

      /// Check if all T are not constant-qualified                           
      template<class...T>
      concept Mutable = sizeof...(T) > 0
          and ((not Constant<T>) and ...);

      /// Check if all T are signed                                           
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class...T>
      concept Signed = sizeof...(T) > 0
          and (::std::is_signed_v<Deref<T>> and ...);

      /// Check if all T are unsigned                                         
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class...T>
      concept Unsigned = sizeof...(T) > 0 and ((not Signed<T>) and ...);

      /// Check if all T are booleans                                         
      template<class...T>
      concept BuiltinBool = sizeof...(T) > 0 and Similar<bool, T...>;

      /// Check if all T are character types                                  
      template<class...T>
      concept BuiltinCharacter = sizeof...(T) > 0 and (
            (SimilarAsOneOf<Deref<T>, char, char8_t, char16_t, char32_t, wchar_t>
         ) and ...);

      /// Check if all T are literals - always bounded array with an extent   
      template<class...T>
      concept StringLiteral = sizeof...(T) > 0 and ((CT::Array<T>
           and BuiltinCharacter<Deext<Deref<T>>>
         ) and ...);

      /// Check if all T are string pointers, hopefully null-terminated       
      /// This account to all string pointers that _do not have extents_      
      template<class...T>
      concept StringPointer = sizeof...(T) > 0 and ((CT::Sparse<T>
           and not CT::Array<T>
           and BuiltinCharacter<Deptr<Deref<T>>>
         ) and ...);

      /// Check if all T are either StringLiteral, or StringPointer           
      template<class...T>
      concept String = sizeof...(T) > 0
          and ((StringLiteral<T> or StringPointer<T>) and ...);

      /// Check if all T are standard C++ containers                          
      /// You can get container type using TypeOf, if CT::Typed               
      template<class...T>
      concept StdContainer = sizeof...(T) > 0
          and (::std::ranges::range<T> and ...);

      /// Standard cintiguous container concept                               
      /// You can get container type using TypeOf, if CT::Typed               
      template<class...T>
      concept StdContiguousContainer = sizeof...(T) > 0
          and ((::std::ranges::contiguous_range<T>
            and requires (T c) { {c.data()} -> CT::Sparse; }
            and requires (T c) { {c.size()} -> CT::Exact<::std::size_t>; }
         ) and ...);

      /// Built-in integer number concept                                     
      ///   @attention excludes boolean types and char types                  
      template<class...T>
      concept BuiltinInteger = sizeof...(T) > 0 and ((
            ::std::integral<Deref<T>>
            and not BuiltinBool<T>
            and not BuiltinCharacter<T>
         ) and ...);

      /// Check if type is any signed integer                                 
      template<class...T>
      concept BuiltinSignedInteger = sizeof...(T) > 0
          and BuiltinInteger<T...> and Signed<T...>;

      /// Check if type is any unsigned integer                               
      template<class...T>
      concept BuiltinUnsignedInteger = sizeof...(T) > 0
          and BuiltinInteger<T...> and Unsigned<T...>;

      /// Built-in real number concept                                        
      template<class...T>
      concept BuiltinReal = sizeof...(T) > 0
          and (::std::floating_point<Deref<T>> and ...);

      /// Built-in number concept                                             
      ///   @attention excludes boolean types and char types                  
      template<class...T>
      concept BuiltinNumber = sizeof...(T) > 0
          and ((BuiltinInteger<T> or BuiltinReal<T>) and ...);

      /// Convertible concept                                                 
      /// Explicit conversion is a mess across many compilers, and the        
      /// standard as a whole, so this concept enforces a new world order     
      template<class FROM, class...TO>
      concept Convertible = sizeof...(TO) > 0 and Complete<FROM, TO...>
          and ((requires (Deref<FROM>& from) { Deref<TO> (from); }
             or requires (Deref<FROM>& from) { Deref<TO> (from.operator Deref<TO>()); }
             or requires (Deref<FROM>& from) { Deref<TO> (from.operator Deref<TO>&()); }
             or requires (Deref<FROM>& from) { Deref<TO> (from.operator const Deref<TO>&()); }
             or requires (Deref<TO>& to, Deref<FROM>& from) { to = static_cast<Deref<TO>>(from); }
             or ::std::convertible_to<Deref<FROM>, Deref<TO>>
          ) and ...);

      namespace Inner
      {

         template<class LHS, class RHS>
         consteval bool Comparable() {
            if constexpr (requires (const LHS& lhs, const RHS& rhs) {
               { lhs == rhs } -> Convertible<bool>;
            }) return true;
            else if constexpr (requires (const LHS& lhs, const RHS& rhs) {
               { lhs == static_cast<const LHS&>(rhs) } -> Convertible<bool>;
            }) return true;
            else if constexpr (requires (const LHS& lhs, const RHS& rhs) {
               { static_cast<const RHS&>(lhs) == rhs } -> Convertible<bool>;
            }) return true;
            else return false;
         }

      } // namespace CT::Inner

      /// Equality comparable concept for any origin LHS and RHS, with an     
      /// adequate == operator                                                
      template<class LHS, class...RHS>
      concept Comparable = sizeof...(RHS) > 0 and Complete<LHS, RHS...>
          and (Inner::Comparable<LHS, RHS>() and ...);

      /// Sortable concept for any origin LHS and RHS, with an adequate       
      /// <, > operators, or combined <=> operator                            
      template<class LHS, class...RHS>
      concept Sortable = sizeof...(RHS) > 0 and Complete<LHS, RHS...>
          and requires (LHS& t, RHS&...u) {
             { ((t < u), ...) } -> Convertible<bool>;
             { ((t > u), ...) } -> Convertible<bool>;
          };

      /// Check if all T are an enum types                                    
      template<class...T>
      concept Enum = sizeof...(T) > 0 and Complete<T...>
          and (::std::is_enum_v<Deref<T>> and ...);

      /// Check if all T are fundamental types                                
      /// Examples: int, void, float, nullptr_t, ...                          
      /// Counter examples: int&, int*, any custom type, ref, or pointer      
      template<class...T>
      concept Fundamental = sizeof...(T) > 0 and Complete<T...>
          and (::std::is_fundamental_v<Deref<T>> and ...);

      /// Check if all T are arithmetic types                                 
      /// Examples: bool, char, int, float, size_t                            
      /// Counter examples: std::byte, atomic_int, custom type, ref, or ptr   
      template<class...T>
      concept Arithmetic = sizeof...(T) > 0 and Complete<T...>
          and (::std::is_arithmetic_v<Deref<T>> and ...);
      
      /// Check if all T are referencable                                     
      /// All must have a Reference(Count) -> Count method                    
      template<class...T>
      concept Referencable = sizeof...(T) > 0 and Complete<T...>
          and requires (T&...a) {
            { (a.Reference(-1), ...) } -> Exact<Count>;
          };
               
      /// Check if the origin T is swappable                                  
      template<class...T>
      concept Swappable = sizeof...(T) > 0 and Complete<T...>
          and (::std::is_swappable_v<T> and ...);

      template<class...T>
      concept SwappableNoexcept = sizeof...(T) > 0 and Complete<T...>
          and (::std::is_nothrow_swappable_v<T> and ...);

      /// Check if the origin T inherits BASE                                 
      template<class T, class...BASE>
      concept DerivedFrom = sizeof...(BASE) > 0 and Complete<Decay<T>>
          and (::std::derived_from<Decay<T>, Decay<BASE>> and ...);
   
      /// Check if T1 is somehow related to all of the provided types         
      template<class T1, class...TN>
      concept Related = sizeof...(TN) > 0
          and ((DerivedFrom<T1, TN> or DerivedFrom<TN, T1>) and ...);

      /// Check if a type is virtually derived from all the provided BASE(s)  
      template<class T, class...BASE>
      concept VirtuallyDerivedFrom = sizeof...(BASE) > 0 and Complete<Decay<T>>
          and ((::std::is_base_of_v<Decay<BASE>, Decay<T>>
            and not requires (Decay<BASE>* from) { static_cast<Decay<T>*>(from); }
          ) and ...);

      /// Binary compatibility check between T1 and the provided TN           
      template<class T1, class...TN>
      concept BinaryCompatible = sizeof...(TN) > 0 and ((
            Similar<T1, TN> or (Related<T1, TN> and sizeof(T1) == sizeof(TN))
         ) and ...);

      /// Check if all T are reference types                                  
      template<class...T>
      concept Reference = sizeof...(T) > 0
          and (::std::is_reference_v<T> and ...);

      /// Check if all T are not reference types                              
      template<class...T>
      concept NotReference = sizeof...(T) > 0
          and ((not ::std::is_reference_v<T>) and ...);

      /// True if T is not a pointer (and has no extent with []), as well as  
      /// not a reference                                                     
      template<class...T>
      concept Slab = sizeof...(T) > 0
          and ((Dense<T> and not Reference<T>) and ...);

      /// Check if types have no reference/pointer/extent/const/volatile      
      template<class...T>
      concept Decayed = sizeof...(T) > 0 and Dense<T...>
          and ((not Reference<T> and not Convoluted<T>) and ...);
   
      /// Check if types have reference/pointer/extent/const/volatile         
      template<class...T>
      concept NotDecayed = sizeof...(T) > 0
          and ((not Decayed<T>) and ...);
   
      /// Check if a function encapsulated in a lambda is a constexpr         
      /// Leverages that lambda expressions can be constexpr as of C++17      
      /// https://stackoverflow.com/questions/55288555                        
      template<class Lambda, int = (Lambda {}(), 0)>
      consteval bool IsConstexpr(Lambda) { return true; }
      consteval bool IsConstexpr(...) { return false; }
         
      /// Check if type is a dense void                                       
      template<class...T>
      concept Void = sizeof...(T) > 0 and (::std::is_void_v<T> and ...);

      /// Check if type-erased                                                
      template<class...T>
      concept TypeErased = sizeof...(T) > 0 and Void<T...>;

      /// A data type is any type that is not a dense void                    
      template<class...T>
      concept Data = sizeof...(T) > 0 and ((not Void<T>) and ...);
      
      /// Dense data concept                                                  
      template<class...T>
      concept DenseData = sizeof...(T) > 0 and Dense<T...> and Data<T...>
          and ((not Reference<T>) and ...);
      
      /// Sparse data concept                                                 
      template<class...T>
      concept SparseData = sizeof...(T) > 0 and Sparse<T...> and Data<T...>
          and ((not Reference<T>) and ...);
      
      /// Data reference concept                                              
      template<class...T>
      concept DataReference = sizeof...(T) > 0
          and Data<T...> and Reference<T...>;
      
      /// Check if all provided types match std::nullptr_t exactly            
      template<class...T>
      concept Nullptr = sizeof...(T) > 0
          and Exact<::std::nullptr_t, T...>;

      namespace Inner
      {

         /// Nested type qualifier stripper                                   
         template<class T>
         consteval auto NestedDecvq() {
            using Stripped = Decvq<Deref<T>>;
            if constexpr (Decayed<Stripped>)
               return (Stripped*) nullptr;
            else if constexpr (Array<Stripped>)
               return (Deptr<decltype(NestedDecvq<Deext<Stripped>>())> (*) [::std::extent_v<Stripped>]) nullptr;
            else if constexpr (Sparse<Stripped>)
               return (decltype(NestedDecvq<Deptr<Stripped>>())*) nullptr;
            else
               LANGULUS_ERROR("Shouldn't be possible");
         }

      } // namespace Langulus::CT::Inner

   } // namespace Langulus::CT

   /// Pick between two types, based on a condition                           
   template<bool CONDITION, class TRUETYPE, class FALSETYPE>
   using Conditional = ::std::conditional_t<CONDITION, TRUETYPE, FALSETYPE>;

   /// Get the extent of an array, or 1 if T is not an array                  
   template<class T>
   constexpr Count ExtentOf = CT::Array<T> ? ::std::extent_v<Deref<T>> : 1;

   /// Strip all qualifiers on all levels of indirection of a type            
   /// const volatile void * const * const becomes void**                     
   /// This strongly guarantees, that it strips EVERYTHING, including nested  
   /// pointer/array constness/volatileness, etc.                             
   template<class T>
   using DecvqAll = Deptr<decltype(CT::Inner::NestedDecvq<T>())>;

   /// Strip all const/volatile qualifiers on all levels of indirection       
   /// In a sense, it's just a nested const_cast                              
   /// (this version preserves a reference, if any)                           
   LANGULUS(ALWAYS_INLINED)
   constexpr decltype(auto) DecvqCast(auto& a) noexcept {
      return const_cast<DecvqAll<decltype(a)>&>(a);
   }

   /// Strip all const/volatile qualifiers on all levels of indirection       
   /// In a sense, it's just a nested const_cast                              
   LANGULUS(ALWAYS_INLINED)
   constexpr decltype(auto) DecvqCast(CT::Sparse auto a) noexcept {
      return const_cast<DecvqAll<decltype(a)>>(a);
   }

   /// Returns true if the provided arguments are of similar types            
   template<class T1, class...TN>
   consteval bool SimilarTypes(T1&&, TN&&...) {
      return CT::Similar<T1, TN...>;
   }

   /// Returns true if the provided arguments are of exactly the same types   
   template<class T1, class...TN>
   consteval bool ExactTypes(T1&&, TN&&...) {
      return CT::Exact<T1, TN...>;
   }

   /// Returns true if the provided arguments have the same origin types      
   template<class T1, class...TN>
   consteval bool SameTypes(T1&&, TN&&...) {
      return CT::Same<T1, TN...>;
   }

} // namespace Langulus

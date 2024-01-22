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

   #if LANGULUS(DEBUG)
      #define LANGULUS_INLINED() inline
   #else
      /// Force always inlining - significantly increases build time!         
      #define LANGULUS_INLINED() __forceinline
   #endif
#else
   /// Force no inlining                                                      
   #define LANGULUS_NOINLINE() __attribute__((noinline))

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
      struct StaticVerb;

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

   namespace Anyness
   {
      struct Block;
      class Neat;
   }

   /// Type for counting things, depends on architecture                      
   using Count = ::std::size_t;
   constexpr Count CountMax = ::std::numeric_limits<Count>::max();

   /// Type for counting bytes, depends on architecture                       
   struct Size {
      ::std::size_t mSize;

      constexpr Size() noexcept = default;
      constexpr Size(const Size&) noexcept = default;
      constexpr Size(Size&&) noexcept = default;
      constexpr Size(::std::size_t v) noexcept : mSize {v} {}
      constexpr operator ::std::size_t() const noexcept { return mSize; }
      constexpr Size& operator = (const Size&) noexcept = default;
      constexpr Size& operator = (Size&&) noexcept = default;

      constexpr ::std::string_view GetSuffix() const noexcept {
         if      (mSize < 1'000LL)                 return "B";
         else if (mSize < 1'000'000LL)             return "KB";
         else if (mSize < 1'000'000'000LL)         return "MB";
         else if (mSize < 1'000'000'000'000LL)     return "GB";
         else if (mSize < 1'000'000'000'000'000LL) return "TB";
         else return " PB";
      }
   };

   constexpr Size SizeMax = ::std::numeric_limits<Size>::max();

   /// Byte count literal                                                     
   constexpr Size operator"" _B(unsigned long long int num) noexcept {
      return Size {num};
   }

	/// Floating-point bytesize literals, like 5.5_kB                          
   constexpr Size operator"" _kiB(long double num) noexcept {
      return Size {(size_t)((1LL << 10) * num)};
   }
   constexpr Size operator"" _MiB(long double num) noexcept {
      return Size {(size_t)((1LL << 20) * num)};
   }
   constexpr Size operator"" _GiB(long double num) noexcept {
      return Size {(size_t)((1LL << 30) * num)};
   }
   constexpr Size operator"" _TiB(long double num) noexcept {
      return Size {(size_t)((1LL << 40) * num)};
   }
   constexpr Size operator"" _PiB(long double num) noexcept {
      return Size {(size_t)((1LL << 50) * num)};
   }
   constexpr Size operator"" _kB(long double num) noexcept {
      return Size {(size_t)(1'000LL * num)};
   }
   constexpr Size operator"" _MB(long double num) noexcept {
      return Size {(size_t)(1'000'000LL * num)};
   }
   constexpr Size operator"" _GB(long double num) noexcept {
      return Size {(size_t)(1'000'000'000LL * num)};
   }
   constexpr Size operator"" _TB(long double num) noexcept {
      return Size {(size_t)(1'000'000'000'000LL * num)};
   }
   constexpr Size operator"" _PB(long double num) noexcept {
      return Size {(size_t)(1'000'000'000'000'000LL * num)};
   }

	/// Integer bytesize literals, like 5_kB                                   
   constexpr Size operator"" _kiB(unsigned long long int num) noexcept {
      return Size {(size_t)((1LL << 10) * num)};
   }
   constexpr Size operator"" _MiB(unsigned long long int num) noexcept {
      return Size {(size_t)((1LL << 20) * num)};
   }
   constexpr Size operator"" _GiB(unsigned long long int num) noexcept {
      return Size {(size_t)((1LL << 30) * num)};
   }
   constexpr Size operator"" _TiB(unsigned long long int num) noexcept {
      return Size {(size_t)((1LL << 40) * num)};
   }
   constexpr Size operator"" _PiB(unsigned long long int num) noexcept {
      return Size {(size_t)((1LL << 50) * num)};
   }
   constexpr Size operator"" _kB(unsigned long long int num) noexcept {
      return Size {(size_t)(1'000LL * num)};
   }
   constexpr Size operator"" _MB(unsigned long long int num) noexcept {
      return Size {(size_t)(1'000'000LL * num)};
   }
   constexpr Size operator"" _GB(unsigned long long int num) noexcept {
      return Size {(size_t)(1'000'000'000LL * num)};
   }
   constexpr Size operator"" _TB(unsigned long long int num) noexcept {
      return Size {(size_t)(1'000'000'000'000LL * num)};
   }
   constexpr Size operator"" _PB(unsigned long long int num) noexcept {
      return Size {(size_t)(1'000'000'000'000'000LL * num)};
   }

   /// Type for offsetting pointers, depends on architecture                  
   using Offset = ::std::size_t;
   constexpr Offset OffsetMax = ::std::numeric_limits<Offset>::max();

   /// Type that holds a hash, depends on architecture                        
   struct Hash {
      static constexpr bool CTTI_POD = true;
      static constexpr bool CTTI_Nullifiable = true;

      ::std::size_t mHash {};

      LANGULUS(INLINED)
      explicit constexpr operator bool() const noexcept {
         return mHash != 0;
      }

      constexpr Hash() noexcept = default;
      constexpr Hash(const Hash&) noexcept = default;
      constexpr Hash(::std::size_t a) noexcept
         : mHash {a} {}

      constexpr bool operator == (const Hash&) const noexcept = default;
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

   LANGULUS(INLINED)
   constexpr Real operator "" _real(unsigned long long n) noexcept {
      return static_cast<Real>(n);
   }
   
   LANGULUS(INLINED)
   constexpr Real operator "" _real(long double n) noexcept {
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
   constexpr Size Alignment = LANGULUS_ALIGNMENT;
   #define LANGULUS_ALIGNED() alignas(::Langulus::Alignment)

   /// The bitness                                                            
   constexpr Size Bitness = LANGULUS(BITNESS);

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
      NOD() constexpr auto NestedDecay() noexcept {
         using Stripped = Decvq<Deptr<Deext<T>>>;
         if constexpr (::std::same_as<T, Stripped>)
            return static_cast<Stripped*>(nullptr);
         else
            return NestedDecay<Stripped>();
      }

      template<class T1, class T2>
      NOD() constexpr bool NestedSimilar() noexcept {
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

      /// Check if T is complete (defined), by exploiting sizeof              
      /// Usefulness of this is limited to the first instantiation, and       
      /// that is how it is used upon reflection by RTTI. Any other use is    
      /// undefined and might produce wrong results on some compilers.        
      /// Thankfully, most modern compilers do detect, if a definition        
      /// changes between completeness checks, so it is unlikely to cause any 
      /// real harm                                                           
      /// https://stackoverflow.com/questions/21119281                        
      template<class...T>
      concept Complete = ((sizeof(T) == sizeof(T)) and ...);

      /// True if decayed T1 matches all decayed types                        
      ///   @attention ignores type density and cv-qualifications             
      template<class T1, class T2, class...TN>
      concept Same = ::std::same_as<Decay<T1>, Decay<T2>>
          and ((::std::same_as<Decay<T1>, Decay<TN>>) and ...);

      /// True if unqualified T1 matches all unqualified types                
      ///   @attention ignores cv-qualifications only                         
      template<class T1, class T2, class...TN>
      concept Similar = Langulus::Inner::NestedSimilar<T1, T2>()
          and (Langulus::Inner::NestedSimilar<T1, TN>() and ...);

      /// True if T1 matches exactly all the provided types, including        
      /// density and cv-qualifiers                                           
      template<class T1, class T2, class...TN>
      concept Exact = ::std::same_as<T1, T2>
          and ((::std::same_as<T1, TN>) and ...);

      /// True if decayed T1 matches at least one of the decayed types        
      ///   @attention ignores type density and cv-qualifications             
      template<class T1, class T2, class...TN>
      concept SameAsOneOf = ::std::same_as<Decay<T1>, Decay<T2>> 
           or ((::std::same_as<Decay<T1>, Decay<TN>>) or ...);

      /// True if unqualified T1 matches at least one of the unqualified types
      ///   @attention ignores cv-qualifications only                         
      template<class T1, class T2, class...TN>
      concept SimilarAsOneOf = Langulus::Inner::NestedSimilar<T1, T2>()
           or (Langulus::Inner::NestedSimilar<T1, TN>() or ...);

      /// True if T1 matches exactly at least one of the types, including     
      /// density and cv-qualifications                                       
      template<class T1, class T2, class...TN>
      concept ExactAsOneOf = ::std::same_as<T1, T2>
           or ((::std::same_as<T1, TN>) or ...);

      /// Check if any T is the built-in one that signifies lack of support   
      template<class...T>
      concept Unsupported = (Same<::Langulus::Inner::Unsupported, T> or ...);

      /// Check if all T are supported                                        
      template<class...T>
      concept Supported = (not Same<::Langulus::Inner::Unsupported, T> and ...);

      /// True if T is an array (has an extent with [])                       
      /// Sometimes a reference hides the pointer/extent, hence the deref     
      template<class...T>
      concept Array = (::std::is_bounded_array_v<Deref<T>> and ...);

      /// True if T is a pointer (or has an extent with [])                   
      /// Sometimes a reference hides the pointer/extent, hence the deref     
      template<class...T>
      concept Sparse = ((::std::is_pointer_v<Deref<T>> or Array<T>) and ...);

      /// True if T is not a pointer (and has no extent with [])              
      template<class...T>
      concept Dense = not Sparse<T...>;

      /// Check if type is constant-qualified                                 
      template<class...T>
      concept Constant = (::std::is_const_v<Deref<T>> and ...);

      /// Check if type is volatile-qualified                                 
      template<class...T>
      concept Volatile = (::std::is_volatile_v<Deref<T>> and ...);

      /// Check if type is either const- or volatile-qualified                
      template<class...T>
      concept Convoluted = ((Constant<T> or Volatile<T>) and ...);

      /// Check if type is not constant-qualified                             
      template<class...T>
      concept Mutable = not Constant<T...>;

      /// Check if type is signed (either sparse or dense)                    
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class...T>
      concept Signed = (::std::is_signed_v<Decay<T>> and ...);

      /// Check if type is signed and dense                                   
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class...T>
      concept DenseSigned = Signed<T...> and Dense<T...>;

      /// Check if type is signed and sparse                                  
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class...T>
      concept SparseSigned = Signed<T...> and Sparse<T...>;

      /// Check if type is unsigned (either sparse or dense)                  
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class...T>
      concept Unsigned = not Signed<T...>;

      /// Check if type is unsigned and dense                                 
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class...T>
      concept DenseUnsigned = Unsigned<T...> and Dense<T...>;

      /// Check if type is unsigned and sparse                                
      ///   @attention doesn't apply to numbers only, but anything negatable  
      template<class...T>
      concept SparseUnsigned = Unsigned<T...> and Sparse<T...>;

      /// Built-in boolean concept (either sparse or dense)                   
      template<class T1, class...TN>
      concept BuiltinBool = Same<bool, T1, TN...>;

      /// Built-in character concept (either sparse or dense)                 
      template<class...T>
      concept BuiltinCharacter = (
            (SameAsOneOf<T, char, char8_t, char16_t, char32_t, wchar_t>
         ) and ...);

      /// String literal concept                                              
      template<class...T>
      concept StringLiteral = ((CT::Array<T>
           and CT::SimilarAsOneOf<Deext<Deref<T>>, char, char8_t, char16_t, char32_t, wchar_t>
         ) and ...);

      /// String pointer concept                                              
      template<class...T>
      concept StringPointer = ((CT::Sparse<T>
           and CT::SimilarAsOneOf<Deptr<Deref<T>>, char, char8_t, char16_t, char32_t, wchar_t>
         ) and ...);

      /// String pointer concept                                              
      template<class...T>
      concept String = ((StringLiteral<T> or StringPointer<T>) and ...);

      /// Standard container concept                                          
      /// You can get container type using TypeOf, if CT::Typed               
      template<class...T>
      concept StdContainer = (::std::ranges::range<T> and ...);

      /// Standard cintiguous container concept                               
      /// You can get container type using TypeOf, if CT::Typed               
      template<class...T>
      concept StdContiguousContainer = ((::std::ranges::contiguous_range<T>
            and requires (T c) { {c.data()} -> CT::Sparse; }
            and requires (T c) { {c.size()} -> CT::Exact<::std::size_t>; }
         ) and ...);

      /// Built-in integer number concept (either sparse or dense)            
      ///   @attention excludes boolean types and char types                  
      template<class...T>
      concept BuiltinInteger = ((::std::integral<Decay<T>>
            and not BuiltinBool<T> and not BuiltinCharacter<T>
         ) and ...);

      /// Check if type is any signed integer (either sparse or dense)			
      template<class...T>
      concept BuiltinSignedInteger = BuiltinInteger<T...> and Signed<T...>;

      /// Check if type is any unsigned integer (either sparse or dense)		
      template<class...T>
      concept BuiltinUnsignedInteger = BuiltinInteger<T...> and Unsigned<T...>;

      /// Built-in real number concept (either sparse or dense)               
      template<class...T>
      concept BuiltinReal = (::std::floating_point<Decay<T>> and ...);

      /// Built-in number concept (either sparse or dense)                    
      ///   @attention excludes boolean types and char types                  
      template<class...T>
      concept BuiltinNumber = ((BuiltinInteger<T> or BuiltinReal<T>) and ...);

      /// Dense built-in number concept                                       
      ///   @attention excludes boolean types and char types                  
      template<class...T>
      concept DenseBuiltinNumber = BuiltinNumber<T...> and Dense<T...>;

      /// Sparse built-in number concept                                      
      ///   @attention excludes boolean types and char types                  
      template<class...T>
      concept SparseBuiltinNumber = BuiltinNumber<T...> and Sparse<T...>;

      namespace Inner
      {

         /// Explicit conversion is a mess across many compilers, and the     
         /// standard as a whole, so this concept enforces a new world order  
         template<class FROM, class TO>
         concept Convertible = requires (FROM& from) { TO {from}; }
                            or requires (FROM& from) { TO {from.operator TO()}; }
                            or requires (FROM& from) { static_cast<TO>(from); }
                            or ::std::convertible_to<FROM, TO>;

         template<class LHS, class RHS = LHS>
         concept Sortable = requires (LHS& t, RHS& u) {
               {t < u} -> Convertible<bool>;
               {t > u} -> Convertible<bool>;
            };

         template<class LHS, class RHS = LHS>
         concept Comparable = requires (LHS& lhs, RHS& rhs) {
               {lhs == rhs} -> Convertible<bool>;
            };

         template<class...T>
         concept Enum = (::std::is_enum_v<T> and ...);

         template<class...T>
         concept Fundamental = (::std::is_fundamental_v<T> and ...);

         template<class...T>
         concept Arithmetic = (::std::is_arithmetic_v<T> and ...);

         template<class...T>
         concept Referencable = requires (T&...a) {
               {(a.Keep(), ...)};
               {(a.Free(), ...)} -> Exact<Count>;
               {(a.GetReferences(), ...)} -> Exact<Count>;
            };

         template<class...T>
         concept Swappable = (::std::is_swappable_v<T> and ...);

         template<class...T>
         concept SwappableNoexcept = (::std::is_nothrow_swappable_v<T> and ...);

         template<class T, class BASE>
         concept DerivedFrom = ::std::derived_from<T, BASE>;

         template<class T1, class T2>
         concept Related = DerivedFrom<T1, T2> or DerivedFrom<T2, T1>;

         template<class T1, class T2>
         concept BinaryCompatible = Similar<T1, T2>
              or (Related<T1, T2> and sizeof(T1) == sizeof(T2));

      } // namespace Langulus::CT::Inner

      /// Sortable concept for any origin LHS and RHS, with an adequate       
      /// <, > operators, or combined <=> operator                            
      template<class LHS, class...RHS>
      concept Sortable = Complete<Decay<LHS>, Decay<RHS>...>
          and (Inner::Sortable<Decay<LHS>, Decay<RHS>> and ...);

      /// Equality comparable concept for any origin LHS and RHS, with an     
      /// adequate == operator                                                
      template<class LHS, class...RHS>
      concept Comparable = Complete<Decay<LHS>, Decay<RHS>...>
          and (Inner::Comparable<Decay<LHS>, Decay<RHS>> and ...);

      /// Convertible concept                                                 
      /// Checks if a static_cast is possible between the provided types      
      template<class FROM, class...TO>
      concept Convertible = Complete<Decay<FROM>, Decay<TO>...>
          and (Inner::Convertible<Decay<FROM>, Decay<TO>> and ...);

      /// Check if the origin T is an enum type                               
      template<class...T>
      concept Enum = Complete<Decay<T>...>
          and Inner::Enum<Decay<T>...>;

      /// Check if the origin T is a fundamental type                         
      template<class...T>
      concept Fundamental = Complete<Decay<T>...>
          and Inner::Fundamental<Decay<T>...>;

      /// Check if the origin T is an arithmetic type                         
      template<class...T>
      concept Arithmetic = Complete<Decay<T>...>
          and Inner::Arithmetic<Decay<T>...>;
      
      /// Check if the origin T is referencable                               
      template<class...T>
      concept Referencable = Complete<Decay<T>...>
          and Inner::Referencable<Decay<T>...>;
               
      /// Check if the origin T is swappable                                  
      template<class...T>
      concept Swappable = Complete<Decay<T>...>
          and Inner::Swappable<Decay<T>...>;

      template<class...T>
      concept SwappableNoexcept = Complete<Decay<T>...>
          and Inner::SwappableNoexcept<Decay<T>...>;

      /// Check if the origin T inherits BASE                                 
      template<class T, class...BASE>
      concept DerivedFrom = Complete<Decay<T>>
          and (Inner::DerivedFrom<Decay<T>, Decay<BASE>> and ...);
   
      /// Check if T1 is somehow related to all of the provided types         
      template<class T1, class T2, class...TN>
      concept Related = Inner::Related<Decay<T1>, Decay<T2>>
          and (Inner::Related<Decay<T1>, Decay<TN>> and ...);
   
      /// Binary compatibility check between T1 and the provided types        
      template<class T1, class T2, class...TN>
      concept BinaryCompatible = Inner::BinaryCompatible<Decay<T1>, Decay<T2>>
          and (Inner::BinaryCompatible<Decay<T1>, Decay<TN>> and ...);

      /// Reference concept                                                   
      template<class...T>
      concept Reference = (::std::is_reference_v<T> and ...);

      /// Check if types have no reference/pointer/extent/const/volatile      
      template<class...T>
      concept Decayed = Dense<T...> and not Reference<T...> and not Convoluted<T...>;
   
      /// Check if types have reference/pointer/extent/const/volatile         
      template<class...T>
      concept NotDecayed = not Decayed<T...>;
   
      /// Check if a function encapsulated in a lambda is a constexpr         
      /// Leverages that lambda expressions can be constexpr as of C++17      
      /// https://stackoverflow.com/questions/55288555                        
      template<class Lambda, int = (Lambda {}(), 0)>
      constexpr bool IsConstexpr(Lambda) { return true; }
      constexpr bool IsConstexpr(...) { return false; }
         
      /// Check if type is a dense void                                       
      template<class...T>
      concept Void = (::std::is_void_v<T> and ...);

      /// Check if type-erased                                                
      template<class...T>
      concept TypeErased = Void<T...>;

      /// A data type is any type that is not a dense void                    
      template<class...T>
      concept Data = not Void<T...>;
      
      /// Dense data concept                                                  
      template<class...T>
      concept DenseData = Dense<T...> and Data<T...> and not Reference<T...>;
      
      /// Sparse data concept                                                 
      template<class...T>
      concept SparseData = Sparse<T...> and Data<T...> and not Reference<T...>;
      
      /// Data reference concept                                              
      template<class...T>
      concept DataReference = Data<T...> and Reference<T...>;
      
      /// Check if all provided types match std::nullptr_t exactly            
      template<class T1, class...TN>
      concept Nullptr = Exact<::std::nullptr_t, T1, TN...>;

      namespace Inner
      {

         template<class T>
         NOD() constexpr auto NestedDecvq() noexcept {
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

} // namespace Langulus

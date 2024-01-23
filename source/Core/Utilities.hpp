///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Common.hpp"


namespace Langulus
{

   /// Similar to std::isalpha, but constexpr                                 
   NOD() LANGULUS(INLINED)
   constexpr Letter IsAlpha(const Letter a) noexcept {
      return (a >= 'a' and a <= 'z')
          or (a >= 'A' and a <= 'Z');
   }

   /// Similar to std::isspace, but constexpr                                 
   NOD() LANGULUS(INLINED)
   constexpr Letter IsSpace(const Letter a) noexcept {
      return a == ' '
          or a == '\t'
          or a == '\n'
          or a == '\r'
          or a == '\f'
          or a == '\v';
   }
   
   /// Similar to std::isdigit, but constexpr                                 
   NOD() LANGULUS(INLINED)
   constexpr Letter IsDigit(const Letter a) noexcept {
      return a >= '0'
         and a <= '9';
   }

   /// Forward lvalue as either lvalue or rvalue                              
   /// Same as ::std::forward, but avoid writing the namespace                
   template<class T> NOD() LANGULUS(INLINED)
   constexpr T&& Forward(Deref<T>& a) noexcept {
      return static_cast<T&&>(a);
   }

   template<class T> NOD() LANGULUS(INLINED)
   constexpr T&& Forward(Deref<T>&& a) noexcept {
      static_assert(not ::std::is_lvalue_reference_v<T>, "Bad forward call");
      return static_cast<T&&>(a);
   }
   
   /// Get number of digits inside an integer                                 
   /// The routine is as statically optimized as possible                     
   ///   @param n - value                                                     
   ///   @return the number of digits inside the value                        
   template<class T, T OFFSET = 10, Count RESULT = 1> NOD() LANGULUS(INLINED)
   constexpr Count DigitsOf(const T& n) noexcept {
      if constexpr (::std::numeric_limits<T>::digits10 + 1 > RESULT) {
         if constexpr (CT::Signed<T>) {
            // Get rid of negatives                                     
            using UnsignedT = ::std::make_unsigned_t<T>;
            return DigitsOf<UnsignedT, OFFSET, RESULT>(
               static_cast<UnsignedT>(::std::abs(n))
            );
         }
         else {
            if (n < OFFSET) {
               // Early exit                                            
               return RESULT;
            }

            // Next digit                                               
            return DigitsOf<T, OFFSET * 10, RESULT + 1>(n);
         }
      }
      else {
         // Reached the numeric limit                                   
         return RESULT;
      }
   }

   /// Check if an unsigned integer is a power of two                         
   ///   @param n - the number to test                                        
   ///   @return true if number has exactly one bit set                       
   NOD() LANGULUS(INLINED)
   constexpr bool IsPowerOfTwo(const CT::Unsigned auto& n) noexcept {
      return ::std::has_single_bit(n);
   }

   /// Returns the number of consecutive 0 bits in the value of x, starting	
   /// from the least significant 'right' bit                                 
   ///   @param x - the value to scan                                         
   ///   @return the number of consecutive zero bits                          
   NOD() LANGULUS(INLINED)
   constexpr int CountTrailingZeroes(const CT::Unsigned auto& x) noexcept {
      return ::std::countr_zero(x);
   }

   /// Returns the number of consecutive 0 bits in the value of x, starting   
   /// from the most significant 'left' bit                                   
   ///   @param x - the value to scan                                         
   ///   @return the number of consecutive zero bits                          
   NOD() LANGULUS(INLINED)
   constexpr int CountLeadingZeroes(const CT::Unsigned auto& x) noexcept {
      return ::std::countl_zero(x);
   }

   /// Round to the upper power-of-two                                        
   ///   @tparam SAFE - set to true if you want it to throw on overflow       
   ///   @tparam T - the unsigned integer type (deducible)                    
   ///   @param x - the unsigned integer to round up                          
   ///   @return the closest upper power-of-two to x                          
   template<bool SAFE = false, CT::Unsigned T> NOD() LANGULUS(INLINED)
   constexpr T Roof2(const T& x) noexcept(not SAFE) {
      if constexpr (SAFE) {
         constexpr T lastPowerOfTwo = (T {1}) << (T {sizeof(T) * 8 - 1});
         if (x > lastPowerOfTwo)
            throw Except::Overflow("Roof2 overflowed");
      }

      return x <= 1 ? x : static_cast<T>(::std::size_t {1} <<
         (sizeof(::std::size_t) * 8 - CountLeadingZeroes<::std::size_t>(x - 1))
      );
   }

   /// Round to the upper power-of-two (constexpr variant)                    
   ///   @tparam SAFE - set to true if you want it to throw on overflow       
   ///   @tparam T - the unsigned integer type (deducible)                    
   ///   @param x - the unsigned integer to round up                          
   ///   @return the closest upper power-of-two to x                          
   template<bool SAFE = false, CT::Unsigned T> NOD() LANGULUS(INLINED)
   constexpr T Roof2cexpr(const T& x) noexcept(not SAFE) {
      //TODO can be detected at compile time, use one combined func
      if constexpr (SAFE) {
         constexpr T lastPowerOfTwo = (T {1}) << (T {sizeof(T) * 8 - 1});
         if (x > lastPowerOfTwo)
            throw Except::Overflow("Roof2 overflowed");
      }

      T n = x;
      --n;
      n |= n >> 1;
      n |= n >> 2;
      n |= n >> 4;
      if constexpr (sizeof(T) > 1)
         n |= n >> 8;
      if constexpr (sizeof(T) > 2)
         n |= n >> 16;
      if constexpr (sizeof(T) > 4)
         n |= n >> 32;
      if constexpr (sizeof(T) > 8)
         throw Except::ToDo("Implement if struck");

      ++n;
      return n;
   }

   /// A somewhat safer reinterpret_cast for dense instances                  
   ///   @param what - reference to reinterpret                               
   ///   @return the result of reinterpret_cast<TO&>                          
   template<CT::Dense TO, CT::Dense FROM> NOD() LANGULUS(INLINED)
   constexpr Decay<TO>& ReinterpretCast(FROM& what) noexcept {
      static_assert(sizeof(Decay<TO>) == sizeof(Decay<FROM>),
         "Size mismatch on a reference reinterpret_cast");
      return reinterpret_cast<Decay<TO>&>(what);
   }

   /// A somewhat safer reinterpret_cast for dense instances (const)          
   ///   @param what - reference to reinterpret                               
   ///   @return the result of reinterpret_cast<const TO&>                    
   template<CT::Dense TO, CT::Dense FROM> NOD() LANGULUS(INLINED)
   constexpr const Decay<TO>& ReinterpretCast(const FROM& what) noexcept {
      static_assert(sizeof(Decay<TO>) == sizeof(Decay<FROM>),
         "Size mismatch on a reference reinterpret_cast");
      return reinterpret_cast<const Decay<TO>&>(what);
   }

   /// A somewhat safer reinterpret_cast for sparse instances                 
   ///   @param what - what to reinterpret                                    
   ///   @return the result of reinterpret_cast<TO*>                          
   template<CT::Dense TO, CT::Dense FROM> NOD() LANGULUS(INLINED)
   constexpr Decay<TO>* ReinterpretCast(FROM* what) noexcept {
      static_assert(sizeof(Decay<TO>) == sizeof(Decay<FROM>),
         "Size mismatch on a pointer reinterpret_cast");
      return reinterpret_cast<Decay<TO>*>(what);
   }

   /// A somewhat safer reinterpret_cast for sparse instances (const)         
   ///   @param what - what to reinterpret                                    
   ///   @return the result of reinterpret_cast<TO*>                          
   template<CT::Dense TO, CT::Dense FROM> NOD() LANGULUS(INLINED)
   constexpr const Decay<TO>* ReinterpretCast(const FROM* what) noexcept {
      static_assert(sizeof(Decay<TO>) == sizeof(Decay<FROM>),
         "Size mismatch on a pointer reinterpret_cast");
      return reinterpret_cast<const Decay<TO>*>(what);
   }

   /// Always returns a pointer to the argument                               
   template<class T> NOD() LANGULUS(INLINED)
   constexpr decltype(auto) SparseCast(T& a) noexcept {
      if constexpr (CT::Sparse<T>)
         return a;
      else
         return &a;
   }

   /// Always returns a pointer to the argument (const)                       
   template<class T> NOD() LANGULUS(INLINED)
   constexpr decltype(auto) SparseCast(const T& a) noexcept {
      if constexpr (CT::Sparse<T>)
         return a;
      else
         return &a;
   }

   /// Always returns a value reference to the argument                       
   /// If argument is an array, return a value reference to the first element 
   template<class T>
   NOD() constexpr decltype(auto) DenseCast(T& a) {
      if constexpr (CT::Array<T>)
         return DenseCast(a[0]);
      else if constexpr (CT::Sparse<T>) {
         if (a == nullptr)
            throw Except::Access("Can't dereference nullptr");

         return DenseCast(*a);
      }
      else return (a);
   }

   /// Always returns a value reference to the argument (const)               
   /// If argument is an array, return a value reference to the first element 
   template<class T>
   NOD() constexpr decltype(auto) DenseCast(const T& a) {
      if constexpr (CT::Array<T>)
         return DenseCast(a[0]);
      else if constexpr (CT::Sparse<T>) {
         if (a == nullptr)
            throw Except::Access("Can't dereference nullptr");

         return DenseCast(*a);
      }
      else return (a);
   }
   
   /// Always returns a mutable value reference to the argument               
   /// If argument is an array, return a value reference to the first element 
   template<class T>
   NOD() constexpr decltype(auto) DenseCastMutable(const T& a) {
      if constexpr (CT::Array<T>)
         return DenseCastMutable(a[0]);
      else if constexpr (CT::Sparse<T>) {
         if (a == nullptr)
            throw Except::Access("Can't dereference nullptr");

         return DenseCastMutable(*a);
      }
      else return const_cast<Decay<T>&>(a);
   }

   /// Align a value to an alignment                                          
   ///   @param v - the value to align                                        
   ///   @param alignment - the alignment to use                              
   ///   @return the aligned value                                            
   template<class T> NOD() LANGULUS(INLINED)
   constexpr T Align(const T& v, const T& alignment) noexcept {
      const auto r = v % alignment;
      return r > 0 ? v + (alignment - r) : v;
   }
   
} // namespace Langulus

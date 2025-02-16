///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "CountOf.hpp"


namespace Langulus
{

   ///                                                                        
   ///   A byte                                                               
   ///                                                                        
   ///   std::byte is shitty, this one's better. It preserves arithmetic      
   /// operations on the byte. These operations counteract integer promotion, 
   /// the result is always truncated back down to a byte.                    
   ///                                                                        
   #pragma pack(push, 1)
   struct Byte {
      using Type = ::std::uint8_t;
      using CTTI_InnerType = Type;
      static constexpr bool CTTI_POD = true;
      static constexpr bool CTTI_Nullifiable = true;

      Type mValue {};

   public:
      constexpr Byte() noexcept = default;
      constexpr Byte(const Byte&) noexcept = default;
      constexpr Byte(Byte&&) noexcept = default;

      LANGULUS(INLINED)
      constexpr Byte(const Type& a) noexcept
         : mValue {a} {}

      Byte& operator = (const Byte&) noexcept = default;
      Byte& operator = (Byte&&) noexcept = default;

      LANGULUS(INLINED)
      Byte& operator = (const Type& a) noexcept {
         mValue = a;
         return *this;
      }

      LANGULUS(INLINED)
      constexpr explicit operator Type& () const noexcept {
         return const_cast<Type&>(mValue);
      }

      template<CT::BuiltinNumber T> LANGULUS(INLINED)
      constexpr explicit operator T () const noexcept
      requires (CT::Dense<T> and not CT::Same<T, Type>) {
         return static_cast<T>(mValue);
      }

      LANGULUS(INLINED)
      constexpr Byte operator + (const Byte& rhs) const noexcept {
         return static_cast<Byte>(mValue + rhs.mValue);
      }

      LANGULUS(INLINED)
      constexpr Byte operator - (const Byte& rhs) const noexcept {
         return static_cast<Byte>(mValue - rhs.mValue);
      }

      LANGULUS(INLINED)
      constexpr Byte operator * (const Byte& rhs) const noexcept {
         return static_cast<Byte>(mValue * rhs.mValue);
      }

      LANGULUS(INLINED)
      constexpr Byte operator / (const Byte& rhs) const noexcept {
         return static_cast<Byte>(mValue / rhs.mValue);
      }

      LANGULUS(INLINED)
      constexpr Byte operator % (const Byte& rhs) const noexcept {
         return static_cast<Byte>(mValue % rhs.mValue);
      }

      LANGULUS(INLINED)
      constexpr Byte operator << (const Byte& rhs) const noexcept {
         return static_cast<Byte>(mValue << rhs.mValue);
      }

      LANGULUS(INLINED)
      constexpr Byte operator >> (const Byte& rhs) const noexcept {
         return static_cast<Byte>(mValue >> rhs.mValue);
      }

      LANGULUS(INLINED)
      constexpr Byte operator ^ (const Byte& rhs) const noexcept {
         return static_cast<Byte>(mValue ^ rhs.mValue);
      }

      LANGULUS(INLINED)
      constexpr Byte operator & (const Byte& rhs) const noexcept {
         return static_cast<Byte>(mValue & rhs.mValue);
      }

      LANGULUS(INLINED)
      constexpr Byte operator | (const Byte& rhs) const noexcept {
         return static_cast<Byte>(mValue | rhs.mValue);
      }

      LANGULUS(INLINED)
      constexpr Byte& operator += (const Byte& rhs) noexcept {
         mValue += rhs.mValue;
         return *this;
      }

      LANGULUS(INLINED)
      constexpr Byte& operator -= (const Byte& rhs) noexcept {
         mValue -= rhs.mValue;
         return *this;
      }

      LANGULUS(INLINED)
      constexpr Byte& operator *= (const Byte& rhs) noexcept {
         mValue *= rhs.mValue;
         return *this;
      }

      LANGULUS(INLINED)
      constexpr Byte& operator /= (const Byte& rhs) noexcept {
         mValue /= rhs.mValue;
         return *this;
      }

      LANGULUS(INLINED)
      constexpr Byte& operator %= (const Byte& rhs) noexcept {
         mValue %= rhs.mValue;
         return *this;
      }

      LANGULUS(INLINED)
      constexpr Byte& operator <<= (const Byte& rhs) noexcept {
         mValue <<= rhs.mValue;
         return *this;
      }

      LANGULUS(INLINED)
      constexpr Byte& operator >>= (const Byte& rhs) noexcept {
         mValue >>= rhs.mValue;
         return *this;
      }

      LANGULUS(INLINED)
      constexpr Byte& operator ^= (const Byte& rhs) noexcept {
         mValue ^= rhs.mValue;
         return *this;
      }

      LANGULUS(INLINED)
      constexpr Byte& operator &= (const Byte& rhs) noexcept {
         mValue &= rhs.mValue;
         return *this;
      }

      LANGULUS(INLINED)
      constexpr Byte& operator |= (const Byte& rhs) noexcept {
         mValue |= rhs.mValue;
         return *this;
      }

      LANGULUS(INLINED)
      constexpr bool operator == (const Byte&) const noexcept = default;

      LANGULUS(INLINED)
      constexpr bool operator <= (const Byte& rhs) const noexcept {
         return mValue <= rhs.mValue;
      }

      LANGULUS(INLINED)
      constexpr bool operator >= (const Byte& rhs) const noexcept {
         return mValue >= rhs.mValue;
      }

      LANGULUS(INLINED)
      constexpr bool operator < (const Byte& rhs) const noexcept {
         return mValue < rhs.mValue;
      }

      LANGULUS(INLINED)
      constexpr bool operator > (const Byte& rhs) const noexcept {
         return mValue > rhs.mValue;
      }

      /// Prefix operators                                                    
      Byte& operator ++ () noexcept {
         ++mValue;
         return *this;
      }
      Byte& operator -- () noexcept {
         --mValue;
         return *this;
      }

      /// Suffix operators                                                    
      Byte operator ++ (int) noexcept {
         return static_cast<Byte>(mValue++);
      }

      Byte operator -- (int) noexcept {
         return static_cast<Byte>(mValue--);
      }
   };
   #pragma pack(pop)

   namespace CT
   {

      /// Built-in byte concept                                               
      template<class...T>
      concept BuiltinByte = sizeof...(T) > 0 and ((
            SimilarAsOneOf<Deref<T>, ::Langulus::Byte, ::std::byte>
         ) and ...);

      /// Custom byte concept (wrapped in another type)                       
      template<class...T>
      concept CustomByte = ((Typed<T> and
            BuiltinByte<TypeOf<T>> and sizeof(T) == sizeof(TypeOf<T>)
         ) and ...);

      /// Byte concept                                                        
      template<class...T>
      concept Byte = ((BuiltinByte<T> or CustomByte<T>) and ...);

      /// Any unsigned character, byte or integer, sized exactly 1 byte       
      template<class...T>
      concept UnsignedInteger8 = sizeof...(T) > 0
          and (((UnsignedInteger<T> or Character<T> or Byte<T>)
          and sizeof(Decay<T>) == 1) and ...);

      /// Any signed character, byte or integer, sized exactly 1 byte         
      template<class...T>
      concept Integer8 = sizeof...(T) > 0
          and ((SignedInteger8<T> or UnsignedInteger8<T>) and ...);

      template<class...T>
      concept IntegerX = sizeof...(T) > 0
          and ((Integer8<T> or Integer16<T> or Integer32<T> or Integer64<T>) and ...);

   } // namespace Langulus::CT

} // namespace Langulus
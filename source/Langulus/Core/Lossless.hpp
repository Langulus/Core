///                                                                           
/// Langulus::RTTI                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Vector.hpp"
#include "Utilities.hpp"
#include "Types.hpp"


namespace Langulus
{

   /// Casts a scalar to its underlying fundamental type (const)              
   /// If non-void T::CTTI_InnerType exists, or if T is an enum, the inner    
   /// type returns                                                           
   ///   @tparam T - type of the scalar/enum to cast                          
   ///   @param a - the scalar to cast                                        
   ///   @return a reference to the underlying type                           
   template<CT::Scalar T, bool FAKE = false> LANGULUS(INLINED)
   constexpr decltype(auto) FundamentalCast(const T& a) noexcept {
      using DT = Decay<Deint<T>>;
      if constexpr (CT::Fundamental<DT>) {
         // Already fundamental, just forward it                        
         return (a);
      }
      else if constexpr ((CT::Typed<DT> or CT::Enum<DT>)
      and requires { static_cast<const TypeOf<DT>&>(DenseCast(a)); }) {
         // Explicitly cast to a reference of the contained type, and   
         // nest down to the fundamentals                               
         return FundamentalCast(static_cast<const TypeOf<DT>&>(DenseCast(a)));
      }
      else if constexpr (FAKE)
         return Unsupported {};
      else
         static_assert(false, "Can't perform FundamentalCast");
   }
   
   /// Casts a scalar to its underlying fundamental type                      
   /// If non-void T::CTTI_InnerType exists, or if T is an enum, the inner    
   /// type returns                                                           
   ///   @tparam T - type of the scalar/enum to cast                          
   ///   @param a - the scalar to cast                                        
   ///   @return a reference to the underlying type                           
   template<CT::Scalar T, bool FAKE = false> LANGULUS(INLINED)
   constexpr decltype(auto) FundamentalCast(T& a) noexcept {
      using DT = Decay<Deint<T>>;
      if constexpr (CT::Fundamental<DT>) {
         // Already fundamental, just forward it                        
         return (a);
      }
      else if constexpr ((CT::Typed<DT> or CT::Enum<DT>)
      and requires { static_cast<TypeOf<DT>&>(DenseCast(a)); }) {
         // Explicitly cast to a reference of the contained type, and   
         // nest down to the fundamentals                               
         return FundamentalCast(static_cast<TypeOf<DT>&>(DenseCast(a)));
      }
      else if constexpr (FAKE)
         return Unsupported {};
      else
         static_assert(false, "Can't perform FundamentalCast");
   }

   namespace CT
   {

      /// Check if an instance of T can be converted to a fundamental         
      template<class...T>
      concept CastsToFundamental = (
         Supported<decltype(FundamentalCast<T, true>(Fake<T&>()))> and ...);

   } // namespace Langulus::CT
    
   /// Returns the extent overlap of two arrays/non arrays                    
   /// An array to be considered array, it has to have more than one element  
   ///   @tparam LHS - left type                                              
   ///   @tparam RHS - right type                                             
   ///   @return the overlapping count:                                       
   ///           the smaller extent, if two arrays are provided;              
   ///           the bigger extent, if one of the arguments isn't an array    
   ///           1 if both arguments are not arrays                           
   /*template<class LHS, class RHS>
   consteval Count OverlapExtents() {
      constexpr auto lhs = ExtentOf<Deint<LHS>>;
      constexpr auto rhs = ExtentOf<Deint<RHS>>;

      if constexpr (lhs > 1 and rhs > 1)
         return lhs < rhs ? lhs : rhs;
      else if constexpr (lhs > 1)
         return lhs;
      else if constexpr (rhs > 1)
         return rhs;
      else
         return 1;
   }

   #define OVERLAP_EXTENTS(l,r) OverlapExtents<decltype(l), decltype(r)>()*/
   
   namespace Inner
   {

      /// When given two types, choose the one that is most lossless in terms 
      /// of behavior, and capacity                                           
      ///  - if T1 or T2 is an array, an array of OverlapCount size will be   
      ///    given back. The array will be of the lossless decayed type       
      ///  - if both types are reals, the bigger real will be returned        
      ///  - if one of the type is a real, and the other an integer, the real 
      ///    will be returned                                                 
      ///  - if both types are integers with different signs, always returns  
      ///    the signed equivalent of the bigger integer                      
      ///  - if one of the types is not CT::Fundamental type, it will always  
      ///    be preferred, as it may have custom behavior                     
      ///  - if both types are not CT::Fundamental, the first type is always  
      ///    preferred (fallback)                                             
      ///   @attention this will discard any sparseness or other modifiers    
      ///   @attention this will shed any intents                             
      template<class T1, class T2>
      consteval auto Lossless() {
         constexpr auto size = OverlapCounts<T1, T2>();
         using LHS = Decay<TypeOf<Deint<T1>>>;
         using RHS = Decay<TypeOf<Deint<T2>>>;

         if constexpr (CT::Fundamental<LHS, RHS>) {
            // Both types are fundamental                               
            if constexpr (CT::Real<LHS, RHS>) {
               // Always prefer the bigger real number                  
               if constexpr (sizeof(LHS) >= sizeof(RHS))
                  return ::std::array<LHS, size> {};
               else
                  return ::std::array<RHS, size> {};
            }
            else if constexpr (CT::Real<LHS> and not CT::Real<RHS>) {
               // Always prefer real numbers                            
               return ::std::array<LHS, size> {};
            }
            else if constexpr (CT::Real<RHS> and not CT::Real<LHS>) {
               // Always prefer real numbers                            
               return ::std::array<RHS, size> {};
            }
            else if constexpr (CT::Signed<LHS> == CT::Signed<RHS>) {
               // Both are signed integers, so pick the bigger one      
               if constexpr (sizeof(LHS) >= sizeof(RHS))
                  return ::std::array<LHS, size> {};
               else
                  return ::std::array<RHS, size> {};
            }
            else if constexpr (CT::Signed<LHS>) {
               // LHS is signed, but RHS is not, so pick the signed one,
               // but also guarantee that size remains the bigger one   
               if constexpr (sizeof(LHS) >= sizeof(RHS))
                  return ::std::array<LHS, size> {};
               else
                  return ::std::array<::std::make_signed_t<RHS>, size> {};
            }
            else {
               // RHS is signed, but LHS is not, so pick the signed one,
               // but also guarantee that size remains the bigger one   
               if constexpr (sizeof(RHS) >= sizeof(LHS))
                  return ::std::array<RHS, size> {};
               else
                  return ::std::array<::std::make_signed_t<LHS>, size> {};
            }
         }
         else if constexpr (CT::Fundamental<LHS>) {
            // RHS isn't fundamental, so always prefer it               
            return ::std::array<RHS, size> {};
         }
         else {
            // Either both types aren't fundamental, or the RHS one is  
            // Just fallback to LHS                                     
            return ::std::array<LHS, size> {};
         }
      }

      /// Nest the above function for all types in a variadic template        
      template<class T1, class T2, class...TN>
      consteval auto LosslessNestedInner() {
         using T1T2 = decltype(Lossless<T1, T2>());

         if constexpr (sizeof...(TN))
            return LosslessNestedInner<T1T2, TN...>();
         else
            return T1T2 {};
      }

      /// Nest the above function for all types in a variadic template        
      template<class T1, class...TN>
      consteval auto LosslessNested() {
         if constexpr (sizeof...(TN) == 0)
            return ::std::array<Decay<TypeOf<T1>>, CountOf<T1>> {};
         else
            return LosslessNestedInner<T1, TN...>();
      }

   } // namespace Langulus::Inner

   /// Given any number of types, choose the one that is most lossless        
   /// after an arithmetic operation is performed between them. If any type   
   /// is an array, an array of OverlapCount size will be given back.         
   ///   @attention this will discard any sparseness or other modifiers       
   template<class T1, class...TN>
   using Lossless = Conditional<
         CountOf<decltype(::Langulus::Inner::LosslessNested<T1, TN...>())> == 1,
          TypeOf<decltype(::Langulus::Inner::LosslessNested<T1, TN...>())>,
          TypeOf<decltype(::Langulus::Inner::LosslessNested<T1, TN...>())>
               [CountOf<decltype(::Langulus::Inner::LosslessNested<T1, TN...>())>]
      >;

   namespace Inner
   {

      template<class T, bool FORCE_SIGNED = false>
      consteval auto WiderInner() {
         if constexpr (CT::SignedInteger8<T>)
            return Types<int16_t> {};
         else if constexpr (CT::UnsignedInteger8<T>) {
            if constexpr (FORCE_SIGNED)
               return Types<int16_t> {};
            else
               return Types<uint16_t> {};
         }
         else if constexpr (CT::SignedInteger16<T>)
            return Types<int32_t> {};
         else if constexpr (CT::UnsignedInteger16<T>) {
            if constexpr (FORCE_SIGNED)
               return Types<int32_t> {};
            else
               return Types<uint32_t> {};
         }
         else if constexpr (CT::SignedInteger32<T>)
            return Types<int64_t> {};
         else if constexpr (CT::UnsignedInteger32<T>) {
            if constexpr (FORCE_SIGNED)
               return Types<int64_t> {};
            else
               return Types<uint64_t> {};
         }
         else if constexpr (CT::Integer64<T>)
            return Types<T> {};
         else if constexpr (CT::Float<T>)
            return Types<double> {};
         else if constexpr (CT::Double<T>)
            return Types<double> {};
         else
           static_assert(false, "Can't find a wider type");
      }

      template<class T>
      consteval auto NarrowerInner() {
         if constexpr (CT::Integer8<T>)
            return Types<T> {};
         else if constexpr (CT::SignedInteger16<T>)
            return Types<int8_t> {};
         else if constexpr (CT::UnsignedInteger16<T>)
            return Types<uint8_t> {};
         else if constexpr (CT::SignedInteger32<T>)
            return Types<int16_t> {};
         else if constexpr (CT::UnsignedInteger32<T>)
            return Types<uint16_t> {};
         else if constexpr (CT::SignedInteger64<T>)
            return Types<int32_t> {};
         else if constexpr (CT::UnsignedInteger64<T>)
            return Types<uint32_t> {};
         else if constexpr (CT::Float<T>)
            return Types<T> {};
         else if constexpr (CT::Double<T>)
            return Types<float> {};
         else
           static_assert(false, "Can't find a narrower type");
      }

   } // namespace Langulus::Inner

   /// Get a wider fundamental type, if possible                              
   /// uint32_t -> uint64_t                                                   
   template<class T1, class...TN>
   using Wider = typename decltype(
         ::Langulus::Inner::WiderInner<Lossless<T1, TN...>>()
      )::First;

   /// Get a signed wider fundamental type, if possible                       
   /// uint32_t -> int64_t                                                    
   template<class T1, class...TN>
   using WiderSigned = typename decltype(
         ::Langulus::Inner::WiderInner<Lossless<T1, TN...>, true>()
      )::First;

   /// Get a smaller fundamental type, if possible                            
   /// uint32_t -> uint16_t                                                   
   template<class T1, class...TN>
   using Narrower = typename decltype(
         ::Langulus::Inner::NarrowerInner<Lossless<T1, TN...>>()
      )::First;

} // namespace Langulus

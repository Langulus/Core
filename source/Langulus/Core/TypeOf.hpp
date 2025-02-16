///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Common.hpp"


namespace Langulus
{
   namespace CT::Inner
   {

      /// Check if T is typed, having either CTTI_InnerType or value_type as  
      /// member type declarations                                            
      ///   @attention the inner type must not be 'void', in order for T to   
      ///      be considered 'typed', as in not 'type-erased'                 
      template<class T>
      consteval bool IsTyped() {
         if constexpr (not Complete<Deref<T>>)
            return false;
         else if constexpr (requires { typename Deref<T>::CTTI_InnerType; })
            return Data<typename Deref<T>::CTTI_InnerType>;
         else if constexpr (requires { typename Deref<T>::value_type; })
            return Data<typename Deref<T>::value_type>;
         else
            return false;
      }

      /// Convenience function that wraps std::underlying_type_t for enums,   
      /// as well as any array, or anything with CTTI_InnerType that isn't    
      /// void, or has the value_type member type defined                     
      ///   - if T is an array, returns pointer of the array type             
      ///   - if T has CTTI_InnerType/value_type, return pointer of the type  
      ///   - if T is an enum, return pointer of the underlying type          
      ///   - otherwise just return a decayed T pointer                       
      template<class T>
      consteval auto GetUnderlyingType() {
         if constexpr (Array<T>)
            return (Deref<Deext<T>>*) nullptr;
         else {
            using DT = Decay<T>;
            if constexpr (not Complete<DT>)
               return (Deref<T>*) nullptr;
            else if constexpr (requires { typename DT::CTTI_InnerType; })
               return (Deref<typename DT::CTTI_InnerType>*) nullptr;
            else if constexpr (requires { typename DT::value_type; })
               return (Deref<typename DT::value_type>*) nullptr;
            else if constexpr (CT::Enum<DT>)
               return (Deref<::std::underlying_type_t<DT>>*) nullptr;
            else
               return (Deref<T>*) nullptr;
         }
      };

   } // namespace Langulus::CT::Inner

   /// Get internal type of an enum, or anything reflected with the           
   /// LANGULUS(TYPED) member                                                 
   template<class T>
   using TypeOf = Deptr<decltype(CT::Inner::GetUnderlyingType<T>())>;

   namespace CT
   {

      /// Check if all T have underlying type defined                         
      template<class...T>
      concept Typed = (Inner::IsTyped<T>() and ...);

      /// Check if all T has no underlying types defined                      
      template<class...T>
      concept Untyped = ((not Typed<T>) and ...);

   } // namespace Langulus::CT

   /// Downcasts a typed wrapper to the contained element, if cast operator   
   /// to TypeOf<T>& is available                                             
   ///   @param what - the instance to decay                                  
   ///   @return a reference to the the inner data                            
   LANGULUS(ALWAYS_INLINED)
   constexpr auto& TypedCast(auto&& what) noexcept {
      using T = decltype(what);
      if constexpr (CT::Typed<T>) {
         using TT = TypeOf<T>;
         if constexpr (requires { what.TypedCast(); })
            return what.TypedCast();
         else if constexpr (requires { what.operator TT&& (); })
            return what.operator TT&& ();
         else if constexpr (requires { what.operator TT& (); })
            return what.operator TT& ();
         else if constexpr (requires { what.operator const TT& (); })
            return what.operator const TT& ();
         else
            static_assert(false, "No cast operator available for decaying to inner type");
      }
      else return what;
   }

} // namespace Langulus

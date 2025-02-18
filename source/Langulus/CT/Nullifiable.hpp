///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Abstract.hpp"


namespace Langulus::CT
{
   namespace Inner
   {

      /// Check if a type is nullifiable                                      
      ///   @tparam T - the type to check                                     
      ///   @return true if T is nullifiable                                  
      template<class T>
      consteval bool IsNullifiable() {
         if constexpr (Complete<T>) {
            if constexpr (not Abstract<T>) {
               if constexpr (Dense<T> and requires { T::CTTI_Nullifiable; })
                  return T::CTTI_Nullifiable;
               else if constexpr (Fundamental<T> or Sparse<T>)
                  return true;
               else
                  return false;
            }
            else return false;
         }
         else return false;
      };

   } // namespace Langulus::CT::Inner

   /// A nullifiable type is any type with a static member                    
   /// T::CTTI_Nullifiable set to true. If no such member exists, the type    
   /// is assumed NOT nullifiable by default, unless it is sparse             
   /// Nullifiable types improve default-construction by using some batching  
   /// runtime optimizations                                                  
   /// Use LANGULUS(NULLIFIABLE) true; as member to tag nullifiable types     
   template<class...T>
   concept Nullifiable = sizeof...(T) > 0 and (Inner::IsNullifiable<T>() and ...);

} // namespace Langulus::CT
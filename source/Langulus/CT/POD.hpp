#pragma once
#include "Abstract.hpp"


namespace Langulus::CT
{
   namespace Inner
   {

      /// Check if a type is POD (plain old data)                             
      ///   @tparam T - the type to check                                     
      ///   @return true if T is a POD type                                   
      template<class T>
      consteval bool IsPOD() {
         if constexpr (Complete<T>) {
            if constexpr (not Abstract<T>) {
               if constexpr (Dense<T> and requires { T::CTTI_POD; })
                  return T::CTTI_POD;

               if constexpr (Fundamental<T> or Sparse<T> or (
               ::std::is_trivial_v<T> and
               ::std::is_standard_layout_v<T> and
               ::std::is_destructible_v<T>))
                  return true;
               else
                  return false;
            }
            else return false;
         }
         else return false;
      };

   } // namespace Langulus::CT::Inner

   /// A POD (Plain Old Data) type is any type with a static member           
   /// T::CTTI_POD set to true. If no such member exists, the type is         
   /// assumed NOT POD by default, unless ::std::is_trivial, which seems to   
   /// be inconsistent across compilers.                                      
   /// POD types improve construction, destruction, copying, and cloning      
   /// by using some batching runtime optimizations                           
   /// All POD types are also directly serializable to binary                 
   /// Use "LANGULUS(POD) true;" as member to tag POD types                   
   template<class...T>
   concept POD = sizeof...(T) > 0 and (Inner::IsPOD<T>() and ...);

} // namespace Langulus::CT

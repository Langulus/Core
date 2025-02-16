#pragma once
#include "Abstract.hpp"


namespace Langulus::CT
{
      
   /// Check if T is default-constructible                                    
   ///   @attention this includes even fundamentals that are not initialized  
   template<class...T>
   concept Defaultable = ((not Abstract<T>) and ...)
       and requires { (T {}, ...); };

   /// Check if T is noexcept default-constructible                           
   ///   @attention this includes even fundamentals that are not initialized  
   template<class...T>
   concept DefaultableNoexcept = Defaultable<T...>
       and (noexcept(T {}) and ...);

} // namespace Langulus::CT
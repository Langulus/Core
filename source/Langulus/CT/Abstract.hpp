#pragma once
#include "../Core/Common.hpp"


namespace Langulus::CT
{
   namespace Inner
   {

      template<class T>
      consteval bool IsAbstract() {
         if constexpr (Complete<T> and Dense<T>) {
            if constexpr (requires { T::CTTI_Abstract; })
               return ::std::is_abstract_v<T> or T::CTTI_Abstract;
            else
               return ::std::is_abstract_v<T>;
         }
         else return false;
      }

   } // namespace Langulus::CT::Inner

   /// Check if all T are abstract (have at least one pure virtual function,  
   /// or are explicitly marked as LANGULUS(ABSTRACT) true). Sparse types are 
   /// never abstract!                                                        
   template<class...T>
   concept Abstract = (Inner::IsAbstract<T>() and ...);

} // namespace Langulus::CT
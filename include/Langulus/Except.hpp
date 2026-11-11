///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include <Langulus/Core.hpp>


namespace Langulus
{
   ///                                                                        
   ///   General exception                                                    
   ///                                                                        
   /// It is an equivalent to std::runtime_error, but with additional info    
   /// for debug builds, like message and location strings.                   
   struct Exception {
      using CTTI_Exception = Yup;

      static constexpr auto Name            = "GeneralException";
      static constexpr auto DefaultMessage  = "<no information provided>";
      static constexpr auto DefaultLocation = "<unknown location>";

      #if LANGULUS(DEBUG)
         // Exception message                                           
         const char* mMessage;
         // Exception location, as a separate literal to avoid concat   
         const char* mLocation;

         constexpr Exception(
            const char* message = DefaultMessage,
            const char* location = DefaultLocation
         ) noexcept : mMessage  {message}, mLocation {location} {}
      #else
         constexpr Exception(
            const char* = DefaultMessage,
            const char* = DefaultLocation
         ) noexcept {}
      #endif
   };
}

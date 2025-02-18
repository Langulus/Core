///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "CountOf.hpp"
#include "Intent.hpp"
#include "Byte.hpp"


namespace Langulus::CT
{

   /// Check if all the provided types are considered Vector types            
   /// Any type that is Typed and has CountOf that is at least 2, and         
   /// the T's size is exactly equal to sizeof(TypeOf<T>) * CountOf<T>        
   /// Additionally, bounded arrays with more than a single element are       
   /// also considered Vector.                                                
   template<class...T>
   concept Vector = (((Typed<Deint<T>>
            and Inner::CountOf<T>() > 1
            and sizeof(Deint<T>) == sizeof(TypeOf<Deint<T>>) * Inner::CountOf<T>()
         ) or (ExtentOf<Deint<T>> > 1)
      ) and ...);

   /// Scalar concept - any fundamental or custom number type, regardless     
   /// if wrapped inside an intent. Bounded arrays of those with              
   /// ExtentOf == 1 are also considered scalars.                             
   template<class...T>
   concept Scalar = ((Inner::CountOf<T>() == 1 and (
            Byte<Deext<Deint<T>>>
         or Integer<Deext<Deint<T>>>
         or Real<Deext<Deint<T>>>
         or Character<Deext<Deint<T>>>
      )) and ...);

} // namespace Langulus::CT
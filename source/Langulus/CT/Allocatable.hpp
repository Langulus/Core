///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "../Core/Common.hpp"


namespace Langulus::CT
{

   /// Check if any of the listed T is unallocatable                          
   /// You can make types unallocatable by the memory manager. This serves    
   /// not only as forcing the type to be either allocated by conventional    
   /// C++ means (on the heap or the stack), but also optimizes away any      
   /// memory manager searches, when inserting pointers while managed memory  
   /// is enabled. Raw function pointers are unallocatable by default         
   template<class...T>
   concept Unallocatable = not Complete<T...> or Function<T...>
        or ((Dense<T> and Decay<T>::CTTI_Unallocatable) or ...);

   /// Check if all of the types are allocatable                              
   template<class...T>
   concept Allocatable = ((not Unallocatable<T>) and ...);

} // namespace Langulus::CT
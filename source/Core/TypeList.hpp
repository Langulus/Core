///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Common.hpp"

namespace Langulus
{

   ///                                                                        
   ///   Compile-time type list                                               
   ///                                                                        
   template<class...T>
   struct TTypeList;

   template<class T>
   struct TTypeList<T> {
      using First = T;
   };

   template<class HEAD, class... TAIL>
   struct TTypeList<HEAD, TAIL...> {
      using First = HEAD;
   };

   template<class... T>
   using FirstOf = typename TTypeList<T...>::First;

   using EmptyTypeList = TTypeList<void>;

} // namespace Langulus

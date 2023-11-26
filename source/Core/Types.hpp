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
   /// It doesn't really carry any data, it's just a useful compile-time tool 
   ///                                                                        
   template<class...T>
   struct Types;

   /// Type list that contains only one type, which isn't void                
   template<CT::Data T>
   struct Types<T> {
      static constexpr bool Empty = false;
      using First = T;
   };

   /// Type list that contains multiple non-void types                        
   template<CT::Data HEAD, CT::Data... TAIL>
   struct Types<HEAD, TAIL...> {
      static constexpr bool Empty = false;
      using First = HEAD;
   };

   /// Type list, that contains only one void item - a canonical empty list   
   template <>
   struct Types<void> {
      static constexpr bool Empty = true;
      using First = void;
   };

   /// Retrieve the first type from a type list                               
   ///   @attention type will be void, if type list is empty                  
   template<class... T>
   using FirstOf = typename Types<T...>::First;

   /// CTAD calls to constructor Types() will instantiate as an empty list    
   /// https://stackoverflow.com/questions/62847200                           
   template<class...> Types() -> Types<void>;

   template<CT::Data... T>
   constexpr auto CreateTypeList() {
      if constexpr (sizeof...(T))
         return Types<T...> {};
      else
         return Types {};
   }

} // namespace Langulus

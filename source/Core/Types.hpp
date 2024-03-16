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

   /// Type list that contains exactly one type, which isn't void             
   template<CT::Data T>
   struct Types<T> {
      static constexpr bool Empty = false;
      using First = T;

      template<class LAMBDA>
      static constexpr decltype(auto) ForEach(LAMBDA&& generator) {
         return generator.template operator()<T>();
      }
      template<class LAMBDA>
      static constexpr decltype(auto) ForEachAnd(LAMBDA&& generator) {
         return generator.template operator()<T>();
      }
      template<class LAMBDA>
      static constexpr decltype(auto) ForEachOr(LAMBDA&& generator) {
         return generator.template operator()<T>();
      }
   };

   /// Type list that contains multiple non-void types                        
   template<CT::Data T1, CT::Data T2, CT::Data...TN>
   struct Types<T1, T2, TN...> {
      static constexpr bool Empty = false;
      using First = T1;
      using Second = T2;

      template<class LAMBDA>
      static constexpr void ForEach(LAMBDA&& generator) {
         generator.template operator()<T1>();
         generator.template operator()<T2>();
         (generator.template operator()<TN>(), ...);
      }
      template<class LAMBDA>
      static constexpr bool ForEachAnd(LAMBDA&& generator) {
         return generator.template operator()<T1>()
            and generator.template operator()<T2>()
            and (... and generator.template operator()<TN>());
      }
      template<class LAMBDA>
      static constexpr bool ForEachOr(LAMBDA&& generator) {
         return generator.template operator()<T1>()
             or generator.template operator()<T2>()
             or (... or generator.template operator()<TN>());
      }
   };

   /// Type list, that contains only one void item - a canonical empty list   
   template<>
   struct Types<void> {
      static constexpr bool Empty = true;

      template<class LAMBDA>
      static constexpr bool ForEach(LAMBDA&&) noexcept {
         return false;
      }
      template<class LAMBDA>
      static constexpr bool ForEachAnd(LAMBDA&&) noexcept {
         return false;
      }
      template<class LAMBDA>
      static constexpr bool ForEachOr(LAMBDA&&) noexcept {
         return false;
      }
   };

   /// Retrieve the first type from a type list                               
   template<class...T>
   using FirstOf = typename Types<T...>::First;

   /// Retrieve the second type from a type list                              
   template<class...T>
   using SecondOf = typename Types<T...>::Second;

   /// CTAD calls to constructor Types() will instantiate as an empty list    
   /// https://stackoverflow.com/questions/62847200                           
   template<class...> Types() -> Types<void>;

   template<CT::Data...T>
   consteval auto CreateTypeList() {
      if constexpr (sizeof...(T))
         return Types<T...> {};
      else
         return Types {};
   }

} // namespace Langulus

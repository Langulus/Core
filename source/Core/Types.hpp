///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
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


   ///                                                                        
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

      template<class...MORE>
      static consteval auto ConcatInner(Types<MORE...>) {
         return Types<T, MORE...> {};
      }

      template<class LIST>
      using Concat = decltype(ConcatInner(LIST {}));
   };


   ///                                                                        
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

      template<class...MORE>
      static consteval auto ConcatInner(Types<MORE...>) {
         return Types<T1, T2, TN..., MORE...> {};
      }

      template<class LIST>
      using Concat = decltype(ConcatInner(LIST {}));
   };


   ///                                                                        
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

      template<class...MORE>
      static consteval auto ConcatInner(Types<MORE...>) {
         return Types<MORE...> {};
      }

      template<class LIST>
      using Concat = decltype(ConcatInner(LIST {}));
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
   
   namespace Inner
   {
      /// Type used to detect unavailable stuff all over                      
      struct Unsupported {};

      /// This function declaration is used to decompose a lambda             
      /// You can use it to extract the argument types of functions, by using 
      /// decltype on the function return.                                    
      /// Handles functors, member/standing function pointers, lambdas.       
      template<class R, class F, class A, class...AN>
      Types<A, AN...> GetFunctionArguments(R(F::*)(A, AN...) const) {
         LANGULUS_ERROR("Calling GetFunctionArguments is ill-formed");
      }
      template<class R, class F, class A, class...AN>
      Types<A, AN...> GetFunctionArguments(R(F::*)(A, AN...)) {
         LANGULUS_ERROR("Calling GetFunctionArguments is ill-formed");
      }
      template<class R, class A, class...AN>
      Types<A, AN...> GetFunctionArguments(R(*)(A, AN...)) {
         LANGULUS_ERROR("Calling GetFunctionArguments is ill-formed");
      }
      template<class F>
      decltype(GetFunctionArguments(&F::operator())) GetFunctionArguments(F) {
         LANGULUS_ERROR("Calling GetFunctionArguments is ill-formed");
      }
   }

   /// Get the type of the first argument of a function                       
   ///   @tparam F - anything invokable, like functor/member function/lambda  
   template<class F>
   using ArgumentOf = typename decltype(Inner::GetFunctionArguments(Fake<F>()))::First;

   /// Get a type list corresponding to the function arguments                
   ///   @tparam F - anything invokable, like functor/member function/lambda  
   template<class F>
   using ArgumentsOf = decltype(Inner::GetFunctionArguments(Fake<F>()));

   /// Get the return type of a function                                      
   ///   @tparam F - anything invokable, like functor/member function/lambda  
   template<class F>
   using ReturnOf = decltype((Fake<F>()) (Fake<ArgumentOf<F>>()));

   namespace CT
   {
      /// Check if any T is the built-in one that signifies lack of support   
      template<class...T>
      concept Unsupported = sizeof...(T) > 0
          and (Same<::Langulus::Inner::Unsupported, T> or ...);

      /// Check if all T are supported                                        
      template<class...T>
      concept Supported = sizeof...(T) > 0
          and ((not Unsupported<T>) and ...);
   }

} // namespace Langulus

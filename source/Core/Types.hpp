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

   namespace CT
   {
      template<class T>
      concept Types = T::CTTI_TypeListTag;
      template<class T>
      concept NotTypes = not Types<T>;
   }

   ///                                                                        
   ///   Compile-time type list                                               
   ///                                                                        
   /// It doesn't really carry any data, it's just a useful compile-time tool 
   ///                                                                        
   template<class...T>
   struct Types;

   namespace Inner
   {
      template<class GATHERED, class HEAD, class...TAIL>
      static consteval CT::Types auto GenerateTypes(auto&& lambda) {
         using R = decltype(lambda.template operator()<HEAD>());
         using C = typename GATHERED::template Concat<R>;
         if constexpr (sizeof...(TAIL))
            return GenerateTypes<C, TAIL...>(lambda);
         else
            return C {};
      }
   }
   
   ///                                                                        
   /// Type list, that contains only one void item - a canonical empty list   
   template<>
   struct Types<void> {
      static constexpr bool CTTI_TypeListTag = true;
      static constexpr bool Empty = true;
      static constexpr size_t Count = 0;

      static constexpr void ForEach   (auto&&) noexcept { }
      static constexpr bool ForEachAnd(auto&&) noexcept { return false; }
      static constexpr bool ForEachOr (auto&&) noexcept { return false; }

      template<class L>
      using Concat = Conditional<CT::Types<L>, L, Types<L>>;
   };


   ///                                                                        
   /// Type list that contains exactly one type, which isn't void             
   template<CT::Data T>
   struct Types<T> {
      static constexpr bool CTTI_TypeListTag = true;
      static constexpr bool Empty = false;
      static constexpr size_t Count = 1;
      using First = T;

      static constexpr void ForEach(auto&& lambda) {
         static_assert(requires{ lambda.template operator()<T>(); },
            "Provided argument is not a lambda of the form []<class>");
         lambda.template operator()<T>();
      }

      static constexpr bool ForEachAnd(auto&& lambda) {
         static_assert(requires{ {lambda.template operator()<T>()} -> ::std::convertible_to<bool>; },
            "Provided argument is not a lambda of the form []<class> -> convertible to bool");
         return lambda.template operator()<T>();
      }

      static constexpr bool ForEachOr(auto&& lambda) {
         static_assert(requires{ {lambda.template operator()<T>()} -> ::std::convertible_to<bool>; },
            "Provided argument is not a lambda of the form []<class> -> convertible to bool");
         return lambda.template operator()<T>();
      }

      static constexpr void ForEachIndexed(auto&& lambda) {
         static_assert(requires{ lambda.template operator()<T,0>(); },
            "Provided argument is not a lambda of the form []<class,index>");
         lambda.template operator()<T,0>();
      }

      static constexpr bool ForEachIndexedAnd(auto&& lambda) {
         static_assert(requires{ {lambda.template operator()<T,0>()} -> ::std::convertible_to<bool>; },
            "Provided argument is not a lambda of the form []<class,index> -> convertible to bool");
         return lambda.template operator()<T,0>();
      }

      static constexpr bool ForEachIndexedOr(auto&& lambda) {
         static_assert(requires{ {lambda.template operator()<T,0>()} -> ::std::convertible_to<bool>; },
            "Provided argument is not a lambda of the form []<class,index> -> convertible to bool");
         return lambda.template operator()<T,0>();
      }

   private:
      template<class...MORE>
      static consteval CT::Types auto ConcatInner(Types<MORE...>) {
         return Types<T, MORE...> {};
      }

   public:
      template<class L>
      using Concat = Conditional<CT::Types<L>, decltype(ConcatInner(L {})), Types<T, L>>;

      template<Offset I>
      static consteval auto AtInner() {
         static_assert(I == 0, "Index is out of type list bounds (list has one type)");
         return Types<T> {};
      }

      template<Offset I>
      using At = typename decltype(AtInner<I>())::First;

      /// Generate a type list by providing a consteval generator lambda      
      ///   @param lambda - the function that will generate the types         
      ///          the lambda may or may not return Types, which will be      
      ///          concatenated along if so                                   
      ///   @return a type list, containing the generated types               
      static consteval CT::Types auto GenerateTypes(auto&& lambda) {
         static_assert(requires{ {lambda.template operator()<T>()} -> CT::Data; },
            "Provided argument is not a lambda of the form []<class> -> non-void type");
         using R = decltype(lambda.template operator()<T>());
         return typename Types<void>::template Concat<R> {};
      }

      using Tuple = std::tuple<T>;

      static constexpr Tuple GenerateData(auto&& lambda) {
         static_assert(requires{ {lambda.template operator()<T>()} -> CT::Data; },
            "Provided argument is not a lambda of the form []<class> -> non-void type");
         return {lambda.template operator()<T>()};
      }
   };


   ///                                                                        
   /// Type list that contains multiple non-void types                        
   template<CT::Data T1, CT::Data T2, CT::Data...TN>
   struct Types<T1, T2, TN...> {
      static constexpr bool CTTI_TypeListTag = true;
      static constexpr bool Empty = false;
      static constexpr size_t Count = sizeof...(TN) + 2;
      using First = T1;
      using Second = T2;

      static constexpr void ForEach(auto&& lambda) {
         static_assert(requires{ lambda.template operator()<T1>(); },
            "Provided argument is not a lambda of the form []<class>");
          lambda.template operator()<T1>();
          lambda.template operator()<T2>();
         (lambda.template operator()<TN>(), ...);
      }

      static constexpr bool ForEachAnd(auto&& lambda) {
         static_assert(requires{ {lambda.template operator()<T1>()} -> ::std::convertible_to<bool>; },
            "Provided argument is not a lambda of the form []<class> -> convertible to bool");
         return lambda.template operator()<T1>()
            and lambda.template operator()<T2>()
            and (... and lambda.template operator()<TN>());
      }

      static constexpr bool ForEachOr(auto&& lambda) {
         static_assert(requires{ {lambda.template operator()<T1>()} -> ::std::convertible_to<bool>; },
            "Provided argument is not a lambda of the form []<class> -> convertible to bool");
         return lambda.template operator()<T1>()
             or lambda.template operator()<T2>()
             or (... or lambda.template operator()<TN>());
      }

      template<Offset IDX = 0>
      static constexpr void ForEachIndexed(auto&& lambda) {
         static_assert(requires{ lambda.template operator()<T1,0>(); },
            "Provided argument is not a lambda of the form []<class,index>");
          lambda.template operator()<T1, IDX + 0>();

          if constexpr (Count > 1) {
             lambda.template operator()<T2, IDX + 1>();

             if constexpr (sizeof...(TN) > 0)
               Types<TN...>::template ForEachIndexed<IDX + 2>(lambda);
          }
      }

      template<Offset IDX = 0>
      static constexpr bool ForEachIndexedAnd(auto&& lambda) {
         static_assert(requires{ {lambda.template operator()<T1,0>()} -> ::std::convertible_to<bool>; },
            "Provided argument is not a lambda of the form []<class,index> -> convertible to bool");
         if constexpr (Count == 1)
            return lambda.template operator()<T1, IDX>();
         else if constexpr (Count == 2)
            return lambda.template operator()<T1, IDX + 0>()
               and lambda.template operator()<T2, IDX + 1>();
         else if constexpr (Count > 2)
            return lambda.template operator()<T1, IDX + 0>()
               and lambda.template operator()<T2, IDX + 1>()
               and Types<TN...>::template ForEachIndexedAnd<IDX + 2>(lambda);
         else return false;
      }

      template<Offset IDX = 0>
      static constexpr bool ForEachIndexedOr(auto&& lambda) {
         static_assert(requires{ {lambda.template operator()<T1,0>()} -> ::std::convertible_to<bool>; },
            "Provided argument is not a lambda of the form []<class,index> -> convertible to bool");
         if constexpr (Count == 1)
            return lambda.template operator()<T1, IDX>();
         else if constexpr (Count == 2)
            return lambda.template operator()<T1, IDX + 0>()
                or lambda.template operator()<T2, IDX + 1>();
         else if constexpr (Count > 2)
            return lambda.template operator()<T1, IDX + 0>()
                or lambda.template operator()<T2, IDX + 1>()
                or Types<TN...>::template ForEachIndexedOr<IDX + 2>(lambda);
         else return false;
      }

   private:
      template<class...MORE>
      static consteval CT::Types auto ConcatInner(Types<MORE...>) {
         return Types<T1, T2, TN..., MORE...> {};
      }

   public:
      template<class L>
      using Concat = Conditional<CT::Types<L>, decltype(ConcatInner(L {})), decltype(ConcatInner(Types<L> {}))>;

      template<Offset I>
      static consteval auto AtInner() {
         if constexpr (I == 0)
            return Types<T1> {};
         else if constexpr (I == 1)
            return Types<T2> {};
         else if constexpr (I < Count)
            return Types<TN...>::template AtInner<I - 2>();
         else
            static_assert(false, "Index is out of type list bounds");
      }

      template<Offset I>
      using At = typename decltype(AtInner<I>())::First;

      /// Generate a type list by providing a consteval generator lambda      
      ///   @param lambda - the function that will generate the types         
      ///          the lambda may or may not return Types, which will be      
      ///          concatenated along if so                                   
      ///   @return a type list, containing the generated types               
      static consteval CT::Types auto GenerateTypes(auto&& lambda) {
         static_assert(requires{ {lambda.template operator()<T1>()} -> CT::Data; },
            "Provided argument is not a lambda of the form []<class> -> non-void type");
         return Inner::GenerateTypes<Types<void>, T1, T2, TN...>(lambda);
      }

      using Tuple = std::tuple<T1, T2, TN...>;

      static constexpr Tuple GenerateData(auto&& lambda) {
         static_assert(requires{ {lambda.template operator()<T1>()} -> CT::Data; },
            "Provided argument is not a lambda of the form []<class> -> non-void type");
         return {
            lambda.template operator()<T1>(),
            lambda.template operator()<T2>(),
            lambda.template operator()<TN>()...
         };
      }
   };

   #define LangulusTypegen(TYPES, LAMBDA) decltype(TYPES::GenerateTypes(LAMBDA));

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
         static_assert(false, "Calling GetFunctionArguments is ill-formed");
      }
      template<class R, class F, class A, class...AN>
      Types<A, AN...> GetFunctionArguments(R(F::*)(A, AN...)) {
         static_assert(false, "Calling GetFunctionArguments is ill-formed");
      }
      template<class R, class A, class...AN>
      Types<A, AN...> GetFunctionArguments(R(*)(A, AN...)) {
         static_assert(false, "Calling GetFunctionArguments is ill-formed");
      }
      template<class F>
      decltype(GetFunctionArguments(&F::operator())) GetFunctionArguments(F) {
         static_assert(false, "Calling GetFunctionArguments is ill-formed");
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

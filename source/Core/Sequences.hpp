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

   template<Offset...IDX>
   using ExpandedSequence = ::std::integer_sequence<Offset, IDX...>;

   ///                                                                        
   ///   Compile-time integer sequences                                       
   ///                                                                        
   template<Offset END>
   struct Sequence {
   protected:
      template<class LAMBDA>
      static consteval bool Noexcept() {
         return noexcept(Fake<LAMBDA&&>().template operator() <0> ());
      }

   public:
      static constexpr Offset Size = END;
      static constexpr bool Empty  = END == 0;
      static constexpr auto Expand = ::std::make_integer_sequence<Offset, END> {};

      /// Iterate through each index in the sequence using generator pattern  
      ///   @param generator - a templated lambda function                    
      /// Example use:                                                        
      ///   Sequence<Ret::Columns>::ForEach([&]<Offset COL>() noexcept {      
      ///      auto& lc = lhs.template GetColumn<COL>();                      
      ///      Sequence<Ret::Rows>::ForEach([&]<Offset ROW>() noexcept {      
      ///         *(r++) = (lc * rhs.template GetRow<ROW>()).HSum();          
      ///      });                                                            
      ///   });                                                               
      template<class LAMBDA> LANGULUS(INLINED)
      static constexpr void ForEach(LAMBDA&& generator) noexcept(Noexcept<LAMBDA>()) {
         [&]<Offset...IDX>(ExpandedSequence<IDX...>) noexcept(Noexcept<LAMBDA>()) {
            (generator.template operator() <IDX> (), ...);
         }(Expand);
      }
   };

} // namespace Langulus

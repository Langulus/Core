///                                                                           
/// Langulus::Core                                                            
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Common.hpp"
#include "Exceptions.hpp"

namespace Langulus
{
   
   /// A namespace dedicated to abstract entities                             
   namespace A
   {
      /// An abstract abandoned value                                         
      struct Abandoned {};
      /// An abstract disowned value                                          
      struct Disowned {};
      /// An abstract cloned value                                            
      struct Cloned {};
   }

   namespace CT
   {
      /// Check if a type is abandoned                                        
      template<class... T>
      concept Abandoned = (DerivedFrom<T, A::Abandoned> && ...);

      /// Check if a type is disowned                                         
      template<class... T>
      concept Disowned = (DerivedFrom<T, A::Disowned> && ...);

      /// Check if a type is cloned                                           
      template<class... T>
      concept Cloned = (DerivedFrom<T, A::Cloned> && ...);

      /// Checks if a type has special semantics                              
      template<class... T>
      concept Semantic = ((Disowned<T> || Abandoned<T> || Cloned<T>) && ...);

      /// Checks if a type has no special semantics                           
      template<class... T>
      concept NotSemantic = (!Semantic<T> && ...);
   }
   

   ///                                                                        
   /// Abandoned value intermediate type, can be used in constructors and     
   /// assignments to provide a guarantee, that the value shall not be used   
   /// after that function, so we can save up on resetting it fully           
   /// For example, you can construct an Any with an abandoned Any, which is  
   /// same as move-construction, but the abandoned Any shall have only its   
   /// mEntry reset, instead of the entire container                          
   ///   @tparam T - the type to abandon                                      
   template<class T>
   struct Abandoned : public A::Abandoned {
      using Type = T;

      T&& mValue;

      Abandoned() = delete;
      Abandoned(const Abandoned&) = delete;
      explicit constexpr Abandoned(Abandoned&&) noexcept = default;
      explicit constexpr Abandoned(T&& value) noexcept 
         : mValue {Langulus::Forward<T>(value)} {
         static_assert(CT::NotSemantic<T>, "Can't nest semantics");
      }
      
      /// Forward as abandoned                                                
      template<class ALT_T = T>
      NOD() constexpr Abandoned<ALT_T> Forward() const noexcept {
         static_assert(CT::NotSemantic<ALT_T>, "Can't nest semantics");
         return Abandoned<ALT_T>{
            Langulus::Forward<ALT_T>(mValue)
         };
      }
   };
   
   /// Abandon a value                                                        
   /// Same as Move, but resets only mandatory data inside source after move  
   /// essentially saving up on a couple of instructions                      
   template<CT::NotSemantic T>
   NOD() constexpr auto Abandon(T&& a) noexcept {
      return Abandoned<T>{Langulus::Forward<T>(a)};
   }

   /// Abandon a value                                                        
   /// Same as Move, but resets only mandatory data inside source after move  
   /// essentially saving up on a couple of instructions                      
   template<CT::NotSemantic T>
   NOD() constexpr auto Abandon(T& a) noexcept {
      return Abandoned<T>{Langulus::Move(a)};
   }


   ///                                                                        
   /// Disowned value intermediate type, use in constructors and assignments  
   /// to copy container while avoiding referencing it                        
   ///   @tparam T - the type to disown                                       
   template<class T>
   struct Disowned : public A::Disowned {
      using Type = T;

      const T& mValue;

      Disowned() = delete;
      Disowned(const Disowned&) = delete;
      explicit constexpr Disowned(Disowned&&) noexcept = default;
      explicit constexpr Disowned(const T& value) noexcept 
         : mValue {value} {
         static_assert(CT::NotSemantic<T>, "Can't nest semantics");
      }
      
      /// Forward as disowned                                                 
      template<class ALT_T = T>
      NOD() constexpr Disowned<ALT_T> Forward() const noexcept {
         static_assert(CT::NotSemantic<ALT_T>, "Can't nest semantics");
         return Disowned<ALT_T>{mValue};
      }		
   };
   
   /// Disown a value                                                         
   /// Same as a shallow-copy, but never references, saving some instructions 
   template<CT::NotSemantic T>
   NOD() constexpr auto Disown(const T& item) noexcept {
      return Disowned<T>{item};
   }
   

   ///                                                                        
   /// Cloned value intermediate type, use in constructors and assignments    
   /// to clone container, doing a deep copy instead of the default shallow   
   ///   @tparam T - the type to clone                                        
   template<class T>
   struct Cloned : public A::Cloned {
      using Type = T;

      const T& mValue;

      Cloned() = delete;
      Cloned(const Cloned&) = delete;
      explicit constexpr Cloned(Cloned&&) noexcept = default;
      explicit constexpr Cloned(const T& value) noexcept
         : mValue {value} {
         static_assert(CT::NotSemantic<T>, "Can't nest semantics");
      }
      
      
      /// Forward as cloned                                                   
      template<class ALT_T = T>
      NOD() constexpr Cloned<ALT_T> Forward() const noexcept {
         static_assert(CT::NotSemantic<ALT_T>, "Can't nest semantics");
         return Cloned<ALT_T>{mValue};
      }		
   };
   
   /// Clone a value                                                          
   template<CT::NotSemantic T>
   NOD() constexpr auto Clone(const T& item) noexcept {
      return Cloned<T>{item};
   }

} // namespace Langulus

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

      /// An abstract semantic value                                          
      struct Semantic {};

      /// An abstract copied value                                            
      struct Copied : public Semantic {
         static constexpr bool Keep = true;
         static constexpr bool Move = false;
         static constexpr bool Shallow = true;
      };

      /// An abstract moved value                                             
      struct Moved : public Semantic {
         static constexpr bool Keep = true;
         static constexpr bool Move = true;
         static constexpr bool Shallow = true;
      };

      /// An abstract abandoned value                                         
      struct Abandoned : public Semantic {
         static constexpr bool Keep = false;
         static constexpr bool Move = true;
         static constexpr bool Shallow = true;
      };

      /// An abstract disowned value                                          
      struct Disowned : public Semantic {
         static constexpr bool Keep = false;
         static constexpr bool Move = false;
         static constexpr bool Shallow = true;
      };

      /// An abstract cloned value                                            
      struct Cloned : public Semantic {
         static constexpr bool Keep = true;
         static constexpr bool Move = false;
         static constexpr bool Shallow = false;
      };
   }

   namespace CT
   {
      /// Checks if a type has special semantics                              
      template<class... T>
      concept Semantic = ((Dense<T> && DerivedFrom<T, A::Semantic>) && ...);

      /// Checks if a type has no special semantics                           
      template<class... T>
      concept NotSemantic = (!Semantic<T> && ...);

      /// Check if a type is copied                                           
      template<class... T>
      concept Copied = ((Dense<T> && DerivedFrom<T, A::Copied>) && ...);

      /// Check if a type is moved                                            
      template<class... T>
      concept Moved = ((Dense<T> && DerivedFrom<T, A::Moved>) && ...);

      /// Check if a type is abandoned                                        
      template<class... T>
      concept Abandoned = ((Dense<T> && DerivedFrom<T, A::Abandoned>) && ...);

      /// Check if a type is disowned                                         
      template<class... T>
      concept Disowned = ((Dense<T> && DerivedFrom<T, A::Disowned>) && ...);

      /// Check if a type is cloned                                           
      template<class... T>
      concept Cloned = ((Dense<T> && DerivedFrom<T, A::Cloned>) && ...);
   }
   
   
   ///                                                                        
   /// Copied value intermediate type, use in constructors and assignments    
   /// to shallow-copy container explicitly                                   
   ///   @tparam T - the type to copy                                         
   template<class T>
   struct Copied : public A::Copied {
      // Makes the type CT::Typed                                       
      using MemberType = T;

      const T& mValue;

      Copied() = delete;
      Copied(const Copied&) = delete;
      explicit constexpr Copied(Copied&&) noexcept = default;
      explicit constexpr Copied(const T& value) noexcept
         : mValue {value} {
         static_assert(CT::NotSemantic<T>, "Can't nest semantics");
      }
      
      /// Forward as copied                                                   
      template<class ALT_T = T>
      NOD() constexpr Copied<ALT_T> Forward() const noexcept {
         static_assert(CT::NotSemantic<ALT_T>, "Can't nest semantics");
         return Copied<ALT_T>{mValue};
      }

      /// Copy something else                                                 
      template<class ALT_T>
      NOD() static constexpr Copied<ALT_T> Nest(const ALT_T& value) noexcept {
         return Copied<ALT_T>{value};
      }

      template<class ALT_T>
      using Nested = Copied<ALT_T>;
   };
   
   /// Copy a value                                                           
   template<CT::NotSemantic T>
   NOD() constexpr auto Copy(const T& item) noexcept {
      return Copied<T>{item};
   }
   

   ///                                                                        
   /// Moved value intermediate type, use in constructors and assignments     
   /// to move data explicitly                                                
   ///   @tparam T - the type to move                                         
   template<class T>
   struct Moved : public A::Moved {
      static_assert(!::std::is_const_v<T>,
         "T must be mutable in order to be moved");

      // Makes the type CT::Typed                                       
      using MemberType = T;

      T&& mValue;

      Moved() = delete;
      Moved(const Moved&) = delete;
      explicit constexpr Moved(Moved&&) noexcept = default;
      explicit constexpr Moved(T&& value) noexcept
         : mValue {::std::forward<T>(value)} {
         static_assert(CT::NotSemantic<T>, "Can't nest semantics");
      }

      /// Forward as moved                                                    
      template<class ALT_T = T>
      NOD() constexpr Moved<ALT_T> Forward() const noexcept {
         static_assert(CT::NotSemantic<ALT_T>, "Can't nest semantics");
         return Moved<ALT_T>{::std::forward<ALT_T>(mValue)};
      }

      /// Move something else                                                 
      template<class ALT_T>
      NOD() static constexpr Moved<ALT_T> Nest(ALT_T& value) noexcept {
         return Moved<ALT_T>{::std::move(value)};
      }

      template<class ALT_T>
      NOD() static constexpr Moved<ALT_T> Nest(ALT_T&& value) noexcept {
         return Moved<ALT_T>{::std::forward<ALT_T>(value)};
      }

      template<class ALT_T>
      using Nested = decltype(Nest(Uneval<ALT_T>()));
   };
   
   /// Move data                                                              
   template<CT::NotSemantic T>
   NOD() constexpr auto Move(T&& a) noexcept {
      return Moved<T>{::std::forward<T>(a)};
   }

   /// Move data                                                              
   template<CT::NotSemantic T>
   NOD() constexpr auto Move(T& a) noexcept {
      return Moved<T>{::std::move(a)};
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
      static_assert(!::std::is_const_v<T>,
         "T must be mutable in order to be abandoned");

      // Makes the type CT::Typed                                       
      using MemberType = T;

      T&& mValue;

      Abandoned() = delete;
      Abandoned(const Abandoned&) = delete;
      explicit constexpr Abandoned(Abandoned&&) noexcept = default;
      explicit constexpr Abandoned(T&& value) noexcept 
         : mValue {::std::forward<T>(value)} {
         static_assert(CT::NotSemantic<T>, "Can't nest semantics");
      }
      
      /// Forward as abandoned                                                
      template<class ALT_T = T>
      NOD() constexpr Abandoned<ALT_T> Forward() const noexcept {
         static_assert(CT::NotSemantic<ALT_T>, "Can't nest semantics");
         return Abandoned<ALT_T>{::std::forward<ALT_T>(mValue)};
      }

      /// Abandon something else                                              
      template<class ALT_T>
      NOD() static constexpr Abandoned<ALT_T> Nest(ALT_T& value) noexcept {
         return Abandoned<ALT_T>{::std::move(value)};
      }

      template<class ALT_T>
      NOD() static constexpr Abandoned<ALT_T> Nest(ALT_T&& value) noexcept {
         return Abandoned<ALT_T>{::std::forward<ALT_T>(value)};
      }

      template<class ALT_T>
      using Nested = decltype(Nest(Uneval<ALT_T>()));
   };
   
   /// Abandon a value                                                        
   /// Same as Move, but resets only mandatory data inside source after move  
   /// essentially saving up on a couple of instructions                      
   template<CT::NotSemantic T>
   NOD() constexpr auto Abandon(T&& a) noexcept {
      return Abandoned<T>{::std::forward<T>(a)};
   }

   /// Abandon a value                                                        
   /// Same as Move, but resets only mandatory data inside source after move  
   /// essentially saving up on a couple of instructions                      
   template<CT::NotSemantic T>
   NOD() constexpr auto Abandon(T& a) noexcept {
      return Abandoned<T>{::std::move(a)};
   }


   ///                                                                        
   /// Disowned value intermediate type, use in constructors and assignments  
   /// to copy container without gaining ownership                            
   ///   @tparam T - the type to disown                                       
   template<class T>
   struct Disowned : public A::Disowned {
      // Makes the type CT::Typed                                       
      using MemberType = T;

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

      /// Disown something else                                               
      template<class ALT_T>
      NOD() static constexpr Disowned<ALT_T> Nest(const ALT_T& value) noexcept {
         return Disowned<ALT_T>{value};
      }

      template<class ALT_T>
      using Nested = Disowned<ALT_T>;
   };
   
   /// Disown a value                                                         
   /// Same as a shallow-copy, but never references, saving some instructions 
   template<CT::NotSemantic T>
   NOD() constexpr auto Disown(const T& item) noexcept {
      return Disowned<T>{item};
   }
   

   ///                                                                        
   /// Cloned value intermediate type, use in constructors and assignments    
   /// to clone container, doing a deep copy instead of default shallow copy  
   ///   @tparam T - the type to clone                                        
   template<class T>
   struct Cloned : public A::Cloned {
      // Makes the type CT::Typed                                       
      using MemberType = T;

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

      /// Clone something else                                                
      template<class ALT_T>
      NOD() static constexpr Cloned<ALT_T> Nest(const ALT_T& value) noexcept {
         return Cloned<ALT_T>{value};
      }

      template<class ALT_T>
      using Nested = Cloned<ALT_T>;
   };
   
   /// Clone a value                                                          
   template<CT::NotSemantic T>
   NOD() constexpr auto Clone(const T& item) noexcept {
      return Cloned<T>{item};
   }

} // namespace Langulus

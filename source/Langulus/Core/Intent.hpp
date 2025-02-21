///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "TypeOf.hpp"
#include "../CT/POD.hpp"


namespace Langulus
{
   
   /// A namespace dedicated to abstract entities                             
   namespace A
   {

      ///                                                                     
      /// An abstract intent                                                  
      ///                                                                     
      struct Intent {
         using CTTI_ActAs = void;
         static constexpr bool CTTI_Abstract = true;
         static constexpr bool CTTI_Unallocatable = true;
      };

      /// An abstract shallow intent                                          
      /// Shallow intents are generally not propagated through indirections   
      struct ShallowIntent : Intent {
         static constexpr bool Shallow = true;
      };

      /// An abstract deep intent                                             
      /// Deep intents are propagated through indirection layers              
      struct DeepIntent : Intent {
         static constexpr bool Shallow = false;
      };

      /// An abstract refer intent                                            
      /// It doesn't copy anything but the container state, and references    
      /// data. For pointers and PODs, Referred is isomorphic to Copied       
      struct Referred : ShallowIntent {
         static constexpr bool Keep = true;
         static constexpr bool Move = false;
      };
      
      /// An abstract shallow-copy intent                                     
      /// A shallow-clone is like a Clone intent, but doesn't clone past      
      /// the first indirection layer. For pointers and PODs, Copied is       
      /// isomorphic to Referred                                              
      struct Copied : ShallowIntent {
         static constexpr bool Keep = true;
         static constexpr bool Move = false;
      };

      /// An abstract move intent                                             
      /// Moving transfers ownership and ensures, that the source is reset    
      /// and left in a safe invariant state.                                 
      struct Moved : ShallowIntent {
         static constexpr bool Keep = true;
         static constexpr bool Move = true;
      };

      /// An abstract abandon-move intent                                     
      /// Unlike the Move intent, this doesn't guarantee, that the moved      
      /// instance is left in a consistent and safe state. Instead, it only   
      /// makes sure, that it is safely destructible, saving some operations. 
      struct Abandoned : ShallowIntent {
         static constexpr bool Keep = false;
         static constexpr bool Move = true;
      };

      /// An abstract disowned-refer intent                                   
      /// Similar to Refer intent, but doesn't actually reference data,       
      /// producing what's essentially a cheap view over the data.            
      struct Disowned : ShallowIntent {
         static constexpr bool Keep = false;
         static constexpr bool Move = false;
      };

      /// An abstract clone intent (a.k.a. deep-copy intent)                  
      /// Copies all elements across all indirection layers into newly        
      /// allocated memory.                                                   
      struct Cloned : DeepIntent {
         static constexpr bool Keep = true;
         static constexpr bool Move = false;
      };

   } // namespace Langulus::A

   struct Describe;

   namespace CT
   {

      /// Checks if all T are intents                                         
      template<class...T>
      concept Intent = sizeof...(T) > 0
          and (DerivedFrom<T, A::Intent> and ...);

      /// Checks if all T are NOT intents                                     
      template<class...T>
      concept NoIntent = sizeof...(T) > 0
          and ((not Intent<T>) and ...);

      /// Checks if all T are shallow intents                                 
      template<class...T>
      concept ShallowIntent = sizeof...(T) > 0
          and (DerivedFrom<T, A::ShallowIntent> and ...);

      /// Checks if all T are deep intents                                    
      template<class...T>
      concept DeepIntent = sizeof...(T) > 0
          and (DerivedFrom<T, A::DeepIntent> and ...);

      /// Check if all T are refer intents                                    
      template<class...T>
      concept Referred = sizeof...(T) > 0
          and (DerivedFrom<T, A::Referred> and ...);
      
      /// Check if all T are copy intents                                     
      template<class...T>
      concept Copied = sizeof...(T) > 0
          and (DerivedFrom<T, A::Copied> and ...);

      /// Check if all T are move intents                                     
      template<class...T>
      concept Moved = sizeof...(T) > 0
          and (DerivedFrom<T, A::Moved> and ...);

      /// Check if all T are abandon intents                                  
      template<class...T>
      concept Abandoned = sizeof...(T) > 0
          and (DerivedFrom<T, A::Abandoned> and ...);

      /// Check if all T are disown intents                                   
      template<class...T>
      concept Disowned = sizeof...(T) > 0
          and (DerivedFrom<T, A::Disowned> and ...);

      /// Check if all T are clone intents                                    
      template<class...T>
      concept Cloned = sizeof...(T) > 0
          and (DerivedFrom<T, A::Cloned> and ...);

   } // namespace Langulus::CT


   /// Test if a semantic needs to be reset after a move                      
   template<template<class> class S>
   consteval bool ResetsOnMove() {
      static_assert(CT::Intent<S<int>>, "Provided type isn't an intent");
      return S<int>::Keep and S<int>::Move;
   }

   /// Test if a semantic needs to be reset after a move                      
   template<CT::Intent S>
   consteval bool ResetsOnMove() {
      return S::Keep and S::Move;
   }


   ///                                                                        
   /// Referred value intermediate type, use in constructors and assignments  
   /// to refer to data explicitly                                            
   ///   @tparam T - the type to refer                                        
   template<class T>
   struct Referred : A::Referred {
   private:
      const T& mValue;

   public:
      using CTTI_InnerType = T;

      Referred() = delete;
      explicit constexpr Referred(const Referred&) noexcept = default;
      explicit constexpr Referred(Referred&&) noexcept = default;

      LANGULUS(ALWAYS_INLINED)
      explicit constexpr Referred(const T& value) noexcept
         : mValue {value} {
         static_assert(CT::NoIntent<T>, "Can't nest intents");
      }
      
      /// Forward as referred                                                 
      ///   @tparam ALT_T - optional type to forward as                       
      ///   @return the desired new type with the same refer intent applied   
      template<class ALT_T = T> LANGULUS(ALWAYS_INLINED)
      constexpr decltype(auto) Forward() const noexcept {
         static_assert(CT::NoIntent<ALT_T>,
            "Can't nest intents");
         static_assert(CT::DerivedFrom<T, ALT_T>,
            "Can't forward as this type");

         // Aggregates don't play well with intents, so if type is an   
         // aggregate, use the standard copy semantics                  
         if constexpr (CT::Aggregate<ALT_T>)
            return static_cast<const ALT_T&>(mValue);
         else
            return Referred<ALT_T> {mValue};
      }

      /// Refer something else                                                
      ///   @param value - the value to refer (can be an intent)              
      ///   @return the referred value, disregarding previous intent          
      LANGULUS(ALWAYS_INLINED)
      static constexpr decltype(auto) Nest(auto&& value) noexcept {
         using ALT = Decq<Deref<decltype(value)>>;

         // Aggregates don't play well with intents, so if type is an   
         // aggregate, use the standard copy semantics                  
         if constexpr (CT::Intent<ALT>) {
            using ALT_T = TypeOf<ALT>;

            if constexpr (CT::Aggregate<ALT_T>)
               return *value;
            else
               return Referred<TypeOf<ALT>> {*value};
         }
         else {
            if constexpr (CT::Aggregate<ALT>)
               return value;
            else
               return Referred<ALT> {value};
         }
      }

      template<class ALT>
      using As = Conditional<CT::Intent<ALT>, Referred<TypeOf<ALT>>, Referred<ALT>>;

      LANGULUS(ALWAYS_INLINED)
      const T& operator * () const noexcept { return mValue; }

      LANGULUS(ALWAYS_INLINED)
      auto operator -> () const noexcept {
         if constexpr (CT::Sparse<T>)
            return mValue;
         else
            return &mValue;
      }

      /// Implicitly collapse the intent                                      
      /// This way this wrapper is seamlessly integrated with the standard    
      /// C++20 copy semantics                                                
      LANGULUS(ALWAYS_INLINED)
      constexpr operator const T& () const& noexcept {
         return mValue;
      }
      LANGULUS(ALWAYS_INLINED)
      constexpr operator const T& () & noexcept {
         return mValue;
      }
      LANGULUS(ALWAYS_INLINED)
      constexpr operator const T& () && noexcept {
         return mValue;
      }
   };
   
   /// Refer a value                                                          
   LANGULUS(ALWAYS_INLINED)
   constexpr auto Refer(auto&& value) noexcept {
      using ALT = Decq<Deref<decltype(value)>>;
      if constexpr (CT::Intent<ALT>)
         return Referred<TypeOf<ALT>> {*value};
      else
         return Referred<ALT> {value};
   }
   
   
   ///                                                                        
   /// Copied value intermediate type, use in constructors and assignments    
   /// to shallow-copy container explicitly                                   
   ///   @tparam T - the type to copy                                         
   template<class T>
   struct Copied : A::Copied {
   private:
      const T& mValue;

   public:
      using CTTI_InnerType = T;

      Copied() = delete;
      constexpr Copied(const Copied&) noexcept = default;
      explicit constexpr Copied(Copied&&) noexcept = default;

      LANGULUS(ALWAYS_INLINED)
      explicit constexpr Copied(const T& value) noexcept
         : mValue {value} {
         static_assert(CT::NoIntent<T>, "Can't nest intents");
      }
      
      /// Forward as copied                                                   
      ///   @tparam ALT_T - optional type to forward as                       
      ///   @return the desired new type with the same copy intent applied    
      template<class ALT_T = T> LANGULUS(ALWAYS_INLINED)
      constexpr decltype(auto) Forward() const noexcept {
         static_assert(CT::NoIntent<ALT_T>,
            "Can't nest intents");
         static_assert(CT::DerivedFrom<T, ALT_T>,
            "Can't forward as this type");

         // Aggregates don't play well with intents, so if type is an   
         // aggregate, use the standard copy semantics                  
         if constexpr (CT::Aggregate<ALT_T>)
            return static_cast<const ALT_T&>(mValue);
         else
            return Copied<ALT_T> {mValue};
      }

      /// Copy something else                                                 
      ///   @param value - the value to copy (can be an intent)               
      ///   @return the copied value, disregarding previous intent            
      LANGULUS(ALWAYS_INLINED)
      static constexpr decltype(auto) Nest(auto&& value) noexcept {
         using ALT = Decq<Deref<decltype(value)>>;

         // Aggregates don't play well with intents, so if type is an   
         // aggregate, use the standard copy semantics                  
         if constexpr (CT::Intent<ALT>) {
            using ALT_T = TypeOf<ALT>;

            if constexpr (CT::Aggregate<ALT_T>)
               return *value;
            else
               return Copied<TypeOf<ALT>> {*value};
         }
         else {
            if constexpr (CT::Aggregate<ALT>)
               return value;
            else
               return Copied<ALT> {value};
         }
      }

      template<class ALT>
      using As = Conditional<CT::Intent<ALT>, Copied<TypeOf<ALT>>, Copied<ALT>>;

      LANGULUS(ALWAYS_INLINED)
      const T& operator * () const noexcept { return mValue; }

      LANGULUS(ALWAYS_INLINED)
      auto operator -> () const noexcept {
         if constexpr (CT::Sparse<T>)
            return mValue;
         else
            return &mValue;
      }

      /// Implicitly collapse the intent, when applying it to POD/Sparse,     
      /// since Refer is isomorphic to Copy in those cases                    
      LANGULUS(ALWAYS_INLINED)
      constexpr operator const T& () const noexcept
      requires (CT::POD<T> or CT::Sparse<T>) {
         return mValue;
      }

      /// Used by DecayCast                                                   
      LANGULUS(ALWAYS_INLINED)
      constexpr const T& TypedCast() const noexcept {
         return mValue;
      }
   };
   
   /// Copy a value                                                           
   LANGULUS(ALWAYS_INLINED)
   constexpr auto Copy(auto&& value) noexcept {
      using ALT = Decq<Deref<decltype(value)>>;
      if constexpr (CT::Intent<ALT>)
         return Copied<TypeOf<ALT>> {*value};
      else
         return Copied<ALT> {value};
   }
   

   ///                                                                        
   /// Moved value intermediate type, use in constructors and assignments     
   /// to move data explicitly                                                
   ///   @tparam T - the type to move                                         
   template<class T>
   struct Moved : A::Moved {
   protected:
      T&& mValue;

   public:
      using CTTI_InnerType = T;

      Moved() = delete;
      constexpr Moved(const Moved& r) noexcept
         : mValue {::std::forward<T>(r.mValue)} {}
      explicit constexpr Moved(Moved&&) noexcept = default;

      LANGULUS(ALWAYS_INLINED)
      explicit constexpr Moved(T& value) noexcept
         : mValue {::std::move(value)} {
         static_assert(CT::NoIntent<T>, "Can't nest intents");
      }

      LANGULUS(ALWAYS_INLINED)
      explicit constexpr Moved(T&& value) noexcept
         : mValue {::std::forward<T>(value)} {
         static_assert(CT::NoIntent<T>, "Can't nest intents");
      }

      /// Forward as moved                                                    
      ///   @tparam ALT_T - optional type to forward as                       
      ///   @return the desired new type with the same move intent applied    
      template<class ALT_T = T> LANGULUS(ALWAYS_INLINED)
      constexpr decltype(auto) Forward() const noexcept {
         static_assert(CT::NoIntent<ALT_T>,
            "Can't nest intents");
         static_assert(CT::DerivedFrom<T, ALT_T>,
            "Can't forward as this type");
         
         // Aggregates don't play well with intents, so if type is an   
         // aggregate, use the standard move semantics                  
         if constexpr (CT::Aggregate<ALT_T>)
            return ::std::forward<ALT_T>(mValue);
         else
            return Moved<ALT_T> {::std::forward<ALT_T>(mValue)};
      }

      /// Move something else                                                 
      ///   @param value - the value to move (can be an intent)               
      ///   @return the moved value, disregarding previous intent             
      LANGULUS(ALWAYS_INLINED)
      static constexpr decltype(auto) Nest(auto&& value) noexcept {
         using ALT = Decq<Deref<decltype(value)>>;

         // Aggregates don't play well with intents, so if type is an   
         // aggregate, use the standard move semantics                  
         if constexpr (CT::Intent<ALT>) {
            using ALT_T = TypeOf<ALT>;

            if constexpr (CT::Aggregate<ALT_T>)
               return ::std::forward<ALT_T>(*value);
            else
               return Moved<ALT_T> {::std::forward<ALT_T>(*value)};
         }
         else {
            if constexpr (CT::Aggregate<ALT>)
               return ::std::forward<ALT>(value);
            else
               return Moved<ALT> {::std::forward<ALT>(value)};
         }
      }

      template<class ALT>
      using As = Conditional<CT::Intent<ALT>, Moved<TypeOf<ALT>>, Moved<ALT>>;

      LANGULUS(ALWAYS_INLINED)
      T& operator * () const noexcept { return mValue; }

      LANGULUS(ALWAYS_INLINED)
      auto operator -> () const noexcept {
         if constexpr (CT::Sparse<T>)
            return mValue;
         else
            return &mValue;
      }

      /// Implicitly collapse the intent                                      
      /// This way this wrapper is seamlessly integrated with the standard    
      /// C++20 move-semantics                                                
      LANGULUS(ALWAYS_INLINED)
      constexpr operator T&& () const noexcept {
         return ::std::forward<T>(mValue);
      }

      LANGULUS(ALWAYS_INLINED)
      constexpr operator T&& () noexcept {
         return ::std::forward<T>(mValue);
      }

      /// Used by DecayCast                                                   
      LANGULUS(ALWAYS_INLINED)
      constexpr T& TypedCast() const noexcept {
         return mValue;
      }
   };
   
   /// Move a value                                                           
   LANGULUS(ALWAYS_INLINED)
   constexpr auto Move(auto&& value) noexcept {
      using ALT = Decq<Deref<decltype(value)>>;
      if constexpr (CT::Intent<ALT>)
         return Moved<TypeOf<ALT>> {::std::forward<TypeOf<ALT>>(*value)};
      else
         return Moved<ALT> {::std::forward<ALT>(value)};
   }


   ///                                                                        
   /// Abandoned value intermediate type, can be used in constructors and     
   /// assignments to provide a guarantee, that the value shall not be used   
   /// after that function, so we can save up on resetting it fully           
   /// For example, you can construct a Many with an abandoned Many, which is 
   /// same as move-construction, but the abandoned Many shall have only its  
   /// mEntry reset, instead of the entire container.                         
   ///   @tparam T - the type to abandon                                      
   template<class T>
   struct Abandoned : A::Abandoned {
   protected:
      T&& mValue;

   public:
      using CTTI_InnerType = T;

      Abandoned() = delete;
      constexpr Abandoned(const Abandoned& r) noexcept
         : mValue {::std::forward<T>(r.mValue)} {}
      explicit constexpr Abandoned(Abandoned&&) noexcept = default;
      
      LANGULUS(ALWAYS_INLINED)
      explicit constexpr Abandoned(T& value) noexcept
         : mValue {::std::move(value)} {
         static_assert(CT::NoIntent<T>, "Can't nest intents");
      }
      
      LANGULUS(ALWAYS_INLINED)
      explicit constexpr Abandoned(T&& value) noexcept
         : mValue {::std::forward<T>(value)} {
         static_assert(CT::NoIntent<T>, "Can't nest intents");
      }
      
      /// Forward as abandoned                                                
      ///   @tparam ALT_T - optional type to forward as                       
      ///   @return the desired new type with the same move intent applied    
      template<class ALT_T = T> LANGULUS(ALWAYS_INLINED)
      constexpr decltype(auto) Forward() const noexcept {
         static_assert(CT::NoIntent<ALT_T>,
            "Can't nest intents");
         static_assert(CT::DerivedFrom<T, ALT_T>,
            "Can't forward as this type");
         
         // Aggregates don't play well with intents, so if type is an   
         // aggregate, use the standard move semantics                  
         if constexpr (CT::Aggregate<ALT_T>)
            return ::std::forward<ALT_T>(mValue);
         else
            return Abandoned<ALT_T> {::std::forward<ALT_T>(mValue)};
      }

      /// Abandon something else                                              
      ///   @param value - the value to abandon (can be an intent)            
      ///   @return the abandoned value, disregarding previous intent         
      LANGULUS(ALWAYS_INLINED)
      static constexpr decltype(auto) Nest(auto&& value) noexcept {
         using ALT = Decq<Deref<decltype(value)>>;

         // Aggregates don't play well with intents, so if type is an   
         // aggregate, use the standard move semantics                  
         if constexpr (CT::Intent<ALT>) {
            using ALT_T = TypeOf<ALT>;

            if constexpr (CT::Aggregate<ALT_T>)
               return ::std::forward<ALT_T>(*value);
            else
               return Abandoned<ALT_T> {::std::forward<ALT_T>(*value)};
         }
         else {
            if constexpr (CT::Aggregate<ALT>)
               return ::std::forward<ALT>(value);
            else
               return Abandoned<ALT> {::std::forward<ALT>(value)};
         }
      }

      template<class ALT>
      using As = Conditional<CT::Intent<ALT>, Abandoned<TypeOf<ALT>>, Abandoned<ALT>>;

      LANGULUS(ALWAYS_INLINED)
      T& operator * () const noexcept { return mValue; }

      LANGULUS(ALWAYS_INLINED)
      auto operator -> () const noexcept {
         if constexpr (CT::Sparse<T>)
            return mValue;
         else
            return &mValue;
      }

      /// Implicitly collapse the intent                                      
      /// This way this wrapper is seamlessly integrated with the standard    
      /// C++20 move-semantics                                                
      LANGULUS(ALWAYS_INLINED)
      constexpr operator T&& () const noexcept {
         return ::std::forward<T>(mValue);
      }

      /// Used by DecayCast                                                   
      LANGULUS(ALWAYS_INLINED)
      constexpr T& TypedCast() const noexcept {
         return mValue;
      }
   };
   
   /// Abandon a value                                                        
   /// Same as Move, but resets only mandatory data inside source after move  
   /// essentially saving up on a couple of instructions                      
   LANGULUS(ALWAYS_INLINED)
   constexpr auto Abandon(auto&& value) noexcept {
      using ALT = Decq<Deref<decltype(value)>>;
      if constexpr (CT::Intent<ALT>) {
         return Abandoned<TypeOf<ALT>> {
            ::std::forward<TypeOf<ALT>>(*value)};
      }
      else {
         return Abandoned<ALT> {
            ::std::forward<ALT>(value)};
      }
   }


   ///                                                                        
   /// Disowned value intermediate type, use in constructors and assignments  
   /// to copy container without gaining ownership                            
   ///   @tparam T - the type to disown                                       
   template<class T>
   struct Disowned : A::Disowned {
   protected:
      const T& mValue;

   public:
      using CTTI_InnerType = T;

      Disowned() = delete;
      constexpr Disowned(const Disowned&) noexcept = default;
      explicit constexpr Disowned(Disowned&&) noexcept = default;

      LANGULUS(ALWAYS_INLINED)
      explicit constexpr Disowned(const T& value) noexcept
         : mValue {value} {
         static_assert(CT::NoIntent<T>, "Can't nest intents");
      }

      
      /// Forward as disowned                                                 
      ///   @tparam ALT_T - optional type to forward as                       
      ///   @return the desired new type with the same disown intent applied  
      template<class ALT_T = T> LANGULUS(ALWAYS_INLINED)
      constexpr decltype(auto) Forward() const noexcept {
         static_assert(CT::NoIntent<ALT_T>,
            "Can't nest intents");
         static_assert(CT::DerivedFrom<T, ALT_T>,
            "Can't forward as this type");

         // Aggregates don't play well with intents, so if type is an   
         // aggregate, use the standard copy semantics                  
         if constexpr (CT::Aggregate<ALT_T>)
            return static_cast<const ALT_T&>(mValue);
         else
            return Disowned<ALT_T> {mValue};
      }

      /// Disown something else                                               
      ///   @param value - the value to disown (can be an intent)             
      ///   @return the disowned value, disregarding previous intent          
      LANGULUS(ALWAYS_INLINED)
      static constexpr decltype(auto) Nest(auto&& value) noexcept {
         using ALT = Decq<Deref<decltype(value)>>;

         // Aggregates don't play well with intents, so if type is an   
         // aggregate, use the standard copy semantics                  
         if constexpr (CT::Intent<ALT>) {
            using ALT_T = TypeOf<ALT>;

            if constexpr (CT::Aggregate<ALT_T>)
               return *value;
            else
               return Disowned<TypeOf<ALT>> {*value};
         }
         else {
            if constexpr (CT::Aggregate<ALT>)
               return value;
            else
               return Disowned<ALT> {value};
         }
      }

      template<class ALT>
      using As = Conditional<CT::Intent<ALT>, Disowned<TypeOf<ALT>>, Disowned<ALT>>;
      
      LANGULUS(ALWAYS_INLINED)
      const T& operator * () const noexcept { return mValue; }

      LANGULUS(ALWAYS_INLINED)
      auto operator -> () const noexcept {
         if constexpr (CT::Sparse<T>)
            return mValue;
         else
            return &mValue;
      }

      /// Implicitly collapse the intent, when applying it to PODs,           
      /// since they are never allowed to have ownership either way           
      LANGULUS(ALWAYS_INLINED)
      constexpr operator const T& () const noexcept requires CT::POD<T> {
         return mValue;
      }

      /// Used by DecayCast                                                   
      LANGULUS(ALWAYS_INLINED)
      constexpr const T& TypedCast() const noexcept {
         return mValue;
      }
   };
   
   /// Disown a value                                                         
   /// Same as a shallow-copy, but never references, saving some instructions 
   LANGULUS(ALWAYS_INLINED)
   constexpr auto Disown(auto&& value) noexcept {
      using ALT = Decq<Deref<decltype(value)>>;
      if constexpr (CT::Intent<ALT>)
         return Disowned<TypeOf<ALT>> {*value};
      else
         return Disowned<ALT> {value};
   }
   

   ///                                                                        
   /// Cloned value intermediate type, used in constructors and assignments   
   /// to clone container, doing a deep copy instead of default shallow one   
   ///   @tparam T - the type to clone                                        
   template<class T>
   struct Cloned : A::Cloned {
   protected:
      const T& mValue;

   public:
      using CTTI_InnerType = T;

      Cloned() = delete;
      constexpr Cloned(const Cloned&) noexcept = default;
      explicit constexpr Cloned(Cloned&&) noexcept = default;

      LANGULUS(ALWAYS_INLINED)
      explicit constexpr Cloned(const T& value) noexcept
         : mValue {value} {
         static_assert(CT::NoIntent<T>, "Can't nest intents");
      }

      /// Forward as cloned, never collapse                                   
      template<class ALT_T = T> LANGULUS(ALWAYS_INLINED)
      constexpr decltype(auto) Forward() const noexcept {
         static_assert(CT::NoIntent<ALT_T>,
            "Can't nest intents");
         static_assert(CT::DerivedFrom<T, ALT_T>,
            "Can't forward as this type");
         return Cloned<ALT_T> {mValue};
      }

      /// Clone something else                                                
      LANGULUS(ALWAYS_INLINED)
      static constexpr decltype(auto) Nest(auto&& value) noexcept {
         using ALT = Decq<Deref<decltype(value)>>;
         if constexpr (CT::Intent<ALT>)
            return Cloned<TypeOf<ALT>> {*value};
         else
            return Cloned<ALT> {value};
      }

      template<class ALT>
      using As = Conditional<CT::Intent<ALT>, Cloned<TypeOf<ALT>>, Cloned<ALT>>;
      
      LANGULUS(ALWAYS_INLINED)
      const T& operator * () const noexcept { return mValue; }

      LANGULUS(ALWAYS_INLINED)
      auto operator -> () const noexcept {
         if constexpr (CT::Sparse<T>)
            return mValue;
         else
            return &mValue;
      }

      /// Implicitly collapse the intent, when applying it to PODs,           
      /// since they are always cloned upon copy (ONLY IF `T` IS DENSE)       
      LANGULUS(ALWAYS_INLINED)
      constexpr operator const T& () const noexcept
      requires (CT::POD<T> and CT::Dense<T>) {
         return mValue;
      }

      /// Used by DecayCast                                                   
      LANGULUS(ALWAYS_INLINED)
      constexpr const T& TypedCast() const noexcept {
         return mValue;
      }
   };
   
   /// Clone a value                                                          
   /// Does a deep-copy                                                       
   LANGULUS(ALWAYS_INLINED)
   constexpr auto Clone(auto&& value) noexcept {
      using ALT = Decq<Deref<decltype(value)>>;
      if constexpr (CT::Intent<ALT>)
         return Cloned<TypeOf<ALT>> {*value};
      else
         return Cloned<ALT> {value};
   }


   ///                                                                        
   /// Descriptor intermediate type, use in constructors to enable descriptor 
   /// construction. The inner type is always Anyness::Many                   
   struct Describe : A::Intent {
   protected:
      using Many = Anyness::Many;
      const Many& mValue;

   public:
      Describe() = delete;
      constexpr Describe(const Describe&) noexcept = default;
      explicit constexpr Describe(Describe&&) noexcept = default;

      LANGULUS(ALWAYS_INLINED)
      explicit constexpr Describe(const Many& value) noexcept
         : mValue {value} {}

      /// Forward as descibe                                                  
      LANGULUS(ALWAYS_INLINED)
      constexpr Describe&& Forward() noexcept {
         return static_cast<Describe&&>(*this);
      }

      /// The describe intent completely ignores nesting, only propagates     
      /// itself                                                              
      LANGULUS(ALWAYS_INLINED)
      static constexpr decltype(auto) Nest(auto&& value) noexcept {
         using ALT = Decq<Deref<decltype(value)>>;
         if constexpr (CT::Similar<ALT, Describe>)
            return Forward<ALT>(value);
         else if constexpr (CT::Intent<ALT> and CT::Similar<TypeOf<ALT>, Many>)
            return Describe {*value};
         else if constexpr (CT::Similar<ALT, Many>)
            return Describe {value};
         else
            static_assert(false, "Can't nest provided type as a Describe semantic");
      }

      LANGULUS(ALWAYS_INLINED)
      const auto& operator *  () const noexcept { return  mValue; }

      LANGULUS(ALWAYS_INLINED)
      const auto* operator -> () const noexcept { return &mValue; }
   };

   
   namespace CT
   {

      ///                                                                     
      ///   Intent type traits                                                
      ///                                                                     
      ///   These concepts are strict requirements, and are true only if the  
      /// corresponding constructors/assigners are implicitly/explicitly      
      /// defined. No fallbacks!                                              
      ///                                                                     

      /// Check if all T have intent constructors for S                       
      ///   @tparam S - the intent                                            
      ///   @tparam T... - the types                                          
      template<template<class> class S, class...T>
      concept HasIntentConstructor = Complete<T...> and Intent<S<T>...>
          and requires (S<T>&&...a) { (T (Forward<S<T>>(a)), ...); };

      /// Check if all TypeOf<S> have intent constructors for S               
      ///   @tparam S - the intent and type                                   
      template<class...S>
      concept HasIntentConstructorAlt = Complete<TypeOf<S>...> and Intent<S...>
          and requires (S&&...a) { (TypeOf<S> (Forward<S>(a)), ...); };

      /// Check if all T have a disown-constructor                            
      /// Disowning does a shallow copy without referencing contents,         
      /// generating a 'view' of the data that is without ownership.          
      template<class...T>
      concept HasDisownConstructor =
         (HasIntentConstructor<Langulus::Disowned, T> and ...);

      /// Check if all Decay<T> have a clone-constructor                      
      /// Does a deep copy                                                    
      template<class...T>
      concept HasCloneConstructor =
         (HasIntentConstructor<Langulus::Cloned, T> and ...);

      /// Check if all T have a abandon-constructor                           
      /// Does a move, but doesn't fully reset source (optimization)          
      template<class...T>
      concept HasAbandonConstructor =
         (HasIntentConstructor<Langulus::Abandoned, T> and ...);

      /// Check if all T have a refer-constructor                             
      /// Refering does a shallow copy while referencing contents, providing  
      /// ownership.                                                          
      /// T has refer-constructor as long as it is std::copy_constuctible     
      template<class...T>
      concept HasReferConstructor = Complete<T...>
          and ((HasIntentConstructor<Langulus::Referred, T>
           or ::std::copy_constructible<T>) and ...);
      
      /// Check if all T have a copy-constructor (don't mistake it for a      
      /// std::copy_constructible!)                                           
      /// Does a shallow copy _of the contents_ (like shallow cloning).       
      template<class...T>
      concept HasCopyConstructor =
         (HasIntentConstructor<Langulus::Copied, T> and ...);

      /// Check if all T have a move-constructor                              
      /// Does a move, fully resetting source                                 
      /// T has move-constructor as long as it is std::move_constuctible      
      template<class...T>
      concept HasMoveConstructor = Complete<T...> and ((Sparse<T>
           or HasIntentConstructor<Langulus::Moved, T>
           or ::std::move_constructible<T>) and ...);

      /// Check if all T have an intent-assigner for S                        
      ///   @tparam S - the intent                                            
      ///   @tparam T... - the types                                          
      template<template<class> class S, class...T>
      concept HasIntentAssign = Complete<T...> and ((Intent<S<T>>
          and ::std::assignable_from<T&, S<T>&&>) and ...);

      /// Check if all TypeOf<S> has intent-assigner for S                    
      ///   @tparam S - the intent and type                                   
      template<class...S>
      concept HasIntentAssignAlt = Complete<TypeOf<S>...> and ((Intent<S>
          and ::std::assignable_from<TypeOf<S>&, S&&>) and ...);

      /// Check if all T have a disown-assigner                               
      /// Disowning does a shallow copy without referencing contents,         
      /// generating a 'view' of the data that is without ownership.          
      template<class...T>
      concept HasDisownAssign =
         (HasIntentAssign<Langulus::Disowned, T> and ...);

      /// Check if all Decay<T> have a clone-assigner                         
      /// Does a deep copy                                                    
      template<class...T>
      concept HasCloneAssign =
         (HasIntentAssign<Langulus::Cloned, T> and ...);

      /// Check if all T have an abandon-assigner                             
      /// Does a move, but doesn't fully reset source (optimization)          
      template<class...T>
      concept HasAbandonAssign =
         (HasIntentAssign<Langulus::Abandoned, T> and ...);

      /// Check if all T have refer-assigner                                  
      /// Refering does a shallow copy while referencing contents, providing  
      /// ownership.                                                          
      /// T has a refer-assigner as long as std::copy_assignable<T> holds     
      template<class...T>
      concept HasReferAssign = Complete<T...>
          and ((HasIntentAssign<Langulus::Referred, T>
           or ::std::assignable_from<T&, const T&>) and ...);
      
      /// Check if all T have a copy-assigner (don't mistake it for a         
      /// std::copy_assignable!)                                              
      /// Does a shallow copy _of the contents_ (like shallow cloning).       
      template<class...T>
      concept HasCopyAssign =
         (HasIntentAssign<Langulus::Copied, T> and ...);

      /// Check if all T have a move-assigner                                 
      /// Does a move, fully resetting source                                 
      /// T has a move-assigner as long as std::assignable_from<T&, T&&> holds
      /// This includes the cases when the type has a default copy-assign to  
      /// which the compiler falls back to. In that case move-assignment is   
      /// the same as refer-assignment.                                       
      template<class...T>
      concept HasMoveAssign = Complete<T...>
          and ((HasIntentAssign<Langulus::Moved, T>
           or ::std::assignable_from<T&, T&&>) and ...);

   } // namespace Langulus::CT

   
   /// Deduce the proper intent, based on whether T already has a             
   /// specified intent, is an rvalue (&&), or none of those                  
   /// If it has one of those, then we get move intent; if it isn't - we      
   /// get refer intent (which can fallback to copy semantics)                
   template<class T>
   using IntentOf = Conditional<CT::Intent<T>, Decay<T>, 
      Conditional<::std::is_rvalue_reference_v<T> and CT::Mutable<Deref<T>>,
         Moved<Deref<T>>, Referred<Deref<T>>>>;

   /// Shed the intent from a type, if any                                    
   template<class T>
   using Deint = Conditional<CT::Intent<T>, TypeOf<T>, T>;

   /// Decay an intent to the contained data                                  
   ///   @param what - the instance to decay                                  
   ///   @return a reference (preferably) or a copy of the inner data         
   LANGULUS(ALWAYS_INLINED)
   constexpr auto& DeintCast(auto&& what) noexcept {
      using T = decltype(what);
      if constexpr (CT::Intent<T>)
         return TypedCast(Forward<T>(what));
      else
         return what;
   }

} // namespace Langulus

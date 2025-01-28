///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// SPDX-License-Identifier: MIT                                              
///                                                                           
#pragma once
#include "Common.hpp"
#include <stdexcept>


namespace Langulus
{

   ///                                                                        
   ///   General exception                                                    
   ///                                                                        
   /// It is an equivalent to std::runtime_error, but with additional info    
   /// for debug builds, like message and location strings                    
   ///                                                                        
   class Exception {
      #if LANGULUS(DEBUG)
      protected:
         // Exception message                                           
         Token mMessage;
         // Exception location, as a separate literal to avoid concat   
         Token mLocation;
      #endif

   public:
      /// Get exception name                                                  
      ///   @return the name of the exception                                 
      virtual Token GetName() const noexcept {
         return "Unspecified";
      }

   #if LANGULUS(DEBUG)
      /// Default exception constructor                                       
      LANGULUS(INLINED)
      constexpr Exception() noexcept
         : mMessage {"<no information provided>"} { }

      /// Manual exception constructor                                        
      ///   @param what - custom message about the exception                  
      ///   @param location - location of exception, see LANGULUS_LOCATION()  
      LANGULUS(INLINED)
      constexpr Exception(
         const char* what,
         const char* location = "<unknown location>"
      ) noexcept
         : mMessage {what}
         , mLocation {location} { }

      /// Get exception message                                               
      ///   @return the message of the exception                              
      LANGULUS(INLINED)
      constexpr const Token& GetMessage() const noexcept {
         return mMessage;
      }

      /// Get exception location                                              
      ///   @return the location of the exception                             
      LANGULUS(INLINED)
      constexpr const Token& GetLocation() const noexcept {
         return mLocation;
      }
   #else
      /// Default exception constructor                                       
      constexpr Exception() noexcept = default;

      /// Manual exception constructor                                        
      ///   @param what - custom message about the exception                  
      ///   @param location - location of exception, see LANGULUS_LOCATION()  
      LANGULUS(INLINED)
      constexpr Exception(const char*, const char* = "<unknown location>") noexcept { }
   #endif
   };

   /// Make sure this is not inlined as it is slow and dramatically enlarges  
   /// code, thus making other inlinings more difficult                       
   /// Throws are also generally the slow path                                
   template <class E, class... Args>
   [[noreturn]] LANGULUS(NOINLINE) void Throw(Args&&... args) {
      throw E {::std::forward<Args>(args)...};
   }

   namespace CT
   {
      template<class...T>
      concept Exception = (CT::DerivedFrom<T, ::Langulus::Exception> and ...);
   }

} // namespace Langulus


/// Convenience macro of declaring an exception type                          
///   @param name - the name of the exception type                            
///                 it will be declared in namespace ::Langulus::Except       
#define LANGULUS_EXCEPTION(name) \
   namespace Langulus::Except \
   { \
      struct name : ::Langulus::Exception { \
         using Exception::Exception; \
         Token GetName() const noexcept override { \
            return #name; \
         } \
      }; \
   }


/// Here's some standard exception definitions                                
LANGULUS_EXCEPTION(Read);
LANGULUS_EXCEPTION(Write);
LANGULUS_EXCEPTION(Copy);
LANGULUS_EXCEPTION(Clone);
LANGULUS_EXCEPTION(Move);
LANGULUS_EXCEPTION(Access);
LANGULUS_EXCEPTION(Convert);
LANGULUS_EXCEPTION(Concat);
LANGULUS_EXCEPTION(Compare);
LANGULUS_EXCEPTION(Allocate);
LANGULUS_EXCEPTION(Deallocate);
LANGULUS_EXCEPTION(Reallocate);
LANGULUS_EXCEPTION(Mutate);
LANGULUS_EXCEPTION(Construct);
LANGULUS_EXCEPTION(Destruct);
LANGULUS_EXCEPTION(Assign);
LANGULUS_EXCEPTION(Reference);
LANGULUS_EXCEPTION(Overflow);
LANGULUS_EXCEPTION(Underflow);
LANGULUS_EXCEPTION(ZeroDivision);
LANGULUS_EXCEPTION(OutOfRange);
LANGULUS_EXCEPTION(Assertion);
LANGULUS_EXCEPTION(ToDo);

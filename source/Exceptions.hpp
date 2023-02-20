///                                                                           
/// Langulus::Core                                                            
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>                    
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Common.hpp"
#include <stdexcept>

namespace Langulus
{

   ///                                                                        
   ///   Common Langulus exception                                            
   ///                                                                        
   class Exception {
   protected:
      // Exception message                                              
      Token mMessage;
      // Exception location, as a separate literal to avoid concat      
      Token mLocation;

   public:
      /// Default exception constructor                                       
      constexpr Exception() noexcept
         : mMessage {"<no information provided>"} { }

      /// Manual exception constructor                                        
      ///   @param what - custom message about the exception                  
      ///   @param location - location of exception, see LANGULUS_LOCATION()  
      constexpr Exception(const char* what, const char* location = "<unknown location>") noexcept
         : mMessage {what}
         , mLocation {location} { }

      /// Get exception name                                                  
      ///   @return the name of the exception                                 
      virtual Token GetName() const noexcept {
         return "Unknown";
      }

      /// Get exception message                                               
      ///   @return the message of the exception                              
      LANGULUS(ALWAYSINLINE)
      const Token& GetMessage() const noexcept {
         return mMessage;
      }

      /// Get exception location                                              
      ///   @return the location of the exception                             
      LANGULUS(ALWAYSINLINE)
      const Token& GetLocation() const noexcept {
         return mLocation;
      }
   };

   /// Make sure this is not inlined as it is slow and dramatically enlarges  
   /// code, thus making other inlinings more difficult                       
   /// Throws are also generally the slow path                                
   template <class E, class... Args>
   [[noreturn]] LANGULUS(NOINLINE) void Throw(Args&&... args) {
      throw E {::std::forward<Args>(args)...};
   }

} // namespace Langulus


/// Convenience macro of declaring an exception type                          
///   @param name - the name of the exception type                            
///                 it will be declared in namespace ::Langulus::Except       
#define LANGULUS_EXCEPTION(name) \
   namespace Langulus::Except \
   { \
      struct name : public ::Langulus::Exception { \
         using Exception::Exception; \
         Token GetName() const noexcept override { \
            return #name; \
         } \
      }; \
   }


/// Here's some standard exception definitions                                
LANGULUS_EXCEPTION(Copy);
LANGULUS_EXCEPTION(Clone);
LANGULUS_EXCEPTION(Move);
LANGULUS_EXCEPTION(Access);
LANGULUS_EXCEPTION(Convert);
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
LANGULUS_EXCEPTION(TODO);

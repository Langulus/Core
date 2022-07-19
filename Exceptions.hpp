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
   ///   A common Langulus exception                                          
   ///                                                                        
   struct Exception : public ::std::runtime_error {
      Exception() noexcept
         : ::std::runtime_error {"<no information provided>"} { }
      Exception(const char* what) noexcept
         : ::std::runtime_error {what} { }

      /// Get exception name                                                  
      ///   @return the name of the exception                                 
      virtual Token GetName() const noexcept {
         return "Unknown";
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

/// Convenience macro of declaring an exception                               
#define LANGULUS_EXCEPTION(name) \
   namespace Langulus::Except \
   { \
      struct name : public Exception { \
         using Exception::Exception; \
         Token GetName() const noexcept override { \
            return #name; \
         } \
      }; \
   }

/// Here's some standard exception definitions                                
LANGULUS_EXCEPTION(Copy);
LANGULUS_EXCEPTION(Move);
LANGULUS_EXCEPTION(Access);
LANGULUS_EXCEPTION(Convert);
LANGULUS_EXCEPTION(Allocate);
LANGULUS_EXCEPTION(Deallocate);
LANGULUS_EXCEPTION(Reallocate);
LANGULUS_EXCEPTION(Mutate);
LANGULUS_EXCEPTION(Construct);
LANGULUS_EXCEPTION(Destruct);
LANGULUS_EXCEPTION(Reference);
LANGULUS_EXCEPTION(Overflow);
LANGULUS_EXCEPTION(Underflow);
LANGULUS_EXCEPTION(ZeroDivision);
LANGULUS_EXCEPTION(OutOfRange);

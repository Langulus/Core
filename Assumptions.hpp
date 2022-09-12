///																									
/// Langulus::Core																			   
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>					      
///																									
/// Distributed under GNU General Public License v3+									
/// See LICENSE file, or https://www.gnu.org/licenses									
///																									
#pragma once
#include "Exceptions.hpp"

namespace Langulus
{

	/// Common Langulus assertion																
	///	@tparam LEVEL - level of the assumption, configurable from CMake,		
	/// using LANGULUS_ENABLE_ASSERTION_LEVEL and LANGULUS_ENABLE_SAFE_MODE		
	///	@param condition - the condition that must hold true						
	///	@param message - an error message if condition doesn't hold				
	///	@param location - the location of the error, if any						
	template<unsigned LEVEL, class EXCEPTION = Except::Assertion>
	LANGULUS(ALWAYSINLINE) void Assume(
		bool condition, 
		const char* message = "<unknown assumption failure>", 
		const char* location = "<unknown location>"
	) {
		if constexpr (LEVEL <= LANGULUS_SAFE()) {
			if (!condition)
				Throw<EXCEPTION>(message, location);
		}
	}

	/// A developer assumption level is higher - it is less likely to fail		
	/// Useful inside inner functions, that are implementation details			
	constexpr unsigned DevAssumes {2};

	/// A user assumption level is lower - more likely to fail - no one RTFM!	
	/// Useful for public interface functions, that are exposed to user input	
	constexpr unsigned UserAssumes {1};

} // namespace Langulus


/// Convenience macro for declaring an assumption										
/// Throws Except::Assertion if condition isn't met									
///   @param level - the level at which assumption will be checked -				
/// if level is larger than LANGULUS_SAFE(), no check will be performed;		
/// Zero level assumptions are always checked											
///	@param condition - the condition to check for failure							
///	@param message - the exception message, if condition doesn't hold			
#define LANGULUS_ASSUME(level, condition, message) \
	::Langulus::Assume<level>(condition, message, LANGULUS_LOCATION())

#define LANGULUS_ASSERT(condition, exception, message) \
	::Langulus::Assume<0, exception>(condition, message, LANGULUS_LOCATION())


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
	template<unsigned LEVEL>
	LANGULUS(ALWAYSINLINE) void Assume(
		bool condition, 
		const char* message = "<unknown assumption failure>", 
		const char* location = {}
	) {
		if constexpr (LEVEL <= LANGULUS_SAFE()) {
			if (!condition)
				Throw<Except::Assertion>(message, location);
		}
	}

} // namespace Langulus


/// Convenience macro for declaring an assumption										
///   @param level - the level at which assumption will be checked -				
/// if level is larger than LANGULUS_SAFE(), no check will be performed;		
/// Zero level assumptions are always checked											
#define LANGULUS_ASSUME(level, condition, message) \
	::Langulus::Assume<level>(condition, message, LANGULUS_LOCATION());

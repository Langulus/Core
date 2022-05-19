///																									
/// Langulus::Core																			   
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>					      
///																									
/// Distributed under GNU General Public License v3+									
/// See LICENSE file, or https://www.gnu.org/licenses									
///																									
#pragma once
#include "Common.hpp"
#include <type_traits>
#include <limits>

namespace Langulus
{

	/// Forward lvalue as either lvalue or rvalue										
	/// Same as ::std::forward, but avoid writing the namespace						
	template<class T>
	NOD() constexpr T&& Forward(Deref<T>& a) noexcept {
		return static_cast<T&&>(a);
	}

	/// Forward rvalue as rvalue																
	/// Same as ::std::forward, but avoid writing the namespace						
	template<class T>
	NOD() constexpr T&& Forward(Deref<T>&& a) noexcept {
		static_assert(!::std::is_lvalue_reference_v<T>, "Bad forward call");
		return static_cast<T&&>(a);
	}

	/// Forward as movable																		
	/// Same as ::std::move, but avoid writing the namespace							
	template<class T>
	NOD() constexpr Deref<T>&& Move(T&& a) noexcept {
		return static_cast<Deref<T>&&>(a);
	}
	
	namespace Inner
	{
		struct AAbandoned {};
		struct ADisowned {};
	}
	
	namespace CT
	{
		template<class T>
		concept Abandoned = DerivedFrom<T, ::Langulus::Inner::AAbandoned>;

		template<class T>
		concept Disowned = DerivedFrom<T, ::Langulus::Inner::ADisowned>;

		template<class T>
		concept CustomData = Data<T> && !Deep<T> && !Disowned<T> && !Abandoned<T>;
	}
	

	/// Abandon a value																			
	/// Same as Move, but resets only mandatory data inside source after move	
	/// essentially saving up on a couple of instructions								
	template<class T>
	struct Abandoned : public Inner::AAbandoned {
		Abandoned() = delete;
		Abandoned(const Abandoned&) = delete;
		explicit constexpr Abandoned(Abandoned&&) noexcept = default;
		explicit constexpr Abandoned(T&& value) noexcept 
			: mValue(Langulus::Forward<T>(value)) {}
		
		T&& mValue;
		
		/// Forward as abandoned																
		template<class ALT_T>
		NOD() constexpr Abandoned<ALT_T> Forward() const noexcept {
			return Abandoned<ALT_T>{
				Langulus::Forward<ALT_T>(mValue)
			};
		}
	};
	
	template<class T>
	NOD() constexpr Abandoned<T> Abandon(T&& a) noexcept {
		return Abandoned<T>{Langulus::Forward<T>(a)};
	}
	template<class T>
	NOD() constexpr Abandoned<T> Abandon(T& a) noexcept {
		return Abandoned<T>{Langulus::Move(a)};
	}

	/// Disown a value																			
	/// Same as a shallow-copy, but never references, saving some instructions	
	///	@attention values initialized using Disowned should be Abandoned		
	///				  before the end of their scope										
	template<class T>
	struct Disowned : public Inner::ADisowned  {
		Disowned() = delete;
		Disowned(const Disowned&) = delete;
		explicit constexpr Disowned(Disowned&&) noexcept = default;
		explicit constexpr Disowned(const T& value) noexcept 
			: mValue(value) {}
		
		const T& mValue;
		
		/// Forward as disowned																	
		template<class ALT_T>
		NOD() constexpr Disowned<ALT_T> Forward() const noexcept {
			return Disowned<ALT_T>{mValue};
		}		
	};
	
	template<class T>
	NOD() constexpr Disowned<T> Disown(const T& item) noexcept {
		return Disowned<T>{item};
	}
	
	/// Get number of digits inside an integer											
	/// The routine is as statically optimized as possible							
	///	@param n - value																		
	///	@return the number of digits inside the value								
	template<class T, T OFFSET = 10, Count RESULT = 1>
	NOD() constexpr Count DigitsOf(const T& n) noexcept {
		if constexpr (::std::numeric_limits<T>::digits10 + 1 > RESULT) {
			if constexpr (CT::Signed<T>) {
				// Get rid of negatives													
				using UnsignedT = ::std::make_unsigned_t<T>;
				return DigitsOf<UnsignedT, OFFSET, RESULT>(
					static_cast<UnsignedT>(::std::abs(n))
				);
			}
			else {
				if (n < OFFSET) {
					// Early exit															
					return RESULT;
				}

				// Next digit																
				return DigitsOf<T, OFFSET * 10, RESULT + 1>(n);
			}
		}
		else {
			// Reached the numeric limit												
			return RESULT;
		}
	}

	/// A somewhat safer reinterpret_cast for dense instances						
	///	@param what - reference to reinterpret											
	///	@return the result of reinterpret_cast<TO&>									
	template<CT::Dense TO, CT::Dense FROM>
	NOD() constexpr Decay<TO>& ReinterpretCast(FROM& what) noexcept {
		static_assert(sizeof(Decay<TO>) == sizeof(Decay<FROM>),
			"Size mismatch on a reference reinterpret_cast");
		return reinterpret_cast<Decay<TO>&>(what);
	}

	/// A somewhat safer reinterpret_cast for dense instances (const)				
	///	@param what - reference to reinterpret											
	///	@return the result of reinterpret_cast<const TO&>							
	template<CT::Dense TO, CT::Dense FROM>
	NOD() constexpr const Decay<TO>& ReinterpretCast(const FROM& what) noexcept {
		static_assert(sizeof(Decay<TO>) == sizeof(Decay<FROM>),
			"Size mismatch on a reference reinterpret_cast");
		return reinterpret_cast<const Decay<TO>&>(what);
	}

	/// A somewhat safer reinterpret_cast for sparse instances						
	///	@param what - what to reinterpret												
	///	@return the result of reinterpret_cast<TO*>									
	template<CT::Dense TO, CT::Dense FROM>
	NOD() constexpr Decay<TO>* ReinterpretCast(FROM* what) noexcept {
		static_assert(sizeof(Decay<TO>) == sizeof(Decay<FROM>),
			"Size mismatch on a pointer reinterpret_cast");
		return reinterpret_cast<Decay<TO>*>(what);
	}

	/// A somewhat safer reinterpret_cast for sparse instances (const)			
	///	@param what - what to reinterpret												
	///	@return the result of reinterpret_cast<TO*>									
	template<CT::Dense TO, CT::Dense FROM>
	NOD() constexpr const Decay<TO>* ReinterpretCast(const FROM* what) noexcept {
		static_assert(sizeof(Decay<TO>) == sizeof(Decay<FROM>),
			"Size mismatch on a pointer reinterpret_cast");
		return reinterpret_cast<const Decay<TO>*>(what);
	}

	/// Always returns a pointer to the argument											
	template<class T>
	NOD() constexpr decltype(auto) MakeSparse(T& a) noexcept {
		if constexpr (CT::Sparse<T>)
			return a;
		else
			return &a;
	}

	/// Always returns a pointer to the argument (const)								
	template<class T>
	NOD() constexpr decltype(auto) MakeSparse(const T& a) noexcept {
		if constexpr (CT::Sparse<T>)
			return a;
		else
			return &a;
	}

	/// Always returns a value reference to the argument								
	/// If argument is an array, return a value reference to the first element	
	template<class T>
	NOD() constexpr decltype(auto) MakeDense(T& a) noexcept {
		if constexpr (CT::Array<T>)
			return MakeDense(a[0]);
		else if constexpr (CT::Sparse<T>)
			return *a;
		else
			return a;
	}

	/// Always returns a value reference to the argument (const)					
	/// If argument is an array, return a value reference to the first element	
	template<class T>
	NOD() constexpr decltype(auto) MakeDense(const T& a) noexcept {
		if constexpr (CT::Array<T>)
			return MakeDense(a[0]);
		else if constexpr (CT::Sparse<T>)
			return *a;
		else
			return a;
	}

} // namespace Langulus

///																									
/// Langulus::Core																			   
/// Copyright(C) 2012 Dimo Markov <langulusteam@gmail.com>					      
///																									
/// Distributed under GNU General Public License v3+									
/// See LICENSE file, or https://www.gnu.org/licenses									
///																									
#pragma once
#include "Common.hpp"

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
	/// Same as ::std::move, but avoids writing the namespace						
	template<class T>
	NOD() constexpr Deref<T>&& Move(T&& a) noexcept {
		return static_cast<Deref<T>&&>(a);
	}
	
	/// A namespace dedicated to abstract entities										
	namespace A
	{
		/// An abstract abandoned value														
		struct Abandoned {};
		/// An abstract disowned value														
		struct Disowned {};
	}

	namespace CT
	{
		/// Check if a type is abandoned														
		template<class T>
		concept Abandoned = DerivedFrom<T, ::Langulus::A::Abandoned>;

		/// Check if a type is disowned														
		template<class T>
		concept Disowned = DerivedFrom<T, ::Langulus::A::Disowned>;

		template<class T>
		concept AbandonedOrDisowned = Disowned<T> || Abandoned<T>;
		template<class T>
		concept NotAbandonedOrDisowned = !AbandonedOrDisowned<T>;

		/// Check if a type can be handled generically by templates, and			
		/// doesn't	require any special handling											
		template<class T>
		concept CustomData = Data<T> && !Deep<T> && NotAbandonedOrDisowned<T>;
	}
	

	/// Abandoned value intermediate type, can be used in constructors and		
	/// assignments to provide a guarantee, that the value shall not be used	
	/// after that function, so we can save up on resetting it fully				
	/// For example, you can construct an Any with an abandoned Any, which is	
	/// same as move-construction, but the abandoned Any shall have only its	
	/// mEntry reset, instead of the entire container									
	template<class T>
	struct Abandoned : public A::Abandoned {
		using Type = T;

		Abandoned() = delete;
		Abandoned(const Abandoned&) = delete;
		explicit constexpr Abandoned(Abandoned&&) noexcept = default;
		explicit constexpr Abandoned(T&& value) noexcept 
			: mValue {Langulus::Forward<T>(value)} {
			static_assert(CT::NotAbandonedOrDisowned<T>,
				"Can't nest abandoned/disowned here");
		}
		
		T&& mValue;
		
		/// Forward as abandoned																
		template<class ALT_T>
		NOD() constexpr Abandoned<ALT_T> Forward() const noexcept {
			static_assert(CT::NotAbandonedOrDisowned<ALT_T>, 
				"Can't nest abandoned/disowned here");
			return Abandoned<ALT_T>{
				Langulus::Forward<ALT_T>(mValue)
			};
		}
	};
	
	/// Abandon a value																			
	/// Same as Move, but resets only mandatory data inside source after move	
	/// essentially saving up on a couple of instructions								
	template<CT::NotAbandonedOrDisowned T>
	NOD() constexpr auto Abandon(T&& a) noexcept {
		return Abandoned<T>{Langulus::Forward<T>(a)};
	}

	/// Abandon a value																			
	/// Same as Move, but resets only mandatory data inside source after move	
	/// essentially saving up on a couple of instructions								
	template<CT::NotAbandonedOrDisowned T>
	NOD() constexpr auto Abandon(T& a) noexcept {
		return Abandoned<T>{Langulus::Move(a)};
	}

	/// Disowned value intermediate type, use in constructors and assignments	
	/// to copy container while avoiding referencing it								
	///	@tparam T - the type to disown													
	template<class T>
	struct Disowned : public A::Disowned {
		using Type = T;

		Disowned() = delete;
		Disowned(const Disowned&) = delete;
		explicit constexpr Disowned(Disowned&&) noexcept = default;
		explicit constexpr Disowned(const T& value) noexcept 
			: mValue {value} {
			static_assert(CT::NotAbandonedOrDisowned<T>,
				"Can't nest abandoned/disowned here");
		}
		
		const T& mValue;
		
		/// Forward as disowned																	
		template<class ALT_T>
		NOD() constexpr Disowned<ALT_T> Forward() const noexcept {
			static_assert(CT::NotAbandonedOrDisowned<ALT_T>,
				"Can't nest abandoned/disowned here");
			return Disowned<ALT_T>{mValue};
		}		
	};
	
	/// Disown a value																			
	/// Same as a shallow-copy, but never references, saving some instructions	
	///	@attention values initialized using Disowned must be zeroed before	
	///				  their destruction - be very careful with it					
	template<CT::NotAbandonedOrDisowned T>
	NOD() constexpr auto Disown(const T& item) noexcept {
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

	/// Check if an unsigned integer is a power of two									
	///	@param n - the number to test														
	///	@return true if number has exactly one bit set								
	template<CT::Unsigned T>
	constexpr bool IsPowerOfTwo(const T& n) noexcept {
		return ::std::has_single_bit(n);
	}

	/// Returns the number of consecutive 0 bits in the value of x, starting	
	/// from the least significant 'right' bit											
	///	@param x - the value to scan														
	///	@return the number of consecutive zero bits									
	template<CT::Unsigned T>
	constexpr int CountTrailingZeroes(const T& x) noexcept {
		return ::std::countr_zero(x);
	}

	/// Returns the number of consecutive 0 bits in the value of x, starting	
	/// from the most significant 'left' bit												
	///	@param x - the value to scan														
	///	@return the number of consecutive zero bits									
	template<CT::Unsigned T>
	constexpr int CountLeadingZeroes(const T& x) noexcept {
		return ::std::countl_zero(x);
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
	NOD() constexpr decltype(auto) SparseCast(T& a) noexcept {
		if constexpr (CT::Sparse<T>)
			return a;
		else
			return &a;
	}

	/// Always returns a pointer to the argument (const)								
	template<class T>
	NOD() constexpr decltype(auto) SparseCast(const T& a) noexcept {
		if constexpr (CT::Sparse<T>)
			return a;
		else
			return &a;
	}

	/// Always returns a value reference to the argument								
	/// If argument is an array, return a value reference to the first element	
	template<class T>
	NOD() constexpr decltype(auto) DenseCast(T& a) noexcept {
		if constexpr (CT::Array<T>)
			return DenseCast(a[0]);
		else if constexpr (CT::Sparse<T>)
			return *a;
		else
			return a;
	}

	/// Always returns a value reference to the argument (const)					
	/// If argument is an array, return a value reference to the first element	
	template<class T>
	NOD() constexpr decltype(auto) DenseCast(const T& a) noexcept {
		if constexpr (CT::Array<T>)
			return DenseCast(a[0]);
		else if constexpr (CT::Sparse<T>)
			return *a;
		else
			return a;
	}

	/// Make a type constant reference or constant pointer							
	template<class T>
	using ConstCast = Conditional<CT::Dense<T>, const Decay<T>&, const Decay<T>*>;

} // namespace Langulus

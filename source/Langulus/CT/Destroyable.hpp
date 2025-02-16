#pragma once
#include "../Core/Common.hpp"


namespace Langulus::CT
{

   /// Check if T requires its destructor being called                        
   template<class...T>
   concept Destroyable = Complete<T...> and ((
          not ::std::is_trivially_destructible_v<T>
          and ::std::is_destructible_v<T>
      ) and ...);

} // namespace Langulus::CT
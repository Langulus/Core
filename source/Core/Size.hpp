///                                                                           
/// Langulus::Core                                                            
/// Copyright (c) 2012 Dimo Markov <team@langulus.com>                        
/// Part of the Langulus framework, see https://langulus.com                  
///                                                                           
/// Distributed under GNU General Public License v3+                          
/// See LICENSE file, or https://www.gnu.org/licenses                         
///                                                                           
#pragma once
#include "Config.hpp"
#include <cstddef>
#include <string_view>
#include <limits>


namespace Langulus
{

   ///                                                                        
   /// Type for counting bytes, depends on architecture                       
   ///                                                                        
   struct Size {
      using Type = ::std::size_t;

      Type mSize;

      static constexpr Type Max = ::std::numeric_limits<Type>::max();
      static constexpr Type KB = 1'024LL;
      static constexpr Type MB = KB * 1'024LL;
      static constexpr Type GB = MB * 1'024LL;

      #if LANGULUS_BITNESS() > 32
         static constexpr Type TB = GB * 1'024LL;
         static constexpr Type PB = TB * 1'024LL;
      #endif

      constexpr Size() noexcept = default;
      constexpr Size(const Size&) noexcept = default;
      constexpr Size(Size&&) noexcept = default;
      constexpr Size(Type v) noexcept
         : mSize {v} {}

      constexpr operator Type() const noexcept {
         return mSize;
      }

      constexpr Size& operator = (const Size&) noexcept = default;
      constexpr Size& operator = (Size&&) noexcept = default;

      constexpr ::std::string_view GetSuffix() const noexcept {
         if          (mSize < KB)   return "B";
         else if     (mSize < MB)   return "KB";
         else if     (mSize < GB)   return "MB";
         #if LANGULUS_BITNESS() > 32
            if       (mSize < TB)   return "GB";
            else if  (mSize < PB)   return "TB";
            else return "PB";
         #else
            else return "GB";
         #endif
      }
   };

   /// Byte count literal                                                     
   constexpr Size operator"" _B(unsigned long long int num) noexcept {
      return Size {static_cast<::std::size_t>(num)};
   }

	/// Floating-point bytesize literals, like 5.5_kB                          
   constexpr Size operator"" _KiB(long double num) noexcept {
      return Size {(size_t)((1LL << 10) * num)};
   }
   constexpr Size operator"" _MiB(long double num) noexcept {
      return Size {(size_t)((1LL << 20) * num)};
   }
   constexpr Size operator"" _GiB(long double num) noexcept {
      return Size {(size_t)((1LL << 30) * num)};
   }
   constexpr Size operator"" _TiB(long double num) noexcept {
      return Size {(size_t)((1LL << 40) * num)};
   }
   constexpr Size operator"" _PiB(long double num) noexcept {
      return Size {(size_t)((1LL << 50) * num)};
   }
   constexpr Size operator"" _KB(long double num) noexcept {
      return Size {(size_t)(Size::KB * num)};
   }
   constexpr Size operator"" _MB(long double num) noexcept {
      return Size {(size_t)(Size::MB * num)};
   }
   constexpr Size operator"" _GB(long double num) noexcept {
      return Size {(size_t)(Size::GB * num)};
   }

#if LANGULUS_BITNESS() > 32
   constexpr Size operator"" _TB(long double num) noexcept {
      return Size {(size_t)(Size::TB * num)};
   }
   constexpr Size operator"" _PB(long double num) noexcept {
      return Size {(size_t)(Size::PB * num)};
   }
#endif

	/// Integer bytesize literals, like 5_kB                                   
   constexpr Size operator"" _KiB(unsigned long long int num) noexcept {
      return Size {(size_t)((1LL << 10) * num)};
   }
   constexpr Size operator"" _MiB(unsigned long long int num) noexcept {
      return Size {(size_t)((1LL << 20) * num)};
   }
   constexpr Size operator"" _GiB(unsigned long long int num) noexcept {
      return Size {(size_t)((1LL << 30) * num)};
   }
   constexpr Size operator"" _TiB(unsigned long long int num) noexcept {
      return Size {(size_t)((1LL << 40) * num)};
   }
   constexpr Size operator"" _PiB(unsigned long long int num) noexcept {
      return Size {(size_t)((1LL << 50) * num)};
   }
   constexpr Size operator"" _KB(unsigned long long int num) noexcept {
      return Size {(size_t)(Size::KB * num)};
   }
   constexpr Size operator"" _MB(unsigned long long int num) noexcept {
      return Size {(size_t)(Size::MB * num)};
   }
   constexpr Size operator"" _GB(unsigned long long int num) noexcept {
      return Size {(size_t)(Size::GB * num)};
   }

#if LANGULUS_BITNESS() > 32
   constexpr Size operator"" _TB(unsigned long long int num) noexcept {
      return Size {(size_t)(Size::TB * num)};
   }
   constexpr Size operator"" _PB(unsigned long long int num) noexcept {
      return Size {(size_t)(Size::PB * num)};
   }
#endif

} // namespace Langulus

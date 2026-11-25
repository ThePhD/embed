//   ----------
// | phd::embed |
//   ----------

// Written in 2026 by Björkus "ThePhD" Dorkus <phdofthehouse@gmail.com>

// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.

// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

#ifndef PHD_EMBED_MAKE_STATIC_HPP
#define PHD_EMBED_MAKE_STATIC_HPP

#pragma once

#include <cassert>
#include <type_traits>
#include <array>
#include <cstddef>

namespace phd {

	template <::std::size_t _Size, ::std::size_t _AppendedZeros = 0>
	constexpr auto make_static (const auto& __source) noexcept {
		using _Ty = ::std::remove_cvref_t<decltype(__source[0])>;
		::std::array<_Ty, _Size + _AppendedZeros> __r = {};
		assert(__source.size() == _Size);
		::std::size_t __i = 0;
		for (; __i < __source.size(); ++__i) {
			__r[__i] = __source[__i];
		}
		if constexpr (_AppendedZeros != 0) {
			for (::std::size_t __z = 0; __z < _AppendedZeros; ++__i, ++__z) {
				__r[__i] = static_cast<_Ty>(0);
			}
		}
		return __r;
	}

	template <::std::size_t _Size>
	constexpr auto make_static_c_str (const auto& __source) noexcept {
		return make_static<_Size, 1>(__source);
	}

}

#endif

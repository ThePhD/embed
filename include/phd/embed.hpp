//   ----------
// | phd::embed |
//   ----------

// Written in 2019 by ThePhD <phdofthehouse@gmail.com>

// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.

// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

#ifndef PHD_EMBED_EMBED_HPP
#define PHD_EMBED_EMBED_HPP

#pragma once

#if defined(__has_include)
#if !defined(PHD_EMBED_HAS_INCLUDE)
#define PHD_EMBED_HAS_INCLUDE(HEADER_TOKEN) __has_include(HEADER_TOKEN)
#endif // undefined PHD_EMBED_HAS_INCLUDE
#endif // __has_include ability

#if defined(PHD_EMBED_HAS_INCLUDE) && (PHD_EMBED_HAS_INCLUDE(<version>))
#include <version>
#endif // <version> check

#if !defined(PHD_EMBED_HAS_BUILTIN_EMBED)
#if defined(__has_builtin)
#if __has_builtin(__builtin_embed) && __has_builtin(__builtin_embed_n)
#define  PHD_EMBED_HAS_BUILTIN_EMBED 1
#endif
#endif // __has_builtin test
#endif // undefined PHD_EMBED_HAS_BUILTIN_EMBED

#if defined(__cpp_lib_embed) || (defined(PHD_EMBED_HAS_INCLUDE) && PHD_EMBED_HAS_INCLUDE(<embed>))

#include <embed>

namespace phd {

	using embed = std::embed;

}

#elif defined(PHD_EMBED_HAS_BUILTIN_EMBED) && (PHD_EMBED_HAS_BUILTIN_EMBED != 0)

#include <cstddef>
#include <span>
#include <string_view>

namespace phd {

	inline constexpr ::std::span<const char> embed (::std::string_view __resource_name) {
		const char* __res = nullptr;
		size_t __res_len = __builtin_embed(__resource_name.data(), __resource_name.size(), &__res);
		return {__res, __res_len};
	}

	inline constexpr ::std::span<const char> embed (::std::string_view __resource_name, size_t __resource_limit) {
		const char* __res = nullptr;
		size_t __res_len = __builtin_embed_n(__resource_name.data(), __resource_name.size(), &__res, __resource_limit);
		return {__res, __res_len};
	}

} // namespace phd

#else

#error "This compiler does not support the required __builtin_embed/__builtin_embed_n functions"

#endif // __cpp_lib_embed from <version> || __has_builtin(__builtin_embed)

#endif // PHD_EMBED_EMBED_HPP

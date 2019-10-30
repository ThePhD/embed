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

#ifdef __has_include
#if __has_include(<version>)
#include <version>
#endif
#endif

#ifdef __cpp_lib_embed

#include <embed>

namespace phd {

	using embed = std::embed;

}

#else

#if !defined(PHD_EMBED_HAS_BUILTIN_EMBED)
#ifdef __has_builtin
#if __has_builtin(__builtin_embed) && __has_builtin(__builtin_embed_n)
#define  PHD_EMBED_HAS_BUILTIN_EMBED 1
#endif
#endif // __has_builtin test
#endif // undefined PHD_EMBED_HAS_BUILTIN_EMBED

#if defined(PHD_EMBED_HAS_BUILTIN_EMBED) && (PHD_HAS_BUILTIN_EMBED != 0))

#include <cstddef>
#include <span>

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

#endif // __has_builtin(__builtin_embed)

#endif // __cpp_lib_embed from <version>

#endif // PHD_EMBED_EMBED_HPP

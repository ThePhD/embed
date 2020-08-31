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
#if __has_builtin(__builtin_embed)
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
#include <type_traits>
#include <string_view>
#if defined(PHD_EMBED_CHECK_TYPE_SIZE) && (PHD_EMBED_CHECK_TYPE_SIZE != 0)
#include <cassert>
#endif

namespace phd {

	namespace __detail {
		template <typename _Ty, bool _Str, bool _Dynamic, ::std::size_t _NElements, typename _StrView, typename... _Args>
		inline constexpr ::std::span<const _Ty, _Dynamic ? ::std::dynamic_extent : _NElements> __embed (_StrView __resource_name, _Args&&... __args) noexcept {
			static_assert(std::is_trivially_copiable_v<_Ty>, "Type must have sizeof(T) == 1, and std::is_trivial_v<T> must be true");
			static_assert(sizeof...(_Args) <= 1, "Can only specify 1 additional argument as the maximum potential number of objects.");
			const _Ty* __res = nullptr;
			// always returns # of bytes
			size_t __res_len = __builtin_embed(__resource_name.size(), __resource_name.data(), _Str, ::std::forward<_Args>(__args)..., &__res);
#if defined(PHD_EMBED_CHECK_TYPE_SIZE) && (PHD_EMBED_CHECK_TYPE_SIZE != 0)	
			assert((sizeof(_Ty) == 1 || (__res_len % sizeof(_Ty) == 0)) && "Returned a number of bytes unsuitable for the type specified...");
#endif // Ensure no partial serializations...
			::std::span<const _Ty, _Dynamic ? ::std::dynamic_extent : _NElements> __elements(__res, (__res_len / sizeof(_Ty)));
			if constexpr(!_Dynamic) {
				if (__elements.size() < (sizeof(_Ty) * _NElements)) {
					throw "cannot have a fixed-size span that returns less than _NElements elements";
				}
			}
			return __elements;
		}
	}

	// normal embed
	// dynamic size
	template <typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty> embed (::std::string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, false, true, 0>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

	template <typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty> embed (::std::wstring_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, false, true, 0>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

#ifdef __cpp_char8_t

	template <typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty> embed (::std::u8string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, false, true, 0>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

#endif // char8_t shenanigans

	template <typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty> embed (::std::u16string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, false, true, 0>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

	template <typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty> embed (::std::u32string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, false, true, 0>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

	// fixed size
	template <::std::size_t _NElements, typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty, _NElements> embed (::std::string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, false, false, _NElements>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}
	
	template <::std::size_t _NElements, typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty, _NElements> embed (::std::wstring_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, false, false, _NElements>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}
	
#ifdef __cpp_char8_t

	template <::std::size_t _NElements, typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty, _NElements> embed (::std::u8string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, false, false, _NElements>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

#endif // char8_t shenanigans

	template <::std::size_t _NElements, typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty, _NElements> embed (::std::u16string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, false, false, _NElements>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}
	
	template <::std::size_t _NElements, typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty, _NElements> embed (::std::u32string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, false, false, _NElements>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

	// string embed
	// dynamic size
	template <typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty> embed (::std::string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, true, true, 0>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

	template <typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty> embed (::std::wstring_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, true, true, 0>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

#ifdef __cpp_char8_t

	template <typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty> embed (::std::u8string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, true, true, 0>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

#endif // char8_t shenanigans

	template <typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty> embed (::std::u16string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, true, true, 0>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

	template <typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty> embed (::std::u32string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, true, true, 0>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

	// fixed size
	template <::std::size_t _NElements, typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty, _NElements> embed (::std::string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, true, false, _NElements>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}
	
	template <::std::size_t _NElements, typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty, _NElements> embed (::std::wstring_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, true, false, _NElements>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}
	
#ifdef __cpp_char8_t

	template <::std::size_t _NElements, typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty, _NElements> embed (::std::u8string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, true, false, _NElements>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

#endif // char8_t shenanigans

	template <::std::size_t _NElements, typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty, _NElements> embed (::std::u16string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, true, false, _NElements>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}
	
	template <::std::size_t _NElements, typename _Ty = std::byte, typename... _Args>
	inline constexpr ::std::span<const _Ty, _NElements> embed (::std::u32string_view __resource_name, _Args&&... __args) noexcept {
		return __detail::__embed<_Ty, true, false, _NElements>(::std::move(__resource_name), ::std::forward<_Args>(__args)...);
	}

} // namespace phd

#else

#error "This compiler does not support the required __builtin_embed function"

#endif // __cpp_lib_embed from <version> || __has_builtin(__builtin_embed)

#endif // PHD_EMBED_EMBED_HPP

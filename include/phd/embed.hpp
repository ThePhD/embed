//   ----------
// | phd::embed |
//   ----------

// Written in 2026 by ThePhD <phdofthehouse@gmail.com>

// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.

// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

#ifndef PHD_EMBED_HPP
#define PHD_EMBED_HPP

#pragma once

#if !defined(PHD_EMBED_HAS_INCLUDE)
#if defined(__has_include)
#define PHD_EMBED_HAS_INCLUDE(...) __has_include(__VA_ARGS__)
#else
#define PHD_EMBED_HAS_INCLUDE(...) 0
#endif // __has_include ability
#endif // undefined PHD_EMBED_HAS_INCLUDE

#if !defined(PHD_EMBED_HAS_BUILTIN)
#if defined(__has_include)
#define PHD_EMBED_HAS_BUILTIN(...) __has_builtin(__VA_ARGS__)
#else
#define PHD_EMBED_HAS_BUILTIN(...) 0
#endif // __has_builtin ability
#endif // undefined PHD_EMBED_HAS_BUILTIN

#if defined(PHD_EMBED_HAS_INCLUDE) && (PHD_EMBED_HAS_INCLUDE(<version>))
#include <version>
#endif // <version> check

#if !defined(PHD_EMBED_HAS_BUILTIN_STD_EMBED)
#if PHD_EMBED_HAS_BUILTIN(__builtin_std_embed)
#define PHD_EMBED_HAS_BUILTIN_STD_EMBED 1
#else
#define PHD_EMBED_HAS_BUILTIN_STD_EMBED 0
#endif // __has_builtin test
#endif // undefined PHD_EMBED_HAS_BUILTIN_STD_EMBED

#if (defined(__cpp_lib_embed) && (__cpp_lib_embed > 202300L)) && PHD_EMBED_HAS_INCLUDE(<embed>)

#include <embed>

namespace phd { inline namespace v0 {

	using embed = std::embed;

}} // namespace phd::v0

#elif (PHD_EMBED_HAS_BUILTIN_STD_EMBED != 0)

#include <cstddef>
#include <span>
#include <type_traits>
#include <string_view>
#include <optional>
#include <cassert>

#if PHD_EMBED_HAS_BUILTIN(__builtin_abort)
#define PHD_EMBED_VERBOSE_ABORT(...) \
	[]() {                          \
		assert(__VA_ARGS__);       \
		__builtin_abort();         \
	}();
#else
#define PHD_EMBED_VERBOSE_ABORT(...) \
	[]() {                          \
		assert(__VA_ARGS__);       \
		::std::abort();            \
	}();
#endif

namespace phd {

	namespace __detail {
		enum __builtin_result {
			__file_not_found = 0,
			__file_found = 1,
			__file_found_but_no_depend = 2,
			__file_found_and_empty = 3
		};

		template <typename _Ty, ::std::size_t _Extent, typename _StrView>
		inline consteval ::std::span<const _Ty, _Extent> __embed(const _StrView& __resource_name,
		     ::std::size_t __offset, const ::std::optional<::std::size_t>& __limit) noexcept {
			static_assert(
			     (::std::is_integral_v<_Ty> || ::std::is_enum_v<_Ty>) && alignof(_Ty) == 1 && sizeof(_Ty) == 1,
			     "Type must have sizeof(T) == 1, alignof(T) == 1, and it must be an integral or "
			     "enumeration type");
			int __status       = -1;
			const _Ty* __res = nullptr;
			size_t __res_len = 0;
			if constexpr (_Extent != ::std::dynamic_extent) {
				__res = __builtin_std_embed(
				     __status, __res_len, __res, __resource_name.size(), __resource_name.data(), __offset, _Extent);
			}
			else {
				if (__limit) {
					__res = __builtin_std_embed(__status, __res_len, __res, __resource_name.size(),
					     __resource_name.data(), __offset, *__limit);
				}
				else {
					__res = __builtin_std_embed(
					     __status, __res_len, __res, __resource_name.size(), __resource_name.data(), __offset);
				}
			}
			if (__status == __file_not_found) {
				PHD_EMBED_VERBOSE_ABORT("file not found");
			}
			else if (__status == __file_found_but_no_depend) {
				PHD_EMBED_VERBOSE_ABORT("file found but not appropriately '#depend <>'-ed");
			}
			if constexpr (_Extent != ::std::dynamic_extent) {
				if (__res_len < _Extent) {
					PHD_EMBED_VERBOSE_ABORT(
					     "the size of the resource is too small for "
					     "the fixed-size span's extent");
				}
			}
			if (__status == __file_found_and_empty) {
				return ::std::span<const _Ty, _Extent>();
			}
			assert(__status == __file_found
			     && "status value should be `1` at this point, indicating a successful "
			        "finding of a file with data that has been depended on");
			return ::std::span<const _Ty, _Extent>(__res, __res_len);
		}
	} // namespace __detail

	template <typename _Ty = std::byte>
	inline constexpr ::std::span<const _Ty> embed(::std::string_view __resource_name, ::std::size_t __offset = 0,
	     ::std::optional<::std::size_t> __limit = ::std::nullopt) noexcept {
		return __detail::__embed<_Ty, ::std::dynamic_extent>(::std::move(__resource_name), __offset, __limit);
	}

	template <typename _Ty = std::byte>
	inline consteval ::std::span<const _Ty> embed(::std::wstring_view __resource_name, ::std::size_t __offset = 0,
	     ::std::optional<::std::size_t> __limit = ::std::nullopt) noexcept {
		return __detail::__embed<_Ty, ::std::dynamic_extent>(::std::move(__resource_name), __offset, __limit);
	}

#ifdef __cpp_char8_t

	template <typename _Ty = ::std::byte>
	inline consteval ::std::span<const _Ty> embed(::std::u8string_view __resource_name,
	     ::std::size_t __offset = 0, ::std::optional<::std::size_t> __limit = ::std::nullopt) noexcept {
		return __detail::__embed<_Ty, ::std::dynamic_extent>(::std::move(__resource_name), __offset, __limit);
	}

#endif // char8_t shenanigans

	template <typename _Ty, ::std::size_t _Extent>
	inline constexpr ::std::span<const _Ty, _Extent> embed(
	     ::std::string_view __resource_name, ::std::size_t __offset = 0) noexcept {
		return __detail::__embed<_Ty, _Extent>(::std::move(__resource_name), __offset, ::std::nullopt);
	}

	template <typename _Ty, ::std::size_t _Extent>
	inline consteval ::std::span<const _Ty, _Extent> embed(
	     ::std::wstring_view __resource_name, ::std::size_t __offset = 0) noexcept {
		return __detail::__embed<_Ty, _Extent>(::std::move(__resource_name), __offset, ::std::nullopt);
	}

#ifdef __cpp_char8_t

	template <typename _Ty, ::std::size_t _Extent>
	inline consteval ::std::span<const _Ty, _Extent> embed(
	     ::std::u8string_view __resource_name, ::std::size_t __offset = 0) noexcept {
		return __detail::__embed<_Ty, _Extent>(::std::move(__resource_name), __offset, ::std::nullopt);
	}

#endif // char8_t shenanigans

} // namespace phd

#else

#error "This compiler does not support the required <embed> functionality"

#endif

#endif // PHD_EMBED_HPP

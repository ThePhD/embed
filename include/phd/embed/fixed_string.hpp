//   ----------
// | phd::embed |
//   ----------

// Written in 2026 by Björkus "ThePhD" Dorkus <phdofthehouse@gmail.com>

// To the extent possible under law, the author(s) have dedicated all copyright and related
// and neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.

// You should have received a copy of the CC0 Public Domain Dedication along with this software.
// If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.

#ifndef PHD_EMBED_FIXED_STRING_HPP
#define PHD_EMBED_FIXED_STRING_HPP

#pragma once

#include <type_traits>
#include <array>
#include <string_view>
#include <cstddef>
#include <cstdint>

namespace phd {
	namespace detail {
		struct from_ptr_t {};
		inline constexpr from_ptr_t from_ptr;
	} // namespace detail

	template <typename Ch, std::size_t N>
	struct basic_fixed_string {
		Ch storage[N] = { };

		constexpr basic_fixed_string(detail::from_ptr_t, const Ch* input) noexcept {
			if (!input)
				return;
			for (std::size_t i = 0; i < N; ++i) {
				storage[i] = static_cast<Ch>(input[i]);
			}
		}

		constexpr basic_fixed_string(const std::array<Ch, N>& in) noexcept
		: basic_fixed_string(detail::from_ptr, in.data()) {
		}

		constexpr basic_fixed_string(const Ch(&input)[N + 1]) noexcept : basic_fixed_string(detail::from_ptr, input) {
		}

		constexpr basic_fixed_string(const Ch(&input)[N]) noexcept : basic_fixed_string(detail::from_ptr, input) {
		}

		constexpr basic_fixed_string(const basic_fixed_string& other) noexcept {
			for (std::size_t i = 0; i < N; ++i) {
				storage[i] = other.storage[i];
			}
		}

		constexpr std::size_t size() const noexcept {
			return N;
		}

		constexpr const Ch* begin() const noexcept {
			return storage;
		}

		constexpr const Ch* end() const noexcept {
			return storage + size();
		}

		constexpr Ch operator[](size_t i) const noexcept {
			return storage[i];
		}

		template <size_t M>
		constexpr bool operator==(const basic_fixed_string<Ch, M>& rhs) const noexcept {
			if (size() != rhs.size())
				return false;
			for (std::size_t i = 0; i != N; ++i) {
				if (storage[i] != rhs[i])
					return false;
			}
			return true;
		}

		constexpr operator std::basic_string_view<Ch>() const noexcept {
			return std::basic_string_view<Ch> { storage, size() };
		}
	};

	template <typename Ch>
	class basic_fixed_string<Ch, 0> {
		inline static constexpr Ch empty_str[1] = { 0 };

	public:
		constexpr basic_fixed_string(const Ch*) noexcept {
		}

		constexpr basic_fixed_string(const std::array<Ch, 0>&) noexcept {
		}

		constexpr basic_fixed_string(const basic_fixed_string&) noexcept {
		}

		constexpr std::size_t size() const noexcept {
			return 0;
		}

		constexpr const Ch* begin() const noexcept {
			return empty_str;
		}

		constexpr const Ch* end() const noexcept {
			return empty_str + size();
		}

		constexpr Ch operator[](size_t) const noexcept {
			return 0;
		}

		template <size_t M>
		constexpr bool operator==(const basic_fixed_string<Ch, M>& rhs) const noexcept {
			if (size() != rhs.size())
				return false;
			return true;
		}

		constexpr operator std::basic_string_view<Ch>() const noexcept {
			return std::basic_string_view<Ch> { empty_str, 0 };
		}
	};

	template <typename Ch, std::size_t N>
	basic_fixed_string(const Ch (&)[N]) -> basic_fixed_string<Ch, N - 1>;
	template <typename Ch, std::size_t N>
	basic_fixed_string(const std::array<Ch, N>&) -> basic_fixed_string<Ch, N>;
	template <typename Ch, std::size_t N>
	basic_fixed_string(basic_fixed_string<Ch, N>) -> basic_fixed_string<Ch, N>;

	template <std::size_t N>
	using fixed_string = basic_fixed_string<char, N>;
	template <std::size_t N>
	using fixed_wstring = basic_fixed_string<wchar_t, N>;
#if __cpp_char8_t
	template <std::size_t N>
	using fixed_u8string = basic_fixed_string<char8_t, N>;
#endif
	template <std::size_t N>
	using fixed_u16string = basic_fixed_string<char16_t, N>;
	template <std::size_t N>
	using fixed_u32string = basic_fixed_string<char32_t, N>;

} // namespace phd

#endif

#include <phd/embed.hpp>
	
#include <cstdint>

constexpr std::uint64_t val_64_const = 0xcbf29ce484222325;
constexpr std::uint64_t prime_64_const = 0x100000001b3;

inline constexpr std::uint64_t
hash_64_fnv1a_const(const char* const ptr, std::size_t ptr_size, const std::uint64_t value = val_64_const) noexcept {
	return (ptr_size == 1) 
		? value : 
		hash_64_fnv1a_const(&ptr[1], ptr_size - 1, (value ^ static_cast<std::uint64_t>(ptr[0])) * prime_64_const);
}

#include <iostream>

int main () {
	constexpr std::span<const char> art_data  = std::embed("art.txt");
	constexpr std::uint64_t expected = 12781078433878002033;
	constexpr std::uint64_t actual = hash_64_fnv1a_const(art_data.data(), art_data.size());

	static_assert(expected == actual, "🚨 SUSPICIOUS ART SUSPICIOUS ART 🚨");

	return 0;
}

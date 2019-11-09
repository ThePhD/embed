#include <phd/embed.hpp>
	
#include <cstdint>

inline constexpr std::uint64_t
compute_stuff(const char* const ptr, std::size_t ptr_size, std::uint64_t seed = 0, std::size_t limit = 5) noexcept {
	const char* first = ptr;
	const char* last = ptr + ptr_size;
	for (const char* first = ptr; first != last; ++first) {
		const char val = *first;
		if (limit > 1 && val > 126) {
			std::span<const char> more = std::embed("foo.txt");
			seed = compute_stuff(more.data(), more.size(), seed, limit - 1);
		}
		seed ^= *first;
	}
	return seed;
}

int main () {
	constexpr std::span<const char> art_data  = std::embed("foo/art.txt");
	constexpr std::uint64_t actual = compute_stuff(art_data.data(), art_data.size());
	static_assert(actual == 103);

	return static_cast<int>(actual);
}

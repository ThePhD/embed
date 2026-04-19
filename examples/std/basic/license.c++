#include <phd/embed.hpp>

#depend <LICENSE>

int main () {
	static constexpr std::span<const unsigned char> data =
		phd::embed<unsigned char>("LICENSE.txt", 0, 1);
	static constexpr std::span<const std::byte> data2 =
		phd::embed<std::byte>("LICENSE.txt", 79, 1);
	static constexpr std::span<const char> data3 =
		phd::embed<char>("LICENSE.txt", 80, 2);

	static_assert(data.size() == 1);
	static_assert(data[0] == '=');
	static_assert(data2.size() == 1);
	static_assert((unsigned char)(data2[0]) == 'T');
	static_assert(data3.size() == 2);
	static_assert(data3[0] == 'h');
	static_assert(data3[1] == 'e');

     return 0;
}

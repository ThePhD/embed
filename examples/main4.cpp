#include <phd/embed.hpp>

int main () {
	constexpr std::span<const char> data = std::embed("foo.txt");
	return data[0];
}

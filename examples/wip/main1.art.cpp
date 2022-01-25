#include <phd/embed.hpp>
	

int main () {
	constexpr std::span<const char> data = std::embed("foo/art.txt");
	return data[0];
}

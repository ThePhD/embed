#include <phd/embed.hpp>

int main () {
	constexpr std::span<const char> data = std::embed("art.txt");
	return data[0];
}

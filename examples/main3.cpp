// Nice.

#include <cstddef>
#include <cassert>

int main () {
	const char* data = nullptr;
	std::size_t data_size = __builtin_embed_n("/dev/urandom", 12, &data, 1);
	assert(data_size == 1);
	return data[0];
}

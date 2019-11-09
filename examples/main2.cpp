// >:3

#include <cstddef>
#include <utility>
#include <cassert>

int main () {
	const char* data = nullptr;
	const char* trunc_data = nullptr;
	const size_t data_size = __builtin_embed("foo.txt.txt", 7, &data);
	const size_t trunc_data_size = __builtin_embed_n("foo.txt.wdhawdkaw", 7, &trunc_data, 1);
	int arr[trunc_data_size]{};
	assert(data_size == 3);
	assert(data[0] == 'F');
	assert(data[1] == 'o');
	assert(data[2] == 'o');
	assert(trunc_data_size == 1);
	assert(trunc_data[0] == 'F');
	return arr[0];
}

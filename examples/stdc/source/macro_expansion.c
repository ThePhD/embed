#include <stdio.h>
#include <string.h>
#include <stddef.h>

#define IMPORTANT_VAL 5
#define LIMIT_PARAM(VAL) limit(VAL)

int main () {
	const unsigned char data[] = {
#embed __FILE__ LIMIT_PARAM(IMPORTANT_VAL)
	};
	const size_t data_size = sizeof(data) / sizeof(*data);
	_Static_assert((sizeof(data) / sizeof(*data)) == IMPORTANT_VAL, "Limit parameter failed!");
	unsigned char read_data[sizeof(data) / sizeof(*data)] = {0};
	const size_t read_data_size = sizeof(read_data) / sizeof(*read_data);
	FILE* this_file = fopen("macro_expansion.c", "rb");
	if (this_file == NULL) {
		return 1;
	}
	size_t read_size = fread(read_data, sizeof(*read_data),
		data_size, this_file);
	if (read_data_size != read_size) {
		return 2;
	}
	if (read_data_size != data_size) {
		return 3;
	}
	if (fclose(this_file) != 0) {
		return 4;
	}
	int result = memcmp(data, read_data, 5);
	return (result != 0 ? 4 : 0);
}

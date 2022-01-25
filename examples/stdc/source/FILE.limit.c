#include <stdio.h>
#include <string.h>
#include <stddef.h>

int main () {
	const unsigned char data[] = {
#embed __FILE__ limit(5)
	};
	const size_t data_size = sizeof(data) / sizeof(*data);
	const unsigned char stored_data[sizeof(data) / sizeof(*data)] = {0};
	const size_t stored_data_size = sizeof(stored_data) / sizeof(*stored_data);
	FILE* this_file = fopen("FILE.c", "rb");
	if (this_file == NULL) {
		return 1;
	}
	size_t read_size = fread(stored_data, sizeof(*stored_data),
		sizeof(stored_data) / sizeof(*stored_data), this_file);
	if (stored_data_size != read_size) {
		return 2;
	}
	if (fclose(this_file) != 0) {
		return 3;
	}
	int result = memcmp(data, stored_data, 5);
	return (result != 0 ? 4 : 0);
}

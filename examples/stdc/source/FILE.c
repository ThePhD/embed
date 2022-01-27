#include <stdio.h>
#include <string.h>
#include <stddef.h>

int main () {
	const unsigned char data[] = {
#embed __FILE__
	};
	unsigned char read_data[sizeof(data) / sizeof(*data)] = {0};
	const size_t read_data_size = sizeof(read_data) / sizeof(*read_data);
	FILE* this_file = fopen("FILE.c", "rb");
	if (this_file == NULL) {
		return 1;
	}
	size_t read_size = fread(read_data, sizeof(*read_data),
		sizeof(read_data) / sizeof(*read_data), this_file);
	if (read_data_size != read_size) {
		return 2;
	}
	if (fclose(this_file) != 0) {
		return 3;
	}
	int result = memcmp(data, read_data, sizeof(data) / sizeof(*data));
	return (result != 0 ? 4 : 0);
}

#include <stdio.h>
#include <stddef.h>
#include <string.h>

int main () {
	const char str_data[] = {
#embed <media/art.txt>
		, 0
	};
	const size_t data_size = sizeof(str_data) / sizeof(*str_data);
	const char art_data[sizeof(str_data) / sizeof(*str_data)] = {0};
	const size_t art_data_size = sizeof(art_data) / sizeof(*art_data);
	FILE* this_file = fopen("art.txt", "rb");
	if (this_file == NULL) {
		return 1;
	}
	size_t read_size = fread(art_data, sizeof(*art_data),
		(sizeof(art_data) / sizeof(*art_data)) - 1, this_file);
	if ((art_data_size - 1) != read_size) {
		return 2;
	}
	if (fclose(this_file) != 0) {
		return 3;
	}
	int result = strcmp(art_data, str_data);
	if (result != 0) {
		return 4;
	}
	return 0;
}

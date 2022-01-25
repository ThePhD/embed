#include <string.h>

int main () {
	const char str_data[] = {
#embed "a.txt"
		, 0
	};
	if (sizeof(str_data) != 2) {
		return 1;
	}
	int result = strcmp("a", str_data);
	if (result != 0) {
		return 2;
	}
	return 0;
}

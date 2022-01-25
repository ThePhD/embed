#include <string.h>

int main () {
	const char str_data[] = {
#embed "/dev/urandom" suffix(,) limit(6)
		0
	};
	if (sizeof(str_data) != 7) {
		return 1;
	}
	if (strlen(str_data) != 6) {
		return 2;
	}
	return 0;
}

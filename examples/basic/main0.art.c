// :D

#include <stdio.h>

int main () {
	const char str_data[] =
#embed_str "foo/art.txt"
	;
	puts(str_data);
	return 0;
}

// :D

#include <stdio.h>

int main () {
	const unsigned char data[] =
#embed 5 "/dev/urandom"
	;
	const char str_data[] =
#embed_str 3 "/dev/urandom"
	;
	return str_data[0] + data[0];
}

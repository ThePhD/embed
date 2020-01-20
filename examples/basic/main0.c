// :D

#include <stdio.h>

int main () {
	const unsigned char data[] =
#embed __FILE__
	;
	const char str_data[] =
#embed_str __FILE__
	;
	puts(str_data);
	return !(str_data[0] == '/'
	&& str_data[1] == '/'
	&& str_data[2] == ' '
	&& data[3] == ':'
	&& data[4] == 'D');
}

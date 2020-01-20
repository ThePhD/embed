// :D

#include <stdio.h>

int main () {
	const unsigned char data[] =
#embed 5 __FILE__
	;
	const char str_data[] =
#embed_str 3 __FILE__
	;
	puts(str_data);
	return !(str_data[0] == '/'
	&& str_data[1] == '/'
	&& str_data[2] == ' '
	&& str_data[3] == '\0'
	&& data[3] == ':'
	&& data[4] == 'D');
}

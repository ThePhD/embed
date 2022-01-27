// hi :D

int main () {
	const unsigned char data[] = {
#embed __FILE__
	};
	if (data[0] != '/') return 1;
	if (data[1] != '/') return 2;
	if (data[2] != ' ') return 3;
	if (data[3] != 'h') return 4;
	if (data[4] != 'i') return 5;
	if (data[5] != ' ') return 6;
	if (data[6] != ':') return 7;
	if (data[7] != 'D') return 8;
	return 0;
}

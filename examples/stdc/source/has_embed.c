int main () {
#if !__has_embed(__FILE__)
	return 1;
#elif !__has_embed(<media/art.txt>)
	return 2;
#elif __has_embed("asdkasdjkadsjkdsfjk")
	return 3;
#elif __has_embed("asdkasdjkadsjkdsfjk" limit(1))
	return 4;
#elif __has_embed("asdkasdjkadsjkdsfjk" suffix(x) limit(1))
	return 5;
#elif __has_embed("asdkasdjkadsjkdsfjk" suffix(x) djsakdasjd::xmeow("xD"))
	return 6;
#elif !__has_embed(__FILE__ limit(2) prefix(y))
	return 7;
#elif !__has_embed(__FILE__ limit(2))
	return 8;
#elif __has_embed(__FILE__ dajwdwdjdahwk::meow(x))
	return 9;
#elif __has_embed(<media/empty>) != 2
	return 10;
#elif __has_embed(<media/empty> limit(0)) != 2
	return 11;
#elif __has_embed(<media/art.txt> limit(0)) != 2
	return 12;
#else
	return 0;
#endif
}

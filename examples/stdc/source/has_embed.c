int main () {
#if !__has_embed(__FILE__)
	return 1;
#elif !__has_embed(<media/art.txt>)
	return 2;
#elif __has_embed("asdkasdjkadsjkdsfjk")
	return 3;
#else
	return 0;
#endif
}

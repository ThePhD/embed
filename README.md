# embed

The library portion of [P1040](https://thephd.github.io/vendor/future_cxx/papers/d1040.html). Use with a capable compiler. Errors if your compiler doesn't have the built-in or a way to detect the built-in.

CC0.

| Compiler | Status | __builtin_embed | __builtin_embed_n | #embed | #embed_str |
|:--------:|:----------------------------------------------------------------------------:|:---------------:|:-----------------:|:-------:|:----------:|
| GCC | [Patchable, Needs Tests](https://github.com/ThePhD/gcc/tree/feature/embed) |  ✔️ | ✔️ | ✔️ | ✔️ |
| Clang | [WIP, Needs Help](https://github.com/ThePhD/llvm-project/tree/feature/embed) |  🔧 WIP 🔧 |  🔧 WIP 🔧 |  ✔️  |  ✔️ |
| MSVC | ✖️ | ✖️ | ✖️ | ✖️ | ✖️ |


## Usage

Filesystem:

```bash
- 📁 bar
- 📂 baz
  |-- 📄 art.txt
- 📄 foo.txt
- 💻 main.cpp
```

Files:

`foo.txt`:
```
Foo
```

`art.txt`:
```
           __  _
       .-.'  `; `-._  __  _
      (_,         .-:'  `; `-._
    ,'o"(        (_,           )
   (__,-'      ,'o"(            )>
      (       (__,-'            )
       `-'._.--._(             )
          |||  |||`-'._.--._.-'
                     |||  |||


```

### Library Function

`g++ -std=c++2a main.cpp`

`main.cpp`:
```cpp
#include <phd/embed.hpp>

int main () {
	constexpr std::span<const char> data = phd::embed("foo.txt");

	static_assert(data[0] == 'F');
	// to pay respects

	return 0;
}

```

`g++ -std=c++2a -fembed-path=baz main.cpp`

`main.cpp`:
```cpp
#include <phd/embed.hpp>
#include <cstdint>

constexpr std::uint64_t val_64_const = 0xcbf29ce484222325;
constexpr std::uint64_t prime_64_const = 0x100000001b3;

inline constexpr std::uint64_t
hash_64_fnv1a_const(const char* const ptr, std::size_t ptr_size, const std::uint64_t value = val_64_const) noexcept {
	return (ptr_size == 1) 
		? value : 
		hash_64_fnv1a_const(&ptr[1],
			ptr_size - 1, 
			(value ^ static_cast<std::uint64_t>(static_cast<char>(*ptr))) * prime_64_const);
}

int main () {
	constexpr std::span<const char> art_data  = phd::embed("art.txt");
	constexpr std::uint64_t expected = 12781078433878002033;
	constexpr std::uint64_t actual = hash_64_fnv1a_const(art_data.data(), art_data.size());

	static_assert(expected == actual, "🚨 SUSPICIOUS ART SUSPICIOUS ART 🚨");

	return 0;
}
```

Code from [here](https://notes.underscorediscovery.com/constexpr-fnv1a/).



`g++ -std=c++2a main.cpp`

`main.cpp`:
```cpp
#include <phd/embed.hpp>

int main () {
	constexpr std::span<const char> data = phd::embed("/dev/urandom", 1);

	static_assert((data[0] % 2) == '\0', "You lose.");

	return 0;
}

```

### Preprocessor

`gcc -std=c2x -fembed-path ./baz main.c`

`main.c`:
```c
#include <stdio.h>

int main () {
	const char str_data[] =
#embed_str "art.txt"
	;
	puts(str_data);
	return 0;
}
```


`gcc -std=c2x -fembed-path=./baz main.c`

`main.c`:
```c
#include <assert.h>
#include <stdbool.h>

#define ARRAY_SIZE(x) (sizeof(arr_val) / sizeof(arr_val[0]))

int main () {
	const char data[] =
#embed "art.txt"
	;

	const int size = ARRAY_SIZE(data);
	bool is_null_terminated = data[size - 1] == '\0';
	assert(not is_null_terminated);

	return 0;
}
```


`gcc -std=c2x main.c`

`main.c`:
```c
#include <assert.h>

#define ARRAY_SIZE(x) (sizeof(arr_val) / sizeof(arr_val[0]))

int main () {
	const char data[] =
#embed 4 "/dev/urandom"
	;

	assert(ARRAY_SIZE(data) == 4);

	// ¯\_(ツ)_/¯
	return data[0];
}
```

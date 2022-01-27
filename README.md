# Preprocessor Embed

[P1967](https://thephd.github.io/vendor/future_cxx/papers/source/C%20-%20embed.html). Use with a capable compiler. Errors if your compiler doesn't have the built-in or a way to detect the built-in.

CC0.

#### NOTE: Clang implementation is the new one that tracks the paper, the GCC implementation is not up to date with the paper yet.

## Clang Implementation Progress

- Clang Progress:
  - [x] `#embed <>` vs. `#embed ""` handled properly
  - [x] Parameters (`suffix(token-soup)`, `prefix(token-soup)`, `limit(token-soup)`, `is_empty(token-soup)`)
  - [x] Unknown parameters are warned on
  - [x] `__has_embed`
  - [ ] Re-done builtin for optimizations.

- [ ] GCC Progress:
  - Maybe some other day when I am not exhausted.

- [ ] MSVC Progress:
  - I can't propose features to that compiler. `¯\_(ツ)_/¯`



## Usage

More examples that are used to test the compiler can be found [in this directory](examples/).

Filesystem:

```bash
- 📁 bar
- 📂 baz
  |-- 📄 art.txt
- 📄 foo.txt
- 💻 main.c
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

### Preprocessor

`clang -std=c2x -binary-dir ./baz main.c`

(Works with `g++` / in C++ mode too.)

`main.c`:
```c
#include <stdint.h>
#include <stdio.h>

int main () {
	const unsigned char str_data[] = {
#embed "art.txt" suffix(,)
		0
		// add your own null termination to data;
		// can put additional initializers 
		// anywhere before / after
	};
	puts(str_data);
	return 0;
}
```


`clang -std=c2x -binary-dir=./baz main.c`

`main.c`:
```c
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#define ARRAY_SIZE(x) (sizeof(arr_val) / sizeof(arr_val[0]))

int main () {
	const unsigned char data[] = {
#embed "art.txt"
	};

	const int size = ARRAY_SIZE(data);
	bool is_null_terminated = data[size - 1] == '\0';
	assert(not is_null_terminated);

	return 0;
}
```


`clang -std=c2x main.c`

(Works with `g++` / in C++ mode too.)

`main.c`:
```c
#include <stdint.h>
#include <assert.h>

#define ARRAY_SIZE(x) (sizeof(arr_val) / sizeof(arr_val[0]))

int main () {
	const unsigned char data[] = {
#embed "/dev/urandom" limit(4)
	};

	assert(ARRAY_SIZE(data) == 4);

	// ¯\_(ツ)_/¯
	return data[0];
}
```



# Speed Results

**WARNING: The following code was done back when an implementation existed for both GCC and Clang. The implementation has been changed since then, so these numbers are currently not reproducible since I'm very much a derp and destroyed my Clang and GCC repositories for both implementations 😭.**

Below are timing results for a file of random bytes using a specific strategy. The file is of the size specified at the top of the column. Files are kept the same between strategies and tests.

- Intel Core i7 @ 2.60 GHz
- 24.0 GB RAM
- Debian Sid or Windows 10
- Method: Gather timings from `time` *nix program | `Measure-Command { ... }` PowerShell, average

| Strategy               |     4 bytes    |   40 bytes    |   400 bytes   |  4 kilobytes  |
|------------------------|----------------|---------------|---------------|---------------|
| `#embed` GCC           |     0.201 s    |     0.208 s   |     0.207 s   |    0.218 s    |
| `#embed` Clang         |     0.290 s    |     0.200 s   |     0.270 s   |    0.180 s    |
| `phd::embed` GCC       |     0.709 s    |     0.724 s   |     0.711 s   |    0.715 s    |
| `phd::embed` Clang     |     0.740 s    |     0.800 s   |     0.750 s   |    0.810 s    |
| `xxd`-generated GCC    |     0.225 s    |     0.215 s   |     0.237 s   |    0.247 s    |
| `xxd`-generated Clang  |     0.272 s    |     0.275 s   |     0.272 s   |    0.272 s    |
| `xxd`-generated MSVC   |     0.204 s    |     0.229 s   |     0.209 s   |    0.232 s    |
| `xxd`-generated Circle |     0.180 s    |     0.190 s   |     0.190 s   |    0.200 s    |
| Circle `@array`        |     0.353 s    |     0.359 s   |     0.361 s   |    0.361 s    |
| Circle `@embed`        |     0.199 s    |     0.208 s   |     0.204 s   |    0.368 s    |
| `objcopy` (linker)     |     0.501 s    |     0.482 s   |     0.519 s   |    0.527 s    |

| Strategy               |  40 kilobytes  | 400 kilobytes |  4 megabytes  |  40 megabytes |
|------------------------|----------------|---------------|---------------|---------------|
| `#embed` GCC           |     0.236 s    |    0.231 s    |     0.300 s   |     1.069 s   |
| `#embed` Clang         |     0.260 s    |    0.300 s    |     1.270 s   |     7.810 s   |
| `phd::embed` GCC       |     0.705 s    |    0.713 s    |     0.772 s   |     1.135 s   |
| `phd::embed` Clang     |     0.760 s    |    0.790 s    |     0.850 s   |     1.070 s   |
| `xxd`-generated GCC    |     0.406 s    |    2.135 s    |    23.567 s   |   225.290 s   |
| `xxd`-generated Clang  |     0.366 s    |    1.063 s    |     8.309 s   |    83.250 s   |
| `xxd`-generated MSVC   |     0.552 s    |    3.806 s    |    52.397 s   | Out of Memory |
| `xxd`-generated Circle |     0.220 s    |    0.450 s    |     3.600 s   |    33.690 s   |
| Circle `@array`        |     0.353 s    |    0.363 s    |     0.421 s   |     0.585 s   |
| Circle `@embed`        |     0.238 s    |    0.199 s    |     0.219 s   |     0.368 s   |
| `objcopy` (linker)     |     0.500 s    |    0.497 s    |     0.555 s   |     2.183 s   |

| Strategy               |    400 megabytes    |     1 gigabyte      |
|------------------------|---------------------|---------------------|
| `#embed` GCC           |        9.803 s      |      26.383 s       |
| `#embed` Clang         |    Out of Memory    |    Out of Memory    |
| `phd::embed` GCC       |        4.170 s      |      11.887 s       |
| `phd::embed` Clang     |        3.790 s      |       8.180 s       |
| `xxd`-generated GCC    |    Out of Memory    |    Out of Memory    |
| `xxd`-generated Clang  |    Out of Memory    |    Out of Memory    |
| `xxd`-generated MSVC   |    Out of Memory    |    Out of Memory    |
| `xxd`-generated Circle | Compiler File Limit | Compiler File Limit |
| Circle `@array`        |        2.655 s      |       6.023 s       |
| Circle `@embed`        |        1.886 s      |       4.762 s       |
| `objcopy` (linker)     |       22.654 s      |      58.204 s       |



# Memory Size Results

Below is the peak memory usage (heap usage) for a file of random bytes using a specific strategy. The file is of the size specified at the top of the column. Files are kept the same between strategies and tests.

- Intel Core i7 @ 2.60 GHz
- 24.0 GB RAM
- Debian Sid or Windows 10
- Method: `/usr/bin/time -v` or Execute command hundreds of times, stare at Task Manager

| Strategy               |     4 bytes    |   40 bytes    |   400 bytes   |  4 kilobytes  |
|------------------------|----------------|---------------|---------------|---------------|
| `#embed` GCC           |    17.26 MB    |    17.26 MB   |    17.26 MB   |    17.27 MB   |
| `#embed` Clang         |    23.08 MB    |    23.02 MB   |    23.08 MB   |    23.37 MB   |
| `phd::embed` GCC       |    38.82 MB    |    38.77 MB   |    38.80 MB   |    38.80 MB   |
| `phd::embed` Clang     |    58.38 MB    |    58.39 MB   |    58.37 MB   |    58.40 MB   |
| `xxd`-generated GCC    |    17.26 MB    |    17.26 MB   |    17.26 MB   |    17.27 MB   |
| `xxd`-generated Clang  |    35.12 MB    |    35.22 MB   |    35.31 MB   |    35.88 MB   |
| `xxd`-generated MSVC   |  < 30.00 MB    |  < 30.00 MB   |  < 33.00 MB   |  < 38.00 MB   |
| `xxd`-generated Circle |    33.31 MB    |    33.31 MB   |    33.30 MB   |    33.31 MB   |
| Circle `@array`        |    53.56 MB    |    53.60 MB   |    53.53 MB   |    53.88 MB   |
| Circle `@embed`        |    33.35 MB    |    33.34 MB   |    33.34 MB   |    33.35 MB   |
| `objcopy` (linker)     |    17.32 MB    |    17.31 MB   |    17.31 MB   |    17.31 MB   |

| Strategy               |  40 kilobytes  | 400 kilobytes |  4 megabytes  |  40 megabytes |
|------------------------|----------------|---------------|---------------|---------------|
| `#embed` GCC           |    17.26 MB    |    17.96 MB   |     53.42 MB  |    341.72 MB  |
| `#embed` Clang         |    26.13 MB    |    53.73 MB   |    369.10 MB  |  3,529.94 MB  |
| `phd::embed` GCC       |    38.80 MB    |    40.10 MB   |     59.06 MB  |    208.52 MB  |
| `phd::embed` Clang     |    58.43 MB    |    59.19 MB   |     66.58 MB  |    172.94 MB  |
| `xxd`-generated GCC    |    24.85 MB    |   134.34 MB   |  1,347.00 MB  | 12,622.00 MB  |
| `xxd`-generated Clang  |    41.83 MB    |   103.76 MB   |    718.00 MB  |  7,116.00 MB  |
| `xxd`-generated MSVC   |   ~48.60 MB    |  ~477.30 MB   | ~5,280.00 MB  | Out of Memory |
| `xxd`-generated Circle |    36.58 MB    |    89.10 MB   |    608.39 MB  |  5,739.57 MB  |
| Circle `@array`        |    53.69 MB    |    54.73 MB   |     65.88 MB  |    176.44 MB  |
| Circle `@embed`        |    33.34 MB    |    33.34 MB   |     39.41 MB  |    113.12 MB  |
| `objcopy` (linker)     |    17.31 MB    |    17.31 MB   |     17.31 MB  |     57.13 MB  |

| Strategy               |    400 megabytes    |      1 gigabyte     |
|------------------------|---------------------|---------------------|
| `#embed` GCC           |     3,995.34 MB     |      9,795.31 MB    |
| `#embed` Clang         |    Out of Memory    |     Out of Memory   |
| `phd::embed` GCC       |     1,494.66 MB     |      5,279.37 MB    |
| `phd::embed` Clang     |     1,278.85 MB     |      3,195.58 MB    |
| `xxd`-generated GCC    |    Out of Memory    |     Out of Memory   |
| `xxd`-generated Clang  |    Out of Memory    |     Out of Memory   |
| `xxd`-generated MSVC   |    Out of Memory    |     Out of Memory   |
| `xxd`-generated Circle | Compiler File Limit | Compiler File Limit |
| Circle `@array`        |     1,282.34 MB     |      3,199.28 MB    |
| Circle `@embed`        |       850.40 MB     |      2,128.36 MB    |
| `objcopy` (linker)     |       425.77 MB     |      1,064.74 MB    |

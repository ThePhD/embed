#include <phd/embed.hpp>

#include <array>
#include <cstddef>

#depend "zres/*"

using big_buffer = std::array<char, 4096>;
using filename_buffer = std::array<char, 256>;

consteval std::string_view stem_root (std::string_view file) noexcept {
	const std::size_t last_index = file.find_last_of("\\/");
	if (last_index == std::string_view::npos) {
		return std::string_view(file.data(), 0);
	}
	return std::string_view(file.data(), last_index);
}

consteval void recursive_parse (std::string_view file, big_buffer& buf, std::size_t& buf_i, int depth) {
	auto data = std::embed<char>(file);
	for (std::size_t i = 0; i < data.size(); ++i) {
		const char& c = data[i];
		switch (c) {
		case 'r': {
			std::size_t characters_left = data.size() - i;
			if (characters_left < 10) {
				buf[buf_i] = c;
				++buf_i;
				break;
			}
			std::string_view maybe_requires(&c, 9);
			const bool single_quote = maybe_requires == "require '";
			const bool double_quote = maybe_requires == "require \"";
			if (!single_quote && !double_quote) {
				buf[buf_i] = c;
				++buf_i;
				break;
			}
			// we start a "require" check
			const char* new_file_start = &c + 9;
			const char* new_file_finish = new_file_start;
			std::size_t file_read_i = i + 9;
			for (; file_read_i < data.size(); ++file_read_i) {
				const char& fc = data[file_read_i];
				if (single_quote && fc == '\'') {
					new_file_finish = &fc;
					break;
				}
				if (double_quote && fc == '"') {
					new_file_finish = &fc;
					break;
				}
				if (fc == '\n' || fc == '\r') {
					// directive is over, probably an error
					throw "bad end of require directive";
				}
			}
			if (new_file_start == new_file_finish) {
				throw "require directive must be supplied a proper string name";
			}
			auto base = stem_root(file);
			filename_buffer new_file_buffer = {};
			std::size_t new_file_buffer_i = 0;
			buf[buf_i] = '-';
			++buf_i;
			buf[buf_i] = '-';
			++buf_i;
			buf[buf_i] = ' ';
			++buf_i;
			if (!base.empty()) {
				for (auto base_i = base.cbegin(), base_finish = base.cend(); base_i != base_finish; ++base_i) {
					new_file_buffer[new_file_buffer_i] = *base_i;
					++new_file_buffer_i;
				}
				new_file_buffer[new_file_buffer_i] = '/';
				++new_file_buffer_i;
			}
			for (const char* new_file_i = new_file_start; new_file_i != new_file_finish; ++new_file_i) {
				buf[buf_i] = *new_file_i;
				++buf_i;
				new_file_buffer[new_file_buffer_i] = *new_file_i;
				++new_file_buffer_i;
			}
			buf[buf_i] = '\n';
			++buf_i;

			std::string_view new_file = std::string_view(new_file_buffer.data(), new_file_buffer_i);
			recursive_parse(new_file, buf, buf_i, depth + 1);
			i = file_read_i;
			break;
		}
		default:
			buf[buf_i] = c;
			++buf_i;
			break;
		}
	}
}

consteval big_buffer recursive_parse (std::string_view file) {
	std::size_t buf_i = 0;
	big_buffer buffer = {};
	recursive_parse(file, buffer, buf_i, 0);
	return buffer;
}

#include <type_traits>
#include <stdio.h>

template <std::size_t Sz, bool AddZero = false>
consteval auto comptime_make_array (const auto& S) {
	std::array<std::remove_cvref_t<decltype(S[0])>, Sz + static_cast<std::size_t>(AddZero)> r = {};
	for (std::size_t i = 0; i < S.size(); ++i) {
		r[i] = S[i];
	}
	return r;
}

int main() {
	constexpr auto original_file_span = std::embed<char>("zres/main.lua");
	constexpr auto original_file = comptime_make_array<original_file_span.size(), true>(original_file_span);
	constexpr big_buffer glob = recursive_parse("zres/main.lua");

	std::printf("original file (\"%s\"):\n\n%s", "zres/main.lua", original_file.data());
	std::printf("\n\n=================================================================================\n");
	std::printf("`consteval recursive_parse` result:\n\n%s", glob.data());

	return 0;
}

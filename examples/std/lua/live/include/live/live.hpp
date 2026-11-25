#ifndef PHD_EXAMPLES_STD_LUA_LIVE_LIVE_HPP
#define PHD_EXAMPLES_STD_LUA_LIVE_LIVE_HPP

#include <phd/embed.hpp>

#include <string_view>
#include <string>
#include <vector>
#include <cstddef>
#include <climits>
#include <algorithm>
#include <fstream>

namespace ctlua {

	constexpr std::string_view stem_root(std::string_view file) noexcept {
		const std::size_t last_index = file.find_last_of("\\/");
		if (last_index == std::string_view::npos) {
			return std::string_view(file.data(), 0);
		}
		return std::string_view(file.data(), last_index);
	}

	namespace dtl {
		constexpr void preprocess(std::string_view file, std::vector<char>& buf, int depth) {
			std::vector<char> maybe_backing_buffer = {};
			std::span<const char> data = {};
			if consteval {
				data = phd::embed<char>(file);
			}
			else {
				throw "the demo shouldn't be running at runtime...";
			}
			for (std::size_t i = 0; i < data.size(); ++i) {
				const char& c = data[i];
				switch (c) {
				case 'r': {
					std::size_t characters_left = data.size() - i;
					if (characters_left < 10) {
						buf.push_back(c);
						break;
					}
					std::string_view maybe_requires(&c, 9);
					const bool single_quote = maybe_requires == "require '";
					const bool double_quote = maybe_requires == "require \"";
					if (!single_quote && !double_quote) {
						buf.push_back(c);
						break;
					}
					// we start a "require" check
					const char* new_file_start  = &c + 9;
					const char* new_file_finish = new_file_start;
					std::size_t file_read_i     = i + 9;
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
					auto base                   = stem_root(file);
					std::string new_file_buffer = { };
					buf.push_back('-');
					buf.push_back('-');
					buf.push_back(' ');
					if (!base.empty()) {
						for (auto base_i = base.cbegin(), base_finish = base.cend(); base_i != base_finish;
						     ++base_i) {
							new_file_buffer.push_back(*base_i);
						}
						new_file_buffer.push_back('/');
					}
					for (const char* new_file_i = new_file_start; new_file_i != new_file_finish; ++new_file_i) {
						buf.push_back(*new_file_i);
						new_file_buffer.push_back(*new_file_i);
					}
					buf.push_back('\n');

					std::string_view new_file = new_file_buffer;
					preprocess(new_file, buf, depth + 1);
					i = file_read_i;
					break;
				}
				default:
					buf.push_back(c);
					break;
				}
			}
		}
	} // namespace dtl

	constexpr std::vector<char> preprocess(std::string_view file) {
		std::vector<char> buffer = { };
		dtl::preprocess(file, buffer, 0);
		return buffer;
	}

	template <phd::fixed_string file>
	constexpr auto preprocess() {
		constexpr auto buffer_size = preprocess(file).size();
		return phd::make_static_c_str<buffer_size>(preprocess(file));
	}
} // namespace ctlua

#endif

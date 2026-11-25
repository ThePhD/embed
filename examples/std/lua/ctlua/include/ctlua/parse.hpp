#ifndef PHD_EXAMPLES_STD_LUA_CTLUA_PARSE_HPP
#define PHD_EXAMPLES_STD_LUA_CTLUA_PARSE_HPP

#include <ctlua/core.hpp>

#include <phd/embed.hpp>

#include <string_view>
#include <string>
#include <vector>
#include <array>
#include <variant>
#include <optional>
#include <cstddef>
#include <climits>
#include <algorithm>
#include <memory>
#include <cassert>
#include <exception>
#include <fstream>

namespace ctlua {

	struct indexed_named_data {
		std::size_t index;
		std::string_view name;
		std::string_view data;
	};

	template <typename T>
	struct indexed {
		std::size_t index;
		T value;
	};

	struct syntax_tree {
		// fixed files
		std::vector<std::unique_ptr<char[]>> file_data_pool = { };
		std::vector<indexed_named_data> file_indices        = { };
		// lex data
		std::vector<std::unique_ptr<char[]>> strings_pool = { };
		std::vector<std::size_t> string_indices           = { };
		std::vector<std::vector<token>> tokens            = { };
		// parse tokens
		std::vector<std::unique_ptr<tree>> trees = { };

		constexpr indexed_named_data add_file(std::string_view name, std::span<const char> data) {
			std::size_t index           = file_data_pool.size();
			const std::size_t data_size = data.size();
			file_data_pool.push_back(std::make_unique<char[]>(data_size + 1));
			char* ptr = file_data_pool[index].get();
			std::copy_n(data.data(), data_size, ptr);
			ptr[data_size] = '\0';
			std::string_view file_data(ptr, data_size);
			indexed_named_data value { index, name, file_data };
			file_indices.push_back(value);
			return value;
		}

		constexpr indexed<std::string_view> add_string(std::string_view str) {
			std::size_t index          = strings_pool.size();
			const std::size_t str_size = str.size();
			strings_pool.push_back(std::make_unique<char[]>(str_size + 1));
			string_indices.push_back(index);
			char* ptr = strings_pool[index].get();
			std::copy_n(str.data(), str_size, ptr);
			ptr[str_size] = '\0';
			std::string_view value(ptr, str_size);
			return { .index = index, .value = value };
		}

		constexpr tree* add_tree(tree t) {
			trees.push_back(std::make_unique<tree>(std::move(t)));
			return trees.back().get();
		}
	};

	namespace dtl {
		inline constexpr token parser_last_token                            = end_token { };

		struct parse_context {
			syntax_tree& root;
			tree* last                                                            = nullptr;
			std::vector<indexed_named_data> files                                 = { };
			std::vector<std::pair<std::size_t, indexed_named_data>> current_stack = { };
			std::string_view current_data                                         = { };
			std::size_t depth                                                     = { };

			constexpr indexed_named_data add_file(std::string_view file) {
				if consteval {
					std::span<const char> file_data = phd::embed<char>(file);
					auto value                      = root.add_file(file, file_data);
					return value;
				}
				else {
					std::ifstream file_stream(std::string(file), std::ios::binary | std::ios::ate);
					if (!file_stream.is_open()) {
						throw std::runtime_error("file open fail: damnit");
					}
					std::vector<char> file_data((std::size_t)file_stream.tellg());
					file_stream.seekg(0, std::ios::beg);
					file_stream.read(file_data.data(), (std::streamsize)file_data.size());
					auto value = root.add_file(file, file_data);
					return value;
				}
			}

			constexpr indexed_named_data enter_file(std::string_view file, std::string_view current) {
				auto it = std::find_if(files.cbegin(), files.cend(),
				                       [&](const indexed_named_data& ind) { return ind.name == file; });
				if (it == files.cend()) {
					it = files.insert(it, add_file(file));
				}
				++depth;
				if (!current.empty()) {
					auto& data_file = current_stack.back();
					data_file.first = current.data() - data_file.second.data.data();
				}
				current_stack.push_back(std::pair<std::size_t, indexed_named_data>(0, *it));
				current_data = it->data;
				return *it;
			}

			constexpr void exit_file() {
				if (current_stack.empty())
					std::terminate();
				current_stack.pop_back();
				--depth;
				if (!current_stack.empty()) {
					const auto& data_file_top = current_stack.back();
					current_data              = data_file_top.second.data;
					current_data              = current_data.substr(data_file_top.first);
				}
				else {
					current_data = "";
				}
			}
		};

		struct codepoint_advance {
			char32_t codepoint;
			std::size_t advance;
		};

		inline constexpr std::optional<codepoint_advance> read_codepoint(std::string_view current, unsigned char c) {
			if (c < 128u) {
				return codepoint_advance { c, 1uz };
			}
			std::size_t expected_advance = 0;
			char32_t c32                 = 0xFFFFFFFFu;
			if ((c & 0b11100000u) == 0b11000000u && current.size() >= 2) {
				expected_advance = 2;
				c32              = (c & 0b00011111u) << 6 | (static_cast<unsigned char>(current[1]) & 0b00111111u);
			}
			else if ((c & 0b11110000u) == 0b11100000u && current.size() >= 3) {
				expected_advance = 3;
				c32 = (c & 0b00001111u) << 12 | (static_cast<unsigned char>(current[1]) & 0b00111111u) << 6
				     | (static_cast<unsigned char>(current[2]) & 0b00111111u);
			}
			else if ((c & 0b11111000u) == 0b11110000u && current.size() >= 4) {
				expected_advance = 4;
				c32 = (c & 0b00000111u) << 18 | (static_cast<unsigned char>(current[1]) & 0b00111111u) << 12
				     | (static_cast<unsigned char>(current[2]) & 0b00111111u) << 6
				     | (static_cast<unsigned char>(current[3]) & 0b00111111u);
			}
			// FIXME: overlong, etc.
			if (c32 <= 0x1FFFFFu) {
				return codepoint_advance { c32, expected_advance };
			}
			// 5 or 6 bytes or just a plain wrong value
			return std::nullopt;
		}

		inline constexpr std::optional<codepoint_advance> read_codepoint(std::string_view current) {
			if (current.empty())
				return std::nullopt;
			return read_codepoint(current, static_cast<unsigned char>(current[0]));
		}

		inline constexpr std::optional<codepoint_advance> read_codepoint_ahead(std::string_view current,
		                                                                       std::size_t by) {
			if (current.size() < by)
				return std::nullopt;
			return read_codepoint(current.substr(by));
		}

		inline constexpr std::optional<std::size_t> detect_raw_open(parse_context& context,
		                                                            const codepoint_advance& open_bracket,
		                                                            indexed_named_data& current,
		                                                            std::size_t buffer_index) {
			std::size_t open       = 0;
			std::size_t open_index = open_bracket.advance;
			while (open_index < current.data.size() && current.data[open_index] == u'=') {
				++open;
				++open_index;
			}
			if (open_index < current.data.size() && current.data[open_index] == u'[') {
				return open + 1;
			}
			return std::nullopt;
		}

		inline constexpr std::optional<std::size_t> detect_raw_close(parse_context& context,
		                                                             std::size_t open_length_expected,
		                                                             const codepoint_advance& close_bracket,
		                                                             indexed_named_data& current,
		                                                             std::size_t buffer_index) {
			std::size_t open       = 0;
			std::size_t open_index = close_bracket.advance;
			while (open_index < current.data.size() && current.data[open_index] == u'=') {
				++open;
				++open_index;
			}
			if (open_index < current.data.size() && current.data[open_index] == u']'
			    && open == open_length_expected) {
				return open + 1;
			}
			return std::nullopt;
		}

		inline constexpr std::optional<comment_token>
		lex_comment(parse_context& context, const codepoint_advance& first_dash, const codepoint_advance& second_dash,
		            indexed_named_data& current, std::size_t buffer_index) {

			bool raw             = false;
			std::size_t open_len = 0;
			std::size_t i        = first_dash.advance + second_dash.advance;
			if (i >= current.data.size()) {
				return comment_token { { { current.index, buffer_index, i }, ws_comment }, raw, open_len };
			}
			auto maybe_long_start = read_codepoint_ahead(current.data, i);
			if (!maybe_long_start) {
				throw "could not read codepoint: damnit";
			}
			if (maybe_long_start->codepoint == U'[') {
				auto maybe_raw_open = detect_raw_open(context, *maybe_long_start, current, buffer_index);
				if (maybe_raw_open) {
					raw      = true;
					open_len = *maybe_raw_open;
					i += open_len;
				}
			}
			for (; i < current.data.size();) {
				auto maybe_c32a = read_codepoint_ahead(current.data, i);
				if (!maybe_c32a) {
					break;
				}
				const auto& c32a = *maybe_c32a;
				if (raw) {
					if (c32a.codepoint == U']') {
						auto maybe_raw_close = detect_raw_close(context, open_len, c32a, current, buffer_index);
						if (maybe_raw_close) {
							i += *maybe_raw_close;
							break;
						}
					}
				}
				else {
					if (c32a.codepoint == U'\n' || c32a.codepoint == U'\r') {
						break;
					}
				}
				i += c32a.advance;
			}
			return comment_token { { { current.index, buffer_index, i }, ws_comment }, raw, open_len };
		}

		inline constexpr std::optional<whitespace_token> lex_whitespace(parse_context& context,
		                                                                const codepoint_advance& c32a,
		                                                                indexed_named_data& current,
		                                                                std::size_t buffer_index) {
			int ws_type   = whitespace_classify(c32a.codepoint);
			std::size_t i = c32a.advance;
			for (; i < current.data.size();) {
				if (auto maybe_c32a = read_codepoint_ahead(current.data, i)) {
					const auto c32_ws_type = whitespace_classify(maybe_c32a->codepoint);
					if (c32_ws_type != ws_unknown) {
						ws_type |= c32_ws_type;
						i += maybe_c32a->advance;
					}
					else {
						break;
					}
				}
				else {
					throw "could not read codepoint: damnit";
				}
			}
			return whitespace_token { { current.index, buffer_index, i }, (whitespace)ws_type };
		}

		inline constexpr std::optional<integer_token> lex_int(parse_context& context, const codepoint_advance& first_digit,
		                                                      indexed_named_data& current, std::size_t buffer_index) {
			
			bool is_hex = false;
			std::size_t i = first_digit.advance;
			unsigned long long value = 0;
			if (first_digit.codepoint == U'0') {
				if (auto maybe_hex_start = read_codepoint_ahead(current.data, i); maybe_hex_start && maybe_hex_start->codepoint == U'x') {
					// hex starter
					i += maybe_hex_start->advance;
					is_hex = true;
				}
			}
			else {
				value += to_dec_value(first_digit.codepoint);
			}
			for (; i < current.data.size();) {
				auto maybe_c32a = read_codepoint_ahead(current.data, i);
				if (!maybe_c32a) {
					return std::nullopt;
				}
				const auto& c32a = *maybe_c32a;
				if (is_hex) {
					if (!is_hex_digit(c32a.codepoint)) {
						break;
					}
					value *= 16;
					value += to_hex_value(c32a.codepoint);
				}
				else {
					if (!is_dec_digit(c32a.codepoint)) {
						break;
					}
					value *= 10;
					value += to_dec_value(c32a.codepoint);
				}
				i += c32a.advance;
			}
			return integer_token{ { current.index, buffer_index, i }, value, is_hex };
		}

		inline constexpr std::optional<float_token> lex_float(parse_context& context, const codepoint_advance& c32a,
		                                                      indexed_named_data& current, std::size_t buffer_index) {
			return std::nullopt;
		}

		inline constexpr std::optional<std::variant<integer_token, float_token>>
		lex_numeric(parse_context& context, const codepoint_advance& c32a, indexed_named_data& current,
		            std::size_t buffer_index) {
			return std::nullopt;
		}

		inline constexpr std::optional<string_token> lex_string(parse_context& context,
		                                                        const codepoint_advance& open_punc,
		                                                        indexed_named_data& current,
		                                                        std::size_t buffer_index) {
			std::string contents = { };
			std::size_t i        = 0;
			string_style opener  = open_punc.codepoint == U'\'' ? strsty_single : strsty_double;
			bool escaped         = false;
			for (; i < current.data.size(); ++i) {
				auto maybe_c32a = read_codepoint_ahead(current.data, i);
				if (!maybe_c32a) {
					return std::nullopt;
				}
				auto& c32a         = *maybe_c32a;
				const char32_t c32 = c32a.codepoint;
				if (escaped) {
					switch (c32) {
					case U't':
						contents.push_back((char)u8'\t');
						break;
					case U'r':
						contents.push_back((char)u8'\r');
						break;
					case U'n':
						contents.push_back((char)u8'\n');
						break;
					case U'v':
						contents.push_back((char)u8'\v');
						break;
					case U'a':
						contents.push_back((char)u8'\a');
						break;
					case U'b':
						contents.push_back((char)u8'\b');
						break;
					case U'\'':
						contents.push_back((char)u8'\'');
						break;
					case U'\"':
						contents.push_back((char)u8'\"');
						break;
					case U'z': {
						i += c32a.advance;
						for (; i < current.data.size();) {
							auto maybe_ws_c32a = read_codepoint_ahead(current.data, i);
							if (!maybe_ws_c32a) {
								break;
							}
							const auto& ws_c32a   = *maybe_ws_c32a;
							const char32_t ws_c32 = ws_c32a.codepoint;
							if (ws_c32 == U' ' || ws_c32 == U'\t' || ws_c32 == U'\r' || ws_c32 == U'\v'
							    || ws_c32 == U'\n') {
								i += ws_c32a.advance;
								continue;
							}
							break;
						}
					} break;
					case U'u': {
						// unicode character - hex in { ... }
						throw "not implemented: damnit";
					} break;
					case U'x': {
						// hex insert -- up to 255 in exact FF format
						auto maybe_hex0_c32a = read_codepoint_ahead(current.data, i);
						if (!maybe_hex0_c32a) {
							return std::nullopt;
						}
						const auto& hex0_c32a = *maybe_hex0_c32a;
						auto maybe_hex1_c32a  = read_codepoint_ahead(current.data, i + hex0_c32a.advance);
						if (!maybe_hex1_c32a) {
							return std::nullopt;
						}
						const auto& hex1_c32a = *maybe_hex1_c32a;
						i += hex0_c32a.advance + hex1_c32a.advance;
						const bool hex0_is_hex = is_hex_digit(hex0_c32a.codepoint);
						const bool hex1_is_hex = is_hex_digit(hex1_c32a.codepoint);
						if (!hex0_is_hex || !hex1_is_hex) {
							return std::nullopt;
						}
						unsigned int hex0_value = to_hex_value(hex0_c32a.codepoint);
						unsigned int hex1_value = to_hex_value(hex1_c32a.codepoint);
						unsigned int hex        = hex0_value * 16 + hex1_value;
						contents.push_back((char)(unsigned char)hex);
					} break;
					case U'0':
					case U'1':
					case U'2':
					case U'3':
					case U'4':
					case U'5':
					case U'6':
					case U'7':
					case U'8':
					case U'9': {
						// decimal insert - up to 255 in d[d[d]] format
						unsigned int charval = to_dec_value(c32);
						i += c32a.advance;
						for (std::size_t d = 0; i < current.data.size() && d < 2; ++d) {
							auto maybe_d_c32a = read_codepoint_ahead(current.data, i);
							if (!maybe_d_c32a) {
								// time to leave!
								contents.push_back((char)(unsigned char)charval);
								break;
							}
							const auto& d_c32a   = *maybe_d_c32a;
							const char32_t d_c32 = d_c32a.codepoint;
							if (!is_dec_digit(d_c32)) {
								// time to leave!
								contents.push_back((char)(unsigned char)charval);
								break;
							}
							i += d_c32a.advance;
							// add it to the value, loop around
							charval *= 10;
							charval += to_dec_value(d_c32);
						}
					} break;
					default:
						// illegal escape
						return std::nullopt;
					}
					escaped = false;
					continue;
				}
				else {
					if ((c32 == U'\'' && opener == strsty_single) || (c32 == U'\"' && opener == strsty_double)) {
						// convert contents, end of string
						i += c32a.advance;
						break;
					}
					if (c32 == U'\n' || c32 == U'\r') {
						// illegal
						return std::nullopt;
					}
					if (c32 == U'\\') {
						escaped = true;
						continue;
					}
				}
				// just paste it in
				contents.append(current.data.substr(i, c32a.advance));
				i += c32a.advance;
			}
			auto contents_view = context.root.add_string(contents);
			return string_token { { current.index, buffer_index, i }, opener, 0, contents_view.value };
		}

		inline constexpr std::optional<string_token>
		lex_raw_string(parse_context& context, const codepoint_advance& open_bracket, std::size_t open_length,
		               indexed_named_data& current, std::size_t buffer_index) {
			std::size_t i        = open_bracket.advance + open_length;
			std::string contents = { };
			// accumulate string contents
			while (i < current.data.size()) {
				auto maybe_c32a = read_codepoint_ahead(current.data, i);
				if (!maybe_c32a) {
					return std::nullopt;
				}
				auto& c32a = *maybe_c32a;
				if (c32a.codepoint == ']') {
					if (auto maybe_is_close
					    = detect_raw_close(context, open_length, c32a, current, buffer_index)) {
						const std::size_t buffer_count = i + c32a.advance + *maybe_is_close;
						// skip starting newlines and ending newlines
						while (whitespace_classify(contents.back()) == ws_newline) {
							contents.pop_back();
						}
						while (whitespace_classify(contents.front()) == ws_newline) {
							// inefficient, but. you know.
							contents.erase(0, 1);
						}
						auto contents_view = context.root.add_string(contents);
						return string_token { { current.index, buffer_index, buffer_count },
							                 strsty_raw,
							                 open_length,
							                 contents_view.value };
					}
				}
				contents.append(current.data.substr(i, c32a.advance));
				i += c32a.advance;
			}
			return std::nullopt;
		}

		inline constexpr std::optional<identifier_token> lex_identifier(parse_context& context,
		                                                                const codepoint_advance& first_c32a,
		                                                                indexed_named_data& current,
		                                                                std::size_t buffer_index) {
			std::size_t i = first_c32a.advance;
			for (auto maybe_c32a = read_codepoint_ahead(current.data, i); maybe_c32a;
			     maybe_c32a      = read_codepoint_ahead(current.data, i)) {
				const auto& c32a       = *maybe_c32a;
				const char32_t c32     = c32a.codepoint;
				const bool add_to_list = c32 >= 0x30
				     && ((c32 >= U'A' && c32 <= U'z') || (c32 >= U'0' && c32 <= U'9'))
				     && (std::find(std::cbegin(punctuators), std::cend(punctuators), c32) == std::cend(punctuators))
				     && !(c32 == U' ' || c32 == U'\t' || c32 == U'\v' || c32 == U'\n' || c32 == U'\r');
				if (add_to_list) {
					// continue for sure
					i += c32a.advance;
					continue;
				}
				return identifier_token { { current.index, buffer_index, i } };
			}
			return std::nullopt;
		}

		inline constexpr std::optional<keyword_token> lex_keyword(parse_context& context,
		                                                          const codepoint_advance& c32a,
		                                                          indexed_named_data& current,
		                                                          std::size_t buffer_index) {
			auto maybe_id = lex_identifier(context, c32a, current, buffer_index);
			if (!maybe_id) {
				return std::nullopt;
			}
			const auto& id = *maybe_id;
			std::string_view maybe_kw(current.data.data(), id.loc_size);
			for (const auto& kw : keywords) {
				if (kw == maybe_kw) {
					return keyword_token { { id.loc_file_index, id.loc_start, id.loc_size },
						                  static_cast<keyword>(&kw - &keywords[0]) };
				}
			}
			return std::nullopt;
		}

		inline constexpr std::optional<punctuator_token> lex_punctuator(parse_context& context,
		                                                                const codepoint_advance& c32a,
		                                                                indexed_named_data& current,
		                                                                std::size_t buffer_index) {
			switch (c32a.codepoint) {
			case U'+':
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_plus };
			case U'%':
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_percent };
			case U'#':
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_hash };
			case U'&':
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_ampersand };
			case U'|':
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_bar };
			case U'(':
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_leftparen };
			case U')':
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_rightparen };
			case U'{':
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_leftbrace };
			case U'}':
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_rightbrace };
			case U';':
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_semicolon };
			case U'~':
				if (auto maybe_equal = read_codepoint_ahead(current.data, c32a.advance);
				    maybe_equal && maybe_equal->codepoint == U'=') {
					const auto& equal = *maybe_equal;
					return punctuator_token { { current.index, buffer_index, c32a.advance + equal.advance },
						                     punc_tildeequal };
				}
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_tilde };
			case U'/':
				if (auto maybe_slash = read_codepoint(current.data, c32a.advance);
				    maybe_slash && maybe_slash->codepoint == U'/') {
					const auto& slash = *maybe_slash;
					return punctuator_token { { current.index, buffer_index, c32a.advance + slash.advance },
						                     punc_slashslash };
				}
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_slash };
			case U'<':
				if (auto maybe_less_or_equal = read_codepoint(current.data, c32a.advance)) {
					const auto& less_or_equal = *maybe_less_or_equal;
					if (less_or_equal.codepoint == U'<') {
						return punctuator_token {
							{ current.index, buffer_index, c32a.advance + less_or_equal.advance }, punc_lessless
						};
					}
					else if (less_or_equal.codepoint == U'=') {
						return punctuator_token {
							{ current.index, buffer_index, c32a.advance + less_or_equal.advance }, punc_lessequal
						};
					}
				}
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_less };
			case U'>':
				if (auto maybe_greater_or_equal = read_codepoint(current.data, c32a.advance)) {
					const auto& greater_or_equal = *maybe_greater_or_equal;
					if (greater_or_equal.codepoint == U'>') {
						return punctuator_token { { current.index, buffer_index,
							                       c32a.advance + greater_or_equal.advance },
							                     punc_greatergreater };
					}
					else if (greater_or_equal.codepoint == U'=') {
						return punctuator_token { { current.index, buffer_index,
							                       c32a.advance + greater_or_equal.advance },
							                     punc_greaterequal };
					}
				}
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_less };
			case U'=':
				if (auto maybe_equal = read_codepoint(current.data, c32a.advance)) {
					const auto& equal = *maybe_equal;
					if (maybe_equal->codepoint == U'=') {
						return punctuator_token { { current.index, buffer_index, c32a.advance + equal.advance },
							                     punc_equalequal };
					}
				}
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_equal };
			case U':':
				if (auto maybe_colon = read_codepoint(current.data, c32a.advance)) {
					const auto& colon = *maybe_colon;
					if (colon.codepoint == U':') {
						return punctuator_token { { current.index, buffer_index, c32a.advance + colon.advance },
							                     punc_coloncolon };
					}
				}
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_colon };
			case U'.':
				if (auto maybe_dot1 = read_codepoint_ahead(current.data, c32a.advance);
				    maybe_dot1 && maybe_dot1->codepoint == U'.') {
					const auto& dot1 = *maybe_dot1;
					if (auto maybe_dot2 = read_codepoint_ahead(current.data, c32a.advance + dot1.advance);
					    maybe_dot2 && maybe_dot2->codepoint == U'.') {
						const auto& dot2 = *maybe_dot2;
						return punctuator_token { { current.index, buffer_index,
							                       c32a.advance + dot1.advance + dot2.advance },
							                     punc_dotdotdot };
					}
					return punctuator_token { { current.index, buffer_index, c32a.advance + dot1.advance },
						                     punc_dot };
				}
				return punctuator_token { { current.index, buffer_index, c32a.advance }, punc_dot };
			}
			return std::nullopt;
		}

		constexpr void lex(std::vector<token>& tokens, parse_context& context, indexed_named_data& current) {
			const auto get_last_location_token_ptr = [](const token& tok) {
				return std::visit(
				     [](const auto& t) -> const location_token* {
					     if constexpr (std::is_base_of_v<std::remove_cvref_t<decltype(t)>, location_token>) {
						     return &t;
					     }
					     return nullptr;
				     },
				     tok);
			};
			std::size_t buffer_index = 0;
			for (; !current.data.empty();) {
				const unsigned char c     = static_cast<unsigned char>(current.data[0]);
				char32_t c32              = U'\0';
				std::size_t advance       = 0;
				codepoint_advance c32a    = { };
				const location_token* loc = nullptr;
				if (auto maybe_code = read_codepoint(current.data, c)) {
					c32a    = *maybe_code;
					c32     = c32a.codepoint;
					advance = c32a.advance;
				}
				else {
					throw "utf8 fail: damnit";
				}
				switch (c32) {
				case U'a':
				case U'b':
				case U'd':
				case U'e':
				case U'f':
				case U'g':
				case U'i':
				case U'l':
				case U'n':
				case U'o':
				case U'r':
				case U't':
				case U'u':
				case U'w': {
					// potential keyword
					if (auto maybe_kw = lex_keyword(context, c32a, current, buffer_index)) {
						// it's a keyword: lex that
						auto& kw = *maybe_kw;
						advance  = kw.loc_size;
						tokens.emplace_back(kw);
						loc = get_last_location_token_ptr(tokens.back());
						break;
					}
					auto maybe_id = lex_identifier(context, c32a, current, buffer_index);
					if (!maybe_id) {
						// this shouldn't be possible: error?
						throw "identifier fail: damnit";
					}
					auto& id = *maybe_id;
					advance  = id.loc_size;
					tokens.emplace_back(id);
					loc = get_last_location_token_ptr(tokens.back());
				} break;
				case U' ':
				case U'\t':
				case U'\n':
				case U'\v': {
					auto maybe_ws = lex_whitespace(context, c32a, current, buffer_index);
					if (!maybe_ws) {
						throw "whitespace fail: damnit";
					}
					auto& ws = *maybe_ws;
					tokens.emplace_back(ws);
					loc = get_last_location_token_ptr(tokens.back());
				} break;
				case U'[': {
					if (auto maybe_raw_open = detect_raw_open(context, c32a, current, buffer_index)) {
						auto maybe_raw_string
						     = lex_raw_string(context, c32a, *maybe_raw_open, current, buffer_index);
						if (!maybe_raw_string) {
							throw "string fail: damnit";
						}
						auto& raw_string = *maybe_raw_string;
						tokens.emplace_back(raw_string);
						loc = get_last_location_token_ptr(tokens.back());
					}
					else {
						auto maybe_p = lex_punctuator(context, c32a, current, buffer_index);
						if (!maybe_p) {
							throw "punctuator fail: damnit";
						}
						auto& p = *maybe_p;
						tokens.emplace_back(p);
						loc = get_last_location_token_ptr(tokens.back());
					}
				} break;
				case U'-': {
					if (auto maybe_dash = read_codepoint(current.data, advance); maybe_dash && maybe_dash->codepoint == U'-') {
						// start a comment
						auto maybe_cm = lex_comment(context, c32a, *maybe_dash, current, buffer_index);
						if (maybe_cm) {
							auto& cm = *maybe_cm;
							tokens.emplace_back(cm);
							loc = get_last_location_token_ptr(tokens.back());
							break;
						}
					}
					auto maybe_p = lex_punctuator(context, c32a, current, buffer_index);
					if (!maybe_p) {
						throw "punctuator fail: damnit";
					}
					auto& p = *maybe_p;
					tokens.emplace_back(p);
					loc = get_last_location_token_ptr(tokens.back());
				} break;
				case U'+':
				case U'/':
				case U'%':
				case U'#':
				case U'&':
				case U'~':
				case U'|':
				case U'<':
				case U'>':
				case U'=':
				case U'(':
				case U')':
				case U'{':
				case U'}':
				case U';':
				case U':':
				case U'.': {
					// punctuator
					auto maybe_p = lex_punctuator(context, c32a, current, buffer_index);
					if (!maybe_p) {
						throw "punctuator fail: damnit";
					}
					auto& p = *maybe_p;
					tokens.emplace_back(p);
					loc = get_last_location_token_ptr(tokens.back());
				} break;
				default:
				     /* what is going on here?? */;
					advance = 1;
					throw "unknown fail: damnit";
					break;
				}
				if (loc) {
					advance = loc->loc_size;
				}
				buffer_index += advance;
				current.data = current.data.substr(advance);
			}
		}

		constexpr std::vector<token> lex(parse_context& context, indexed_named_data& current) {
			std::vector<token> tokens = { };
			lex(tokens, context, current);
			return tokens;
		}

		struct parse_cursor {
			std::span<const token> tokens;
			std::size_t index;

			constexpr bool empty () const noexcept {
				return index >= tokens.size();
			}

			constexpr const token& get () const noexcept {
				return tokens[index];
			}

			constexpr const token& next () noexcept {
				for (; index < tokens.size(); ++index) {
					if (!tokens[index].is_whitespace()) {
						break;
					}
				}
				return get();
			}

			constexpr const token& raw_next () noexcept {
				if (index >= tokens.size())
					return parser_last_token;
				++index;
				return get();
			}

			constexpr const token& peek (std::size_t ahead = 1) const noexcept {
				std::size_t i = 0, p = 0;
				for (; i < ahead; ++p) {
					std::size_t target_index = index + p;
					if (target_index >= tokens.size()) {
						return parser_last_token;
					}
					if (!tokens[target_index].is_whitespace()) {
						++i;
					}
				}
				std::size_t target_index = index + p;
				if (target_index >= tokens.size())
					return parser_last_token;
				return tokens[target_index];
			}
		};

		constexpr std::optional<statement> parse_statement (parse_context& context, parse_cursor& cursor) {
			if (auto maybe_punc = cursor.get().as_ptr<punctuator_token>()) {
				const punctuator_token& punc = *maybe_punc;
				if (punc.punc == punc_semicolon) {
					// empty statement
					cursor.next();
					return empty_statement {};
				}
			}
			return std::nullopt;
		}

		constexpr std::optional<block> parse_block (parse_context& context, parse_cursor& cursor) {
			return std::nullopt;
		}

		constexpr chunk parse_chunk(parse_context& context, parse_cursor& cursor, tree* parent) {
			chunk ck = {};
			ck.parent = parent;
			for (;!cursor.empty();) {
				auto maybe_statement = parse_statement(context, cursor);
				if (!maybe_statement) {
					throw "failure to parse statement: damnit";
				}
				tree* stmt = context.root.add_tree(std::move(*maybe_statement));
				ck.statements.push_back(stmt);
			}
			return ck;
		}

		constexpr void parse_global_chunk(parse_context& context, parse_cursor& cursor) {
			parse_chunk(context, cursor, nullptr);
		}

		constexpr void parse_global_chunk(parse_context& context, std::span<const token> tokens) {
			parse_cursor cursor = { .tokens = tokens, .index = 0 };
			parse_global_chunk(context, cursor);
		}

		constexpr void parse(parse_context& context, indexed_named_data& current) {
			auto tokens = lex(context, current);
			context.root.tokens.push_back(std::move(tokens));
			parse_global_chunk(context, context.root.tokens.back());
		}

		constexpr void parse(std::string_view file, parse_context& context) {
			indexed_named_data current = context.enter_file(file, "");
			parse(context, current);
			context.exit_file();
		}
	} // namespace dtl

	inline constexpr syntax_tree parse(std::string_view file) {
		syntax_tree ast = { };
		dtl::parse_context context { .root = ast };
		dtl::parse(file, context);
		return ast;
	}

	template <phd::fixed_string file>
	inline constexpr auto parse() {
		if consteval {
			constexpr syntax_tree tree = parse(file);
			return tree;
		}
		else {
			return parse(file);
		}
	}

	inline constexpr std::string preprocess(std::string_view file) {
		syntax_tree ast = { };
		dtl::parse_context context { .root = ast };
		dtl::parse(file, context);
		return "";
	}

	template <phd::fixed_string file>
	inline constexpr auto preprocess() {
		if consteval {
			constexpr std::string single = preprocess(file);
			constexpr std::size_t single_size = single.size();
			return phd::make_static_c_str<single_size>(single);
		}
		else {
			return preprocess(file);
		}
	}
} // namespace ctlua

#endif

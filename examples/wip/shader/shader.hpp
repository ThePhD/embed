#ifndef PHD_EMBED_EXAMPLE_SHADER_HPP
#define PHD_EMBED_EXAMPLE_SHADER_HPP

#include <phd/embed/embed.hpp>
#include <span>
#include <array>
#include <iterator>
#include <string_view>
#include <cstddef>

struct finish {};

struct parse_count {
  std::size_t size;
  std::size_t exact_file_size;

  constexpr std::size_t operator()(char) noexcept {
    size += 1;
    exact_file_size += 1;
    return 1;
  }

  constexpr std::size_t operator()(std::string_view directive) noexcept {
    if (directive.starts_with("include")) {
      size += 1 + 3 + directive.size();
    }
    else {
      size += 1 + directive.size();
    }
    exact_file_size += 1 + directive.size();
    return 1 + directive.size();
  }

  constexpr void operator()(finish) noexcept {}
};

template <std::size_t N>
struct parse_load {
  std::array<char, N + 1> data;
  std::size_t write_index;

  constexpr parse_load () noexcept : data(), write_index(0) {}

  constexpr std::size_t operator()(char c) noexcept {
    data[write_index] = c;
    ++write_index;
    return 1;
  }

  constexpr std::size_t operator()(std::string_view directive) noexcept {
    if (directive.starts_with("include")) {
      data[write_index] = '/';
      ++write_index;
      data[write_index] = '/';
      ++write_index;
    }
    data[write_index] = '#';
    ++write_index;
    for (std::size_t i = 0; i < directive.size(); ++i, (void)++write_index)
      data[write_index] = directive[i];
    return 1 + directive.size();
  }

  constexpr void operator()(finish) noexcept {
    data[write_index] = '\0';
    ++write_index;
  }
};

template <typename Action>
consteval void preprocess_shader(std::string_view file_name, std::span<const char> data, Action& action, int depth = 0);

consteval std::array<char, 2048> find_file(std::string_view file_base, std::string_view directive_file) {
  std::array<char, 2048> include_buffer{};
  std::size_t ib = 4;
  for (std::size_t i = 0; i < file_base.size(); ++i, ++ib)
    include_buffer[ib] = file_base[i];
  include_buffer[ib++] = '/';
  for (std::size_t i = 0; i < directive_file.size(); ++i, ++ib)
    include_buffer[ib] = directive_file[i];
  ib -= 4;
  include_buffer[0] = (ib & 0xFF);
  include_buffer[1] = (ib & 0xFF00) >> 8;
  include_buffer[2] = (ib & 0xFF000) >> 16;
  include_buffer[3] = (ib & 0xFF00000) >> 24;
  return include_buffer;
}

template <typename Action>
consteval void preprocess_shader_lex_include(std::string_view file_name, std::string_view directive, Action& action, int depth) {
  std::size_t inc_quote = directive.find('"');
  if (inc_quote == std::string_view::npos) {
    return;
  }
  std::size_t inc_quote_end = directive.find('"', inc_quote + 1);
  if (inc_quote_end == std::string_view::npos) {
    return;
  }
  std::size_t inc_slash = file_name.rfind('/');
  if (inc_slash == std::string_view::npos)
    inc_slash = file_name.size();
  std::string_view file_base = file_name.substr(0, inc_slash);
  std::string_view directive_file = directive.substr(inc_quote + 1, inc_quote_end - inc_quote - 1);
  std::array<char, 2048> include_buffer = find_file(file_base, directive_file);
  std::size_t include_buffer_size = 0;
  include_buffer_size |= static_cast<std::size_t>(include_buffer[0]);
  include_buffer_size |= static_cast<std::size_t>(include_buffer[1]) << 8;
  include_buffer_size |= static_cast<std::size_t>(include_buffer[2]) << 16;
  include_buffer_size |= static_cast<std::size_t>(include_buffer[3]) << 24;
  std::string_view included_file_name(include_buffer.data() + 4, include_buffer_size);
  std::span<const char> included_file = phd::embed<char>(included_file_name);
  preprocess_shader(included_file_name, included_file, action, depth + 1);
}

template <typename Action>
consteval void preprocess_shader(std::string_view file_name, std::span<const char> data, Action& action, int depth) {
  std::string_view shader_text(data.data(), data.size());
  for (std::size_t i = 0; i < shader_text.size();) {
    char currentc = shader_text[i];
    if (currentc == '#') {
      std::string_view directive = shader_text.substr(i + 1);
      std::size_t end_of_directive_index = directive.find_first_of("\r\n");
      if (end_of_directive_index == std::string::npos) {
        end_of_directive_index = shader_text.size();
      }
      directive = shader_text.substr(i + 1, end_of_directive_index);
      if (directive.starts_with("include")) {
        preprocess_shader_lex_include(file_name, directive, action, depth);
      }
      else if (directive.starts_with("version")) {
        // ok
      }
      else if (directive.starts_with("if")) {
        // ok
      }
      else if (directive.starts_with("ifdef")) {
        // ok
      }
      else if (directive.starts_with("ifndef")) {
        // ok
      }
      else if (directive.starts_with("elif")) {
        // ok
      }
      else if (directive.starts_with("endif")) {
        // ok
      }
      else {
        throw std::runtime_error("invalid directive");
      }
      i += action(directive);
      continue;
    }
    i += action(currentc);
  }
}

consteval std::size_t parse_shader_size(std::string_view file_name, std::span<const char> main_data) {
  parse_count pc{};
  preprocess_shader(file_name, main_data, pc);
  pc(finish{});
  return pc.size;
}

template <std::size_t N>
consteval auto parse_shader(std::string_view file_name, std::span<const char> main_data) {
  parse_load<N> pl{};
  preprocess_shader(file_name, main_data, pl);
  pl(finish{});
  return std::move(pl.data);
}


#endif // PHD_EMBED_EXAMPLE_SHADER_HPP

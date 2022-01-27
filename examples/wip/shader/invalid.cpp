#depend "assets/shaders/nymph.vxglsl"
#depend "assets/shaders/nymph.pxglsl"

#include <phd/embed/embed.hpp>
#include <span>
#include <array>
#include <iterator>
#include <string_view>
#include <cstddef>



#include <iostream>

int main() {
  static constexpr std::string_view px_shader_main_file = "assets/shaders/invalid.pxglsl";
  static constexpr std::span<const char> px_shader_main =
      phd::embed<char>(px_shader_main_file);

  // these will never evaluate, ever
  static constexpr std::size_t px_shader_size = parse_shader_size(px_shader_main_file, px_shader_main);
  constexpr auto px_shader = parse_shader<px_shader_size>(px_shader_main_file, px_shader_main);

  return 0;
}

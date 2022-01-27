#depend "assets/shaders/*"



#include <iostream>

int main() {
  static constexpr std::string_view vx_shader_main_file = "assets/shaders/nymph.vxglsl";
  static constexpr std::string_view px_shader_main_file = "assets/shaders/nymph.pxglsl";
  static constexpr std::span<const char> vx_shader_main =
      phd::embed<char>(vx_shader_main_file);
  static constexpr std::span<const char> px_shader_main =
      phd::embed<char>(px_shader_main_file);
  static constexpr std::span<const unsigned short> foo =
      nphd::embed<unsigned short>(px_shader_main_file);
  static constexpr std::size_t vx_shader_size = parse_shader_size(vx_shader_main_file, vx_shader_main);
  static constexpr std::size_t px_shader_size = parse_shader_size(px_shader_main_file, px_shader_main);
  constexpr auto vx_shader = parse_shader<vx_shader_size>(vx_shader_main_file, vx_shader_main);
  constexpr auto px_shader = parse_shader<px_shader_size>(px_shader_main_file, px_shader_main);

  std::cout << "[[Vertex Shader]] " << vx_shader.size() << "\n";
  std::cout << vx_shader.data();
  std::cout << std::endl;

  std::cout << "[[Pixel Shader]] " << px_shader.size() << "\n";
  std::cout << px_shader.data();
  std::cout << std::endl;

  return 0;
}

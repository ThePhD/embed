#include <ctlua/ctlua.hpp>

#include <phd/embed.hpp>

#depend "resources/**"

int main(int, char*[]) {
	[[maybe_unused]] auto lua_code = ctlua::preprocessor("resources/lua/main.lua");

	//std::printf("original file (\"%s\"):\n\n%s", "resources/main.lua", original_file.data());
	//std::printf("\n\n=================================================================================\n");
	std::printf("`consteval preprocessor` result:\n\n%s", lua_code.data());
	
	return 0;
}

#depend "resources/lua/main.lua"

#include <live/live.hpp>

#include <phd/embed.hpp>

#include <sol/sol.hpp>

#include <cstdio>
#include <cassert>

int main(int, char*[]) {
	constexpr const char original_file[] = {
#embed <resources/lua/main.lua> suffix(, 0)
	};
#if 0
	constexpr auto lua_code = ctlua::preprocess<"resources/lua/main.lua">();

	std::printf("original file (\"%s\"):\n\n%s", "resources/main.lua", original_file);
	std::printf("\n\n=================================================================================\n");
	std::printf("`consteval preprocess` result:\n\n%s", lua_code.data());

	{
		// works ✅
		sol::state lua;
		auto result = lua.safe_script(std::string_view(lua_code.data()));
		if (result.valid()) {
			int value = lua["main"]();
			std::printf("lua code result:\n%d\n", value);
		}
		else {
			std::printf("original file failed to run\n");
		}
	}
	{
		// fails ❌
		sol::state lua;
		auto result = lua.safe_script(original_file);
		if (result.valid()) {
			int value = lua["main"]();
			std::printf("original file result:\n%d\n", value);
		}
		else {
			std::printf("original file failed to run\n");
		}
	}
	return 0;
#else
	constexpr auto lua_code = phd::embed("resources/lua/main.lua");
	return (int)lua_code[0] == (int)original_file[0] ? 0 : 1;
#endif
}

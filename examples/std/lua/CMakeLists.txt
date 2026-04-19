add_executable(lua lua.c++)
target_common_options(lua)
target_compile_options(lua
	PRIVATE -embed-dir=${CMAKE_CURRENT_SOURCE_DIR}
)

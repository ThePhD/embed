# FILE
add_executable(FILE source/FILE.c)
target_common_options(FILE)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/FILE.c
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/source/FILE.c
		${CMAKE_CURRENT_BINARY_DIR}/FILE.c
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/FILE.c
)
add_custom_target(FILE-copy
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/FILE.c
)
add_dependencies(FILE FILE-copy)
add_test(NAME FILE COMMAND FILE)

# FILE.limit
add_executable(FILE.limit source/FILE.limit.c)
target_common_options(FILE.limit)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/FILE.limit.c
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/source/FILE.limit.c
		${CMAKE_CURRENT_BINARY_DIR}/FILE.limit.c
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/FILE.limit.c
)
add_custom_target(FILE.limit-copy
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/FILE.limit.c
)
add_dependencies(FILE.limit FILE.limit-copy)
add_test(NAME FILE.limit COMMAND FILE.limit)

# has_embed
add_executable(has_embed source/has_embed.c)
target_common_options(has_embed)
add_test(NAME has_embed COMMAND has_embed)

# is_empty
add_executable(is_empty source/is_empty.c)
target_common_options(is_empty)
add_test(NAME is_empty COMMAND is_empty)

# macro_expansion
add_executable(macro_expansion source/macro_expansion.c)
target_common_options(macro_expansion)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/macro_expansion.c
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/source/macro_expansion.c
		${CMAKE_CURRENT_BINARY_DIR}/macro_expansion.c
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/macro_expansion.c
)
add_custom_target(macro_expansion-copy
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/macro_expansion.c
)
add_dependencies(macro_expansion macro_expansion-copy)
add_test(NAME macro_expansion COMMAND macro_expansion)

# path.quote
add_executable(path.quote source/path.quote.c)
target_common_options(path.quote)
add_test(NAME path.quote COMMAND path.quote)

# path.chevron
add_executable(path.chevron source/path.chevron.c)
target_common_options(path.chevron)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/art.txt
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/assets/media/art.txt
		${CMAKE_CURRENT_BINARY_DIR}/art.txt
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/media/art.txt
)
add_custom_target(path.chevron-copy
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/art.txt
)
add_dependencies(path.chevron path.chevron-copy)
add_test(NAME path.chevron COMMAND path.chevron)

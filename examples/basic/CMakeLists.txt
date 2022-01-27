# hi
add_executable(hi source/hi.c)
target_common_options(hi)
add_custom_command(OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/hi.c
	COMMAND ${CMAKE_COMMAND} -E copy
		${CMAKE_CURRENT_SOURCE_DIR}/source/hi.c
		${CMAKE_CURRENT_BINARY_DIR}/hi.c
	DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/source/hi.c
)
add_custom_target(hi-copy
	DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/hi.c
)
add_dependencies(hi hi-copy)
add_test(NAME hi COMMAND hi)

# urandom
if (EXISTS /dev/urandom)
	add_executable(urandom source/urandom.c)
	target_common_options(urandom)
	add_test(NAME urandom COMMAND urandom)
endif()

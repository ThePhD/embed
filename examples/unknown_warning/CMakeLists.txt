add_executable(unknown_warning source/unknown_warning.c)
target_common_options(unknown_warning)
target_compile_options(unknown_warning
	PRIVATE
	-Wno-unknown-directive-parameters
)
add_test(NAME unknown_warning COMMAND unknown_warning)

add_executable(unknown_warning.compile EXCLUDE_FROM_ALL source/unknown_warning.c)
target_common_options(unknown_warning.compile)
# force the unknown_warning.compile target
# to rebuild every time, by deleting its object and executable files!
add_test(NAME unknown_warning.compile.clean
	COMMAND ${CMAKE_COMMAND} -E rm -f
		$<TARGET_FILE:unknown_warning.compile>
		$<TARGET_OBJECTS:unknown_warning.compile>
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)
add_test(NAME unknown_warning.compile
	COMMAND ${CMAKE_COMMAND} --build .
		--target unknown_warning.compile
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
set_tests_properties(unknown_warning.compile
	PROPERTIES
	DEPENDS unknown_warning.compile.clean
	PASS_REGULAR_EXPRESSION "5.*unknown-directive-parameters;6.*unknown-directive-parameters"
)

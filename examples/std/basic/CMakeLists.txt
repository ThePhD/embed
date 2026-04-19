add_executable(license license.c++)
target_common_options(license)
target_compile_options(license
	PRIVATE --embed-dir=${CMAKE_SOURCE_DIR}
)

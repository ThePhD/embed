cmake_minimum_required(VERSION 3.15.0)

project(directory_iteration_benchmark VERSION 1.0.0 LANGUAGES C CXX)

file(GLOB_RECURSE directory_iteration_benchmark.sources LIST_DIRECTORIES false CONFIGURE_DEPENDS "sources/*.cpp")

add_exectable(directory_iteration_benchmark ${directory_iteration_benchmark.sources})
target_include_directories(directory_iteration_benchmark
	PRIVATE
	../../vendor/llfio/include)
target_compile_definitions(directory_iteration_benchmark
	PUBLIC
	LLFIO_HEADERS_ONLY=1
	LLFIO_EXPERIMENTAL_STATUS_CODE=1)

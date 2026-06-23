# ============================================================================
#
# phd.embed
#
# Written in 2019 by ThePhD <phdofthehouse@gmail.com>
#
# To the extent possible under law, the author(s) have dedicated all copyright and related
# and neighboring rights to this software to the public domain worldwide. This software is
# distributed without any warranty.
#
# You should have received a copy of the CC0 Public Domain Dedication along with this software.
# If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
#
# ============================================================================

cmake_minimum_required(VERSION 3.25.0)

# # Project declaration
# informs about the project, gives a description, version and MOST IMPORTANTLY
# the languages the project is going to use. Required.
project(phd.embed VERSION 1.0.0 DESCRIPTION "A spicy library on top of compiler builtins." LANGUAGES C CXX)

# # Modules
# Include modules useful to the project, whether locally made in our own cmake DIRECTORY
# our from the standard cmake libraries
# Add home-rolled modules path to front of module path list
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

# # Include standard modules
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)
include(CheckCompilerFlag)

# # Options
option(PHD_EMBED_CI "Whether or not we are in contiguous integration mode" OFF)
option(PHD_EMBED_TESTS "Enable build of tests" OFF)
option(PHD_EMBED_EXAMPLES "Enable build of examples" OFF)
option(PHD_EMBED_BENCHMARKS "Enable build of benchmarks" OFF)

set(phd.embed.sources
	"include/phd/embed/embed.hpp"
)

add_library(phd.embed INTERFACE)
add_library(phd::embed ALIAS phd.embed)
target_link_libraries(phd.embed)
target_include_directories(phd.embed INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)

target_sources(phd.embed INTERFACE ${phd.embed.sources})

# # Benchmarks, Tests, Examples
function(target_common_options target style)
	target_compile_options(${target}
		PRIVATE
		-Wall
		-Wpedantic
		"--embed-dir=${CMAKE_SOURCE_DIR}/examples/assets"
		"--embed-dir=${CMAKE_CURRENT_SOURCE_DIR}/resources"
		"--embed-dir=${CMAKE_SOURCE_DIR}"
	)
	target_compile_definitions(${target}
		PRIVATE
		_CRT_SECURE_NO_WARNINGS=1
		_CRT_SECURE_NO_DEPRECATE=1
	)
	target_include_directories(${target}
		PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
	)
	target_link_libraries(${target}
		PRIVATE
		phd::embed
	)
	if (".c++" STREQUAL ${style})
		target_compile_options(${target}
			PRIVATE
			-std=c++2d
		)
		if (${CMAKE_CXX_COMPILER_ID} MATCHES "[Cc]lang")
			target_compile_options(${target}
				PRIVATE
				-Wno-c++2d-extensions
				-Wno-c2y-extensions
				-Wno-c23-extensions
			)
		endif()
	else()
		target_compile_options(${target}
			PRIVATE
			-std=c2y
		)
	endif()
endfunction()

if (PHD_EMBED_TESTS)
	enable_testing()
endif()
if (PHD_EMBED_EXAMPLES)
	add_subdirectory(examples)
endif()
if (PHD_EMBED_BENCHMARKS)
	add_subdirectory(benchmarks)
endif()

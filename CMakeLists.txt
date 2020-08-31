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

cmake_minimum_required(VERSION 3.13.0)

# # Project declaration
# informs about the project, gives a description, version and MOST IMPORTANTLY
# the languages the project is going to use. Required.
project(phd.embed VERSION 0.3.0 DESCRIPTION "A spicy library on top of compiler builtins." LANGUAGES C CXX)

# # Modules
# Include modules useful to the project, whether locally made in our own cmake DIRECTORY
# our from the standard cmake libraries
# Add home-rolled modules path to front of module path list
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" "${CMAKE_MODULE_PATH}")

# # # Top-Level Directories
# Check if this is the top-level project or not
if(CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
    set(PHD_EMBED_IS_TOP_LEVEL_PROJECT true)
else()
    set(PHD_EMBED_IS_TOP_LEVEL_PROJECT false)
endif()

# Modify bad flags / change defaults if that is the case
if (PHD_EMBED_IS_TOP_LEVEL_PROJECT)
	if (CMAKE_SIZEOF_VOID_P EQUAL 4)
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/${CFG_INT_DIR}/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/${CFG_INT_DIR}/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x86/${CMAKE_BUILD_TYPE}/${CFG_INT_DIR}/bin")
	else()
		set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/${CFG_INT_DIR}/lib")
		set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/${CFG_INT_DIR}/bin")
		set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/x64/${CMAKE_BUILD_TYPE}/${CFG_INT_DIR}/bin")
	endif()

	if (MSVC)
		string(REGEX REPLACE "/W[0-4]" "" CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
		string(REGEX REPLACE "/W[0-4]" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
	endif()
endif()

# # Include standard modules
include(CMakeDependentOption)
include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

# # Options
option(PHD_EMBED_CI "Whether or not we are in contiguous integration mode" OFF)
option(PHD_EMBED_TESTS "Enable build of tests" OFF)
option(PHD_EMBED_EXAMPLES "Enable build of examples" OFF)
option(PHD_EMBED_BENCHMARKS "Enable build of benchmarks" OFF)

set(phd.embed.sources
	"include/phd/embed.hpp"
)

add_library(phd.embed INTERFACE)
add_library(phd::embed ALIAS phd.embed)
target_link_libraries(phd.embed)
target_include_directories(phd.embed INTERFACE 
	$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
	$<INSTALL_INTERFACE:include>
)
if (NOT MSVC)
	target_compile_options(phd.embed INTERFACE 
		-fembed
		-fembed-builtins
	)
endif()

target_sources(phd.embed INTERFACE ${phd.embed.sources})
#set_target_properties(phd.embed
#	PROPERTIES
#	EXPORT_NAME phd::embed
#)

# # Benchmarks, Tests, Examples
if (PHD_EMBED_TESTS)
	enable_testing()
endif()
if (PHD_EMBED_TESTS)
	enable_testing()
	add_subdirectory(tests)
endif()
if (PHD_EMBED_EXAMPLES)
	add_subdirectory(examples)
endif()
if (PHD_EMBED_BENCHMARKS)
	add_subdirectory(benchmarks)
endif()
if (PHD_EMBED_SCRATCH_LIVE_DEMO)
	add_subdirectory(scratch)
endif()

## Root CMake file for libarcstk
## vim:fdm=marker

cmake_minimum_required (VERSION 3.10 )

## --- Policies {{{1

## Set variables corresponding to unspecified versions to the empty string.
## Required for specifying the project version along with the name in project().
## See: https://cmake.org/cmake/help/latest/policy/CMP0048.html
cmake_policy (SET CMP0048 NEW )

## Support IN_LIST if() operator
## https://cmake.org/cmake/help/latest/policy/CMP0057.html
cmake_policy (SET CMP0057 NEW )

## 1}}}

project (libarcstk VERSION 0.3.1 LANGUAGES CXX )

set (LIBARCSTK_VERSION_SUFFIX "alpha.1" )
set (LIBARCSTK_VERSION_STRING "${PROJECT_VERSION}-${LIBARCSTK_VERSION_SUFFIX}" )

set (PROJECT_DESCRIPTION
	"Calculate and Verify AccurateRip Checksums and Ids" )

set (PROJECT_HOMEPAGE_URL "https://github.com/crf8472/${PROJECT_NAME}" )

## --- Abort Release-Builds on Big Endian Platforms {{{1

include (TestBigEndian )
TEST_BIG_ENDIAN (IS_BIG_ENDIAN )

if (IS_BIG_ENDIAN )

	if (CMAKE_BUILD_TYPE STREQUAL "Release" )

		message (FATAL_ERROR
			"Big endian platforms are not yet supported, use -DCMAKE_BUILD_TYPE=Debug to configure your build." )
	else ()

		message (WARNING "Big endian platform support is not yet available" )
	endif ()

endif()

## 1}}}

set (CMAKE_EXPORT_COMPILE_COMMANDS ON) ## required for clang-tidy and iwyu

## --- Includes {{{1
set (cmake_include_dir_ "${CMAKE_CURRENT_SOURCE_DIR}/cmake/functions" )

## Mandatory configuration
include ("${cmake_include_dir_}/ProjectFilesAndDirectories.cmake" )
include ("${cmake_include_dir_}/BuildTreeSetup.cmake" )
include ("${cmake_include_dir_}/CompilerFlags.cmake" )
include ("${cmake_include_dir_}/GitInfo.cmake" )

## Optional support for development
include ("${cmake_include_dir_}/ToolClangTidy.cmake" )
include ("${cmake_include_dir_}/ToolGraphViz.cmake" )
include ("${cmake_include_dir_}/ToolClangIncludeGraph.cmake" )

unset (cmake_include_dir_ )
## 1}}}

git_get_version_string (LIBARCSTK_GIT_VERSION_STRING )
git_get_commit_id      (LIBARCSTK_GIT_COMMIT_ID )

enable_clang_tidy              (LIBARCSTK_CLANG_TIDY_ENABLED )
enable_target_dep_graph_by_dot (LIBARCSTK_TARGET_DEPS_GRAPH_ENABLED )
enable_clang_include_graph     (LIBARCSTK_CLANG_INCLUDE_GRAPH_ENABLED )

message (STATUS "Building ${PROJECT_NAME} ${LIBARCSTK_VERSION_STRING}" )

## --- Setup Default Build Type {{{1

if (NOT CMAKE_BUILD_TYPE )

	set (CMAKE_BUILD_TYPE "Release"
		CACHE STRING "Choose the type of build (e.g. Debug or Release)" FORCE )

	message (STATUS "No build type specified, set to ${CMAKE_BUILD_TYPE}" )
endif (NOT CMAKE_BUILD_TYPE )
## 1}}}

message (STATUS "Build type: ${CMAKE_BUILD_TYPE}" )

## --- Setup Build Tree {{{1
setup_build_tree()

# 1}}}
## --- Trigger Configuration Step Whenever Git Index Changes {{{1

set_property (DIRECTORY APPEND
	PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/.git/index" )

configure_file ("${LIBARCSTK_SOURCE_DIR}/version.cpp.in"
	"${LIBARCSTK_GENSRC_BINARY_DIR}/version.cpp"
	@ONLY )

# 1}}}
## --- Define Project Target {{{1

add_library (${PROJECT_NAME} SHARED
	${LIBARCSTK_SOURCES}
	"${LIBARCSTK_GENSRC_BINARY_DIR}/version.cpp"
)

get_directory_property (HAS_PARENT PARENT_DIRECTORY )

if (HAS_PARENT )
	## Make project's includes available to PARENT
	target_include_directories (${PROJECT_NAME}
		## public headers:
		INTERFACE $<BUILD_INTERFACE:${LIBARCSTK_BINARY_DIR}/include>
	)

	## Provide a namespaced target also when added by add_subdirectory
	add_library (${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME} )
endif()

target_include_directories (${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${LIBARCSTK_INCLUDE_BINARY_DIR}>
		$<INSTALL_INTERFACE:${LIBARCSTK_INCLUDE_INSTALL_SUBDIR}>
	PRIVATE
		"${LIBARCSTK_GENSRC_BINARY_DIR}" ## for generated sources
	)

set_target_properties (${PROJECT_NAME} PROPERTIES
	CXX_STANDARD   17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF ## use -std=c++17 instead of std=gnu++17
	PUBLIC_HEADER  "${LIBARCSTK_PUBLIC_HEADERS}"
	PREFIX         "" ## No 'lib' prefix in soname, PROJECT_NAME is prefixed
	VERSION        "${PROJECT_VERSION}"
	SOVERSION      "${PROJECT_VERSION}" )

add_dependencies (${PROJECT_NAME} libarcstk_link_to_headers )

## 1}}}
## --- Compiler Specific Settings {{{1

apply_compiler_flags (${PROJECT_NAME} )

## 1}}}

get_target_property (LIBARCSTK_CXX_FLAGS ${PROJECT_NAME} COMPILE_OPTIONS )
message (STATUS "Compile flags: ${LIBARCSTK_CXX_FLAGS}" )

## --- Installation {{{1

if (NOT SKIP_INSTALL_ALL )

	add_subdirectory (install )
endif ()

## 1}}}
## --- Optional: Build Tests (default: OFF) {{{1

option (WITH_TESTS "Build unit tests" OFF )

if (WITH_TESTS)

	message (STATUS "Build with tests" )

	enable_testing()

	## Add Catch2 test framework if it is not already present
	if (NOT TARGET Catch2::Catch2 )
		add_subdirectory (thirdparty/catch2 EXCLUDE_FROM_ALL )
	endif()

	## Add tests
	add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/test" )

endif (WITH_TESTS)

## 1}}}
## --- Optional: Build documentation (default: OFF) {{{1

option (WITH_DOCS          "Build documentation for public API"           OFF )
option (WITH_INTERNAL_DOCS "Build documentation also for internal APIs"   OFF )
option (USE_DOC_TOOL       "Build documentation using specified tool"     OFF )
option (WITH_EXAMPLES      "Build examples"                               OFF )

## -- Process configuration for building the documentation
if (WITH_DOCS )

	add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/doc" )
else ()

	if (USE_DOC_TOOL )
		message (FATAL_ERROR "-DWITH_DOCS=ON is required to set USE_DOC_TOOL." )
	endif()
endif (WITH_DOCS )


## -- Require WITH_DOCS to use WITH_INTERNAL_DOCS
if (WITH_INTERNAL_DOCS )

#	set (WITH_DOCS ON CACHE BOOL "Build documentation for public API" FORCE )
	if (NOT WITH_DOCS )
		message (WARNING
			"-DWITH_DOCS=ON is OFF but -DWITH_INTERNAL_DOCS=ON will only "
			"have any effects as soon as -DWITH_DOCS is set ON." )
	endif()
endif()

## 1}}}
## --- Process configuration for building the examples {{{1

if (WITH_EXAMPLES )
	add_subdirectory ("${CMAKE_CURRENT_SOURCE_DIR}/examples" )
endif()

## 1}}}


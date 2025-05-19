## Root CMake file for libarcstk
## vim:fdm=marker

cmake_minimum_required (VERSION 3.10 )
## --- Policies {{{1

## Set variables corresponding to unspecified versions to the empty string.
## Required for specifying the project version along with the name in project().
cmake_policy (SET CMP0048 NEW )

## 1}}}


project (libarcstk VERSION 0.3.0 LANGUAGES CXX )


## --- Setup Short Description, URL and Version Information {{{1

set (PROJECT_BRIEF_DESC
	"Calculate and Verify AccurateRip Checksums and Ids" )

set (PROJECT_URL "https://github.com/crf8472/${PROJECT_NAME}" )

set (PROJECT_VERSION_SUFFIX "alpha.1" )

set (PROJECT_DEFAULT_BUILD_TYPE "Release" )

set (PROJECT_VERSION_STRING_WITHOUT_SUFFIX
	"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
	)

set (PROJECT_VERSION_STRING "${PROJECT_VERSION_STRING_WITHOUT_SUFFIX}" )

if (PROJECT_VERSION_SUFFIX )
	set (PROJECT_VERSION_STRING
		"${PROJECT_VERSION_STRING_WITHOUT_SUFFIX}-${PROJECT_VERSION_SUFFIX}" )
endif (PROJECT_VERSION_SUFFIX )

message (STATUS "Building ${PROJECT_NAME} ${PROJECT_VERSION_STRING}" )



## --- Setup Default Build Type {{{1

if (NOT CMAKE_BUILD_TYPE )

	set (CMAKE_BUILD_TYPE "${PROJECT_DEFAULT_BUILD_TYPE}"
		CACHE STRING "Choose the type of build (e.g. Debug or Release)" FORCE )

	message (STATUS "No build type specified, set to ${CMAKE_BUILD_TYPE}" )
endif (NOT CMAKE_BUILD_TYPE )

message (STATUS "Build type: ${CMAKE_BUILD_TYPE}" )



## --- Trigger Configuration Step Whenever Git Index Changes {{{1

set_property (DIRECTORY APPEND
	PROPERTY CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/.git/index" )



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



## --- Project Directory Layout {{{1

## Source directory for markdown files
set (PROJECT_ROOT_DIR           "${CMAKE_CURRENT_SOURCE_DIR}" )

## Root directory for sources
set (PROJECT_SOURCE_DIR         "${CMAKE_CURRENT_SOURCE_DIR}/src" )

## Root directory for public/exported headers
set (PROJECT_INCLUDE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include" )

## Source directory for tests
set (PROJECT_TEST_SOURCE_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/test/src" )


## Root directory for out-of-source build
set (PROJECT_BINARY_DIR         "${CMAKE_CURRENT_BINARY_DIR}" )

## Subdir for installing includes
set (PROJECT_INCLUDE_INSTALL_SUBDIR "include/arcstk" )

## Target directory for public/exported headers
set (PROJECT_INCLUDE_BINARY_DIR "${PROJECT_BINARY_DIR}/${PROJECT_INCLUDE_INSTALL_SUBDIR}" )

## Target directory for non-cmake logs
set (PROJECT_LOG_DIR            "${PROJECT_BINARY_DIR}/log" )

## Target directory for generated sources and configuration
set (PROJECT_GEN_SOURCES_DIR    "${PROJECT_BINARY_DIR}/generated-sources" )



## --- Create build-tree directories {{{1

## -- Log Directory {{{2

add_custom_command (
	OUTPUT  "${PROJECT_LOG_DIR}"
	COMMAND "${CMAKE_COMMAND}"
	ARGS    -E make_directory "${PROJECT_LOG_DIR}"
	VERBATIM
)

add_custom_target (libarcstk_create_log_dir ALL DEPENDS "${PROJECT_LOG_DIR}" )


## -- Include Directory + Link {{{2

add_custom_command (
	OUTPUT  "${PROJECT_BINARY_DIR}/include"
	COMMAND "${CMAKE_COMMAND}"
	ARGS    -E make_directory "${PROJECT_BINARY_DIR}/include"
	VERBATIM
)

add_custom_target (libarcstk_create_include_dir
	DEPENDS "${PROJECT_BINARY_DIR}/include"
	VERBATIM
)

## Create build-tree include directory (symlink)
## This is useful for finding the includes when used as a subproject.
add_custom_target (libarcstk_link_to_headers
	COMMAND "${CMAKE_COMMAND}"
		-E create_symlink
		"${CMAKE_CURRENT_SOURCE_DIR}/include"
		"${PROJECT_INCLUDE_BINARY_DIR}"
	VERBATIM
)

add_dependencies (libarcstk_link_to_headers libarcstk_create_include_dir )



## --- Update version.cpp with info from git {{{1

find_package (Git QUIET REQUIRED )

## Run 'git describe'
execute_process (
	COMMAND "${GIT_EXECUTABLE}" describe --always HEAD
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
	RESULT_VARIABLE GIT_EXIT_CODE
	OUTPUT_VARIABLE GIT_VERSION_STRING
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

## Did running git work?
if (GIT_EXIT_CODE EQUAL 0 )
	message (STATUS "git describe --always HEAD: ${GIT_VERSION_STRING}" )
else()
	message (WARNING "Failed to run git describe --always HEAD" )
endif()

## Run 'git rev-parse'
execute_process (
	COMMAND "${GIT_EXECUTABLE}" rev-parse HEAD
	WORKING_DIRECTORY "${CMAKE_BINARY_DIR}"
	RESULT_VARIABLE GIT_EXIT_CODE
	OUTPUT_VARIABLE GIT_COMMIT_ID
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

## Did running git work?
if (GIT_EXIT_CODE EQUAL 0 )
	message (STATUS "git rev-parse HEAD: ${GIT_COMMIT_ID}" )
else()
	message (WARNING "Failed to run git rev-parse HEAD" )
endif()

## This presupposes having CMAKE_CONFIGURE_DEPENDS set to .git/index
configure_file ("${PROJECT_SOURCE_DIR}/version.cpp.in"
	"${PROJECT_GEN_SOURCES_DIR}/version.cpp"
	@ONLY )




## --- Define Project Target {{{1

## -- Specify Exported Headers {{{2

set (INTERFACE_HEADERS )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_SOURCE_DIR}/accuraterip.hpp" )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_SOURCE_DIR}/algorithms.hpp"  )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_SOURCE_DIR}/calculate.hpp"   )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_SOURCE_DIR}/checksum.hpp"    )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_SOURCE_DIR}/dbar.hpp"        )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_SOURCE_DIR}/identifier.hpp"  )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_SOURCE_DIR}/logging.hpp"     )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_SOURCE_DIR}/metadata.hpp"    )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_SOURCE_DIR}/policies.hpp"    )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_SOURCE_DIR}/samples.hpp"     )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_SOURCE_DIR}/verify.hpp"      )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_SOURCE_DIR}/version.hpp"     )


## -- Specify Sources {{{2

add_library (${PROJECT_NAME} SHARED
	"${PROJECT_SOURCE_DIR}/accuraterip.cpp"
	"${PROJECT_SOURCE_DIR}/calculate.cpp"
	"${PROJECT_SOURCE_DIR}/checksum.cpp"
	"${PROJECT_SOURCE_DIR}/dbar.cpp"
	"${PROJECT_SOURCE_DIR}/identifier.cpp"
	"${PROJECT_SOURCE_DIR}/logging.cpp"
	"${PROJECT_SOURCE_DIR}/metadata.cpp"
	"${PROJECT_SOURCE_DIR}/samples.cpp"
	"${PROJECT_SOURCE_DIR}/verify.cpp"
	"${PROJECT_GEN_SOURCES_DIR}/version.cpp" )


## -- Configure Target {{{2

get_directory_property (HAS_PARENT PARENT_DIRECTORY )

if (HAS_PARENT )
	## Make project's includes available to PARENT
	target_include_directories (${PROJECT_NAME}
		## public headers:
		INTERFACE $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
	)

	## Provide a namespaced target also when added by add_subdirectory
	add_library (${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME} )
endif()

target_include_directories (${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_INCLUDE_BINARY_DIR}>
		$<INSTALL_INTERFACE:${PROJECT_INCLUDE_INSTALL_SUBDIR}>
	PRIVATE
		"${PROJECT_GEN_SOURCES_DIR}" ## for generated sources
	)

set_target_properties (${PROJECT_NAME} PROPERTIES
	CXX_STANDARD   17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF ## use -std=c++17 instead of std=gnu++17
	PUBLIC_HEADER  "${INTERFACE_HEADERS}"
	PREFIX         "" ## No 'lib' prefix in soname, PROJECT_NAME is prefixed
	VERSION        "${PROJECT_VERSION_STRING_WITHOUT_SUFFIX}"
	SOVERSION      "${PROJECT_VERSION_STRING_WITHOUT_SUFFIX}" )

add_dependencies (${PROJECT_NAME} libarcstk_link_to_headers )



## --- Compiler Specific Settings {{{1

## -- Define Warning and Optimization Flags for g++ and clang++ {{{2

## g++: Warnings
set (PROJECT_CXX_GNU_FLAGS_WARNINGS
	-Wall -Wextra -Wpedantic -Weffc++
	-Wold-style-cast
	-Wsuggest-override
	-Wcast-align
	-Wcast-qual
	-Wctor-dtor-privacy
	-Wdisabled-optimization
	-Wformat=2
	-Wlogical-op
	-Wmissing-declarations
	-Wmissing-include-dirs
	-Wnoexcept
	-Woverloaded-virtual
	-Wredundant-decls
	-Wshadow
	-Wsign-conversion
	-Wsign-promo
	-Wstrict-null-sentinel
	-Wstrict-overflow=5
	-Wswitch-default
	-Wundef
	-Wuseless-cast
	-Wzero-as-null-pointer-constant
	)

## g++: Optimize flags
set (PROJECT_CXX_GNU_FLAGS_OPTIMIZE  -march=native -mtune=generic )


## clang++: Warnings
## Clang warnings are quite aggressive by default. Some of the g++ warnings are
## either compatibility flags that do nothing or are activated by other meta
## flags. Some are just on by default. The following list is intended to contain
## only those flags that are off by default and no compatibility flags.
set (PROJECT_CXX_CLANG_FLAGS_WARNINGS
	-Wall -Wextra -Wpedantic -Weffc++
	-Wold-style-cast
	-Wcast-align
	-Wcast-qual
	-Wformat=2
	-Wshadow
	-Wsign-conversion
	-Wundef
	-Wunused
	-Wzero-as-null-pointer-constant
	# -- clang++ specific --
	-Winconsistent-missing-destructor-override
	)

## clang++: Optimize flags
set (PROJECT_CXX_CLANG_FLAGS_OPTIMIZE -march=native -mtune=generic )


## -- Select Warning and Optimization Flags for Actual Compiler {{{2

if (CMAKE_COMPILER_IS_GNUCXX ) ## for g++

	message (STATUS "Select compile flags for g++" )

	set (PROJECT_CXX_FLAGS_WARNINGS ${PROJECT_CXX_GNU_FLAGS_WARNINGS} )
	set (PROJECT_CXX_FLAGS_OPTIMIZE ${PROJECT_CXX_GNU_FLAGS_OPTIMIZE} )

elseif (CMAKE_CXX_COMPILER MATCHES ".*clang"       ## for clang++
		OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )

	message (STATUS "Select compile flags for clang++" )

	set (PROJECT_CXX_FLAGS_WARNINGS ${PROJECT_CXX_CLANG_FLAGS_WARNINGS} )
	set (PROJECT_CXX_FLAGS_OPTIMIZE ${PROJECT_CXX_CLANG_FLAGS_OPTIMIZE} )

else()

	message (WARNING "C++ Compiler seems to be neither g++ nor clang++" )
	message (WARNING "No compile flags specified for the actual C++ compiler" )
	message (WARNING "Try a generic compile without project specific flags" )

endif()


## -- Activate Compiler Warnings on Library Sources  {{{2

## We do not use target_compile_options since those flags should not be
## applied to Catch2 and tests.
## TODO Find a way to use target_compile_options

get_target_property (PROJECT_SOURCES ${PROJECT_NAME} SOURCES )

foreach (_sourcefile ${PROJECT_SOURCES} )
	set_property (
		SOURCE "${_sourcefile}"
		APPEND
		PROPERTY COMPILE_OPTIONS ${PROJECT_CXX_FLAGS_WARNINGS}
	)
	#message (STATUS "${_sourcefile}: ${PROJECT_CXX_FLAGS_WARNINGS}" )
endforeach()


## -- Manage Optimization Flags  {{{2

## - Optional: Activate Optimization Option (default ON for Release only) {{{3

option (WITH_NATIVE "Use platform specific optimization on compiling" OFF )

## For 'Release', Set Default ON
if (CMAKE_BUILD_TYPE STREQUAL "Release")

	set (WITH_NATIVE ON CACHE BOOL
			"Release: Force platform specific optimization" FORCE )
endif ()

## - If Optimization Option Activated, Add Flags {{{3
if (WITH_NATIVE )

	message (STATUS "Build with platform specific optimization")

	target_compile_options (${PROJECT_NAME}
		PRIVATE ${PROJECT_CXX_FLAGS_OPTIMIZE} )
else()

	message (STATUS "Build without platform specific optimization" )

endif (WITH_NATIVE )

## - Status Message About Compile Flags {{{3

get_target_property (PROJECT_CXX_FLAGS ${PROJECT_NAME} COMPILE_OPTIONS )
message (STATUS "Compile flags: ${PROJECT_CXX_FLAGS}" )



## --- Installation {{{1

if (NOT SKIP_INSTALL_ALL )

	add_subdirectory (install)

endif ()



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



## --- Optional: Build documentation (default: OFF) {{{1

option (WITH_DOCS          "Build documentation for public API"           OFF )
option (WITH_INTERNAL_DOCS "Build documentation also for internal APIs"   OFF )
option (USE_DOC_TOOL       "Build documentation using specified tool"     OFF )
## TODO Remove the following line
option (USE_MCSS           "Build documentation using m.css"              OFF )


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

	set (WITH_DOCS ON CACHE BOOL "Build documentation for public API" FORCE )
	if (NOT WITH_DOCS )
		message (FATAL_ERROR
			"-DWITH_DOCS=ON is required to set -DWITH_INTERNAL_DOCS."
			" Auto-set it now, but this will be an error in future versions." )
	endif()
endif()


## -- For backwards compatibility
if (USE_MCSS ) # TODO Remove that

	if (NOT "MCSS" IN_LIST USE_DOC_TOOL )
		list (APPEND USE_DOC_TOOL "MCSS" )
	endif()
	unset (USE_MCSS )
	message (WARNING "Option USE_MCSS is deprecated and will be removed soon!"
		" Use -DUSE_DOC_TOOL=MCSS instead." )
endif()


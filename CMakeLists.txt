## Root CMake file for libarcstk

cmake_minimum_required (VERSION 3.9.6 )

## Set variables corresponding to unspecified versions to the empty string
## Required for specifying the project version along with the name in project()
cmake_policy (SET CMP0048 NEW )


project (libarcstk VERSION 0.1.0 LANGUAGES CXX )

get_directory_property (HAS_PARENT PARENT_DIRECTORY )


## --- Setup Version Information

set (PROJECT_BRIEF_DESC
	"Toolkit to calculate and verify AccurateRip checksums and ids" )

set (PROJECT_URL "https://github.com/crf8472/${PROJECT_NAME}" )

set (PROJECT_VERSION_SUFFIX "" )

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



## --- Setup Default Build Type

if (NOT CMAKE_BUILD_TYPE )

	set (CMAKE_BUILD_TYPE "${PROJECT_DEFAULT_BUILD_TYPE}"
		CACHE STRING "Choose the type of build (e.g. Debug or Release)" FORCE )

	message (STATUS "No build type specified, set to ${CMAKE_BUILD_TYPE}" )
endif (NOT CMAKE_BUILD_TYPE )

message (STATUS "Build type: ${CMAKE_BUILD_TYPE}" )



## --- Global settings

## Trigger configuration phase whenever git index changes
set_property (DIRECTORY APPEND
	PROPERTY CMAKE_CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/.git/index )


set (CMAKE_CXX_EXTENSIONS OFF ) ## use -std=c++14 instead of std=gnu++14


## Abort release-builds on big endian platforms

include (TestBigEndian)

TEST_BIG_ENDIAN (IS_BIG_ENDIAN )

if (IS_BIG_ENDIAN )

	if (CMAKE_BUILD_TYPE STREQUAL "Release" )

		message (FATAL_ERROR
			"Big endian platforms are not yet supported, use -DCMAKE_BUILD_TYPE=Debug to configure your build." )
	else ()

		message (WARNING "Big endian platform support is not yet available" )
	endif ()

endif()



## --- Project Directory Layout

## "Root directory for out-of-source build"
set (PROJECT_BINARY_DIR       "${CMAKE_CURRENT_BINARY_DIR}" )

## "Root directory for sources"
set (PROJECT_SOURCE_DIR       "${CMAKE_CURRENT_SOURCE_DIR}/src" )

## "Root directory for public/exported headers"
set (PROJECT_INCLUDE_DIR      "${PROJECT_BINARY_DIR}/include/arcstk" )

## "Target directory for non-cmake logs"
set (PROJECT_LOG_DIR          "${PROJECT_BINARY_DIR}/log" )

## "Target directory for generated sources and configuration"
set (PROJECT_BUILD_SOURCE_DIR "${PROJECT_BINARY_DIR}/generated-sources" )

## Subdir for installing includes
set (PROJECT_INSTALL_INCLUDES_SUBDIR "include/arcstk" )



## --- Create build-tree directories

## Log directory

add_custom_target (libarcstk_create_log_dir ALL
	COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_LOG_DIR} )

add_custom_target (libarcstk_create_include_dir
	COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJECT_BINARY_DIR}/include )

## include directory + link

## Create build-tree include directory (symlink)
## This is useful for finding the includes when used as a subproject.
add_custom_target (libarcstk_link_to_headers
	COMMAND ${CMAKE_COMMAND} -E create_symlink
	${CMAKE_CURRENT_SOURCE_DIR}/include ${PROJECT_INCLUDE_DIR} )

add_dependencies (libarcstk_link_to_headers libarcstk_create_include_dir )



## --- Update version.cpp with info from git

find_package (Git QUIET REQUIRED )

## Run 'git describe'
execute_process (
	COMMAND ${GIT_EXECUTABLE} describe --always HEAD
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
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
	COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
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
configure_file (${PROJECT_SOURCE_DIR}/version.cpp.in
	${PROJECT_BUILD_SOURCE_DIR}/version.cpp
	@ONLY )



## --- Specify Exported Headers

set (INTERFACE_HEADERS )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_DIR}/calculate.hpp" )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_DIR}/match.hpp" )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_DIR}/identifier.hpp" )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_DIR}/logging.hpp" )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_DIR}/parse.hpp" )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_DIR}/policies.hpp" )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_DIR}/samples.hpp" )
list (APPEND INTERFACE_HEADERS "${PROJECT_INCLUDE_DIR}/version.hpp" )

set (PRIVATE_HEADERS )
list (APPEND PRIVATE_HEADERS "${PROJECT_INCLUDE_DIR}/details/builders.tpp" )
list (APPEND PRIVATE_HEADERS "${PROJECT_INCLUDE_DIR}/details/validate.tpp" )


## --- Add and configure project target

add_library (${PROJECT_NAME} SHARED
	"${PROJECT_SOURCE_DIR}/calculate.cpp"
	"${PROJECT_SOURCE_DIR}/match.cpp"
	"${PROJECT_SOURCE_DIR}/identifier.cpp"
	"${PROJECT_SOURCE_DIR}/logging.cpp"
	"${PROJECT_SOURCE_DIR}/parse.cpp"
	"${PROJECT_SOURCE_DIR}/samples.cpp"
	"${PROJECT_BUILD_SOURCE_DIR}/version.cpp" )

if (HAS_PARENT )
	## Make project's includes available to PARENT
	target_include_directories (${PROJECT_NAME}
		INTERFACE $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>
	)

	## Provide a namespaced target also When added by add_subdirectory
	add_library (${PROJECT_NAME}::${PROJECT_NAME} ALIAS ${PROJECT_NAME} )
endif()

target_include_directories (${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${PROJECT_INCLUDE_DIR}>
		$<INSTALL_INTERFACE:${PROJECT_INSTALL_INCLUDES_SUBDIR}>
	PRIVATE
		"${PROJECT_BUILD_SOURCE_DIR}" )

set_target_properties (${PROJECT_NAME} PROPERTIES
	CXX_STANDARD   14
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF ## use -std=c++14 instead of std=gnu++14
	PUBLIC_HEADER  "${INTERFACE_HEADERS}"
	PRIVATE_HEADER "${PRIVATE_HEADERS}"
	PREFIX         "" ## No 'lib' prefix in soname, PROJECT_NAME is prefixed
	VERSION        "${PROJECT_VERSION_STRING_WITHOUT_SUFFIX}"
	SOVERSION      "${PROJECT_VERSION_STRING_WITHOUT_SUFFIX}" )

add_dependencies (${PROJECT_NAME} libarcstk_link_to_headers )


## --- Compiler Specific Settings

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
	# -- clang specific --
	-Winconsistent-missing-destructor-override
	)

## clang++: Optimize flags
set (PROJECT_CXX_CLANG_FLAGS_OPTIMIZE -march=native -mtune=generic )


## Compiler: apply specific flags

if (CMAKE_COMPILER_IS_GNUCXX ) ## for g++

	message (STATUS "Select compile flags for g++" )

	set (PROJECT_CXX_FLAGS_WARNINGS ${PROJECT_CXX_GNU_FLAGS_WARNINGS} )
	set (PROJECT_CXX_FLAGS_OPTIMIZE ${PROJECT_CXX_GNU_FLAGS_OPTIMIZE} )

elseif (CMAKE_CXX_COMPILER MATCHES ".*clang"       ## for clang
		OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

	message (STATUS "Select compile flags for clang++" )

	set (PROJECT_CXX_FLAGS_WARNINGS ${PROJECT_CXX_CLANG_FLAGS_WARNINGS} )
	set (PROJECT_CXX_FLAGS_OPTIMIZE ${PROJECT_CXX_CLANG_FLAGS_OPTIMIZE} )

else()

	message (WARNING "C++ Compiler seems to be neither g++ nor clang++" )
	message (WARNING "No compile flags specified for the actual C++ compiler" )
	message (WARNING "Try a generic compile without project specific flags" )

endif()


## Compiler: Activate Warnings on library sources
## We do not use target_compile_options since those flags should not be
## applied to Catch2 and tests.

get_target_property (PROJECT_SOURCES ${PROJECT_NAME} SOURCES )

foreach (_sourcefile ${PROJECT_SOURCES} )
	set_property (
		SOURCE ${_sourcefile}
		APPEND
		PROPERTY COMPILE_OPTIONS ${PROJECT_CXX_FLAGS_WARNINGS}
	)
	#message (STATUS "${_sourcefile}: ${PROJECT_CXX_FLAGS_WARNINGS}" )
endforeach()


## Compiler: Add optimization flags (always in the same way)

macro (add_cxx_flags_optimize )

	message (STATUS "Build with platform specific optimization")

	target_compile_options (${PROJECT_NAME}
			PRIVATE ${PROJECT_CXX_FLAGS_OPTIMIZE} )
endmacro()


## Compiler: Activate Optimizations (default ON for Release, otherwise OFF)

option (WITH_NATIVE "Use platform specific optimization on compiling" OFF )

if (CMAKE_BUILD_TYPE STREQUAL "Release")

	set (WITH_NATIVE ON CACHE BOOL
			"Release: Force platform specific optimization" FORCE )
endif ()

if (WITH_NATIVE )

	add_cxx_flags_optimize()
else()

	message (STATUS "Build without platform specific optimization")

endif (WITH_NATIVE )

get_target_property (PROJECT_CXX_FLAGS ${PROJECT_NAME} COMPILE_OPTIONS )

message (STATUS "Compile flags: ${PROJECT_CXX_FLAGS}" )



## --- Install binary and exported header files

if (NOT SKIP_INSTALL_ALL )

	set (INSTALL_CMAKECONFIG_DIR
		"${CMAKE_INSTALL_PREFIX}/lib/cmake/${PROJECT_NAME}"
		CACHE PATH "Installation directory for cmake package files" )

	set (PROJECT_EXPORT_SET "${PROJECT_NAME}-targets" )

	install (TARGETS ${PROJECT_NAME}
		EXPORT "${PROJECT_EXPORT_SET}"
		LIBRARY       DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
		PUBLIC_HEADER DESTINATION
				"${CMAKE_INSTALL_PREFIX}/${PROJECT_INSTALL_INCLUDES_SUBDIR}"
		PRIVATE_HEADER DESTINATION
				"${CMAKE_INSTALL_PREFIX}/${PROJECT_INSTALL_INCLUDES_SUBDIR}/details" )

	install (EXPORT ${PROJECT_EXPORT_SET}
		NAMESPACE libarcstk::
		DESTINATION "${INSTALL_CMAKECONFIG_DIR}" )

	if (NOT SKIP_INSTALL_FILES )

		## Support pkgconfig

		configure_file (
			"${CMAKE_CURRENT_SOURCE_DIR}/libarcstk.pc.in"
			"${PROJECT_BUILD_SOURCE_DIR}/libarcstk.pc"
			@ONLY
		)

		install (FILES "${PROJECT_BUILD_SOURCE_DIR}/libarcstk.pc"
			DESTINATION "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig" )

		## Support CMake Packaging

		include (CMakePackageConfigHelpers )

		set (PROJECT_CONFIG_FILE "${PROJECT_NAME}-config.cmake" )

		set (PROJECT_CONFIG_VERSION_FILE "${PROJECT_NAME}-config-version.cmake")

		configure_package_config_file (
			"${CMAKE_CURRENT_SOURCE_DIR}/${PROJECT_CONFIG_FILE}.in"
			"${PROJECT_BUILD_SOURCE_DIR}/${PROJECT_CONFIG_FILE}"
			INSTALL_DESTINATION "${INSTALL_CMAKECONFIG_DIR}"
		)

		write_basic_package_version_file (
			"${PROJECT_BUILD_SOURCE_DIR}/${PROJECT_CONFIG_VERSION_FILE}"
			VERSION        ${PROJECT_VERSION_STRING_WITHOUT_SUFFIX}
			COMPATIBILITY  SameMajorVersion
		)

		install (FILES
			"${PROJECT_BUILD_SOURCE_DIR}/${PROJECT_CONFIG_FILE}"
			"${PROJECT_BUILD_SOURCE_DIR}/${PROJECT_CONFIG_VERSION_FILE}"
			DESTINATION "${INSTALL_CMAKECONFIG_DIR}"
		)

	endif ()

endif (NOT SKIP_INSTALL_ALL)



## --- Optional: Build tests, using Catch2 testing framework (default: OFF)

option (WITH_TESTS "Build unit tests" OFF )

if (WITH_TESTS)

	message (STATUS "Build with tests" )

	enable_testing()

	## Add Catch2 test framework
	add_subdirectory (thirdparty/catch2 EXCLUDE_FROM_ALL )

	## Add tests
	add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/test )

endif (WITH_TESTS)


## --- Optional: Build documentation (default: OFF)

option (WITH_DOCS          "Build documentation for public API"           OFF )
option (WITH_INTERNAL_DOCS "Build documentation also for internal APIs"   OFF )
option (USE_MCSS           "Build documentation using m.css"              OFF )

## Auto-activate WITH_DOCS if settings for documentation are present
if (WITH_INTERNAL_DOCS OR USE_MCSS )

	set (WITH_DOCS ON CACHE BOOL "Build documentation for public API" FORCE )
endif()

## Process configuration for building the documentation
if (WITH_DOCS )

	add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/doc )
endif (WITH_DOCS )



## --- Optional: Add include-what-you-use support (default: OFF)

option (IWYU "Don't compile, check headers with include-what-you-use" OFF )
if (IWYU )

	message (STATUS "Program include-what-you-use requested" )
	find_program (IWYU_BINARY NAMES include-what-you-use iwyu )

	if (IWYU_BINARY )

		## Set for all subsequent targets
		set (CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_BINARY} )

		message (STATUS "Program include-what-you-use found: ${IWYU_BINARY}" )

	else (IWYU_BINARY )

		message (FATAL_ERROR "Program include-what-you-use not found" )

	endif (IWYU_BINARY )
endif (IWYU )


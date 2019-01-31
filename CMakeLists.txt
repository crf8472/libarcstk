## Root CMake file for libarcs

cmake_minimum_required (VERSION 3.9.6 )

## Set variables corresponding to unspecified versions to the empty string
## needed for specifying the project version along with the name in project()
cmake_policy (SET CMP0048 NEW)

## Support new IN_LIST if() operator
## needed for FindDoxygen.cmake
cmake_policy (SET CMP0057 NEW)


project (arcs VERSION 0.1.0 LANGUAGES CXX )

set (PROJECT_VERSION_SUFFIX "-alpha.1" )

set (PROJECT_DEFAULT_BUILD_TYPE "Release" )


## --- Setup Version Information

set (PROJECT_VERSION_STRING_WITHOUT_SUFFIX
	"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
	)

set (PROJECT_VERSION_STRING "${PROJECT_VERSION_STRING_WITHOUT_SUFFIX}" )

if (PROJECT_VERSION_SUFFIX)
	set(PROJECT_VERSION_STRING
		"${PROJECT_VERSION_STRING}${PROJECT_VERSION_SUFFIX}" )
endif(PROJECT_VERSION_SUFFIX)

message (STATUS
"Building lib${PROJECT_NAME} version ${PROJECT_VERSION_STRING_WITHOUT_SUFFIX}" )


## --- Setup Default Build Type

if (NOT CMAKE_BUILD_TYPE)

	set (CMAKE_BUILD_TYPE "${PROJECT_DEFAULT_BUILD_TYPE}"
		CACHE STRING "Choose the type of build (e.g. Debug or Release)" FORCE )

	message (WARNING "No build type specified, set to: ${CMAKE_BUILD_TYPE}" )

endif (NOT CMAKE_BUILD_TYPE)

message (STATUS "Build type: ${CMAKE_BUILD_TYPE}" )


## --- Abort releases on BigEndian platforms

include (TestBigEndian)

TEST_BIG_ENDIAN(IS_BIG_ENDIAN)

if (IS_BIG_ENDIAN )

	if (CMAKE_BUILD_TYPE STREQUAL "Release" )

		message (FATAL_ERROR
			"Big endian platforms are not yet supported, use -DCMAKE_BUILD_TYPE=Debug to configure your build." )
	else ()

		message (WARNING
			"Big endian platform support is not yet available" )
	endif ()

endif()


## --- Project Directory Layout

set (PROJECT_BINARY_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/build" )
set (PROJECT_SOURCE_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/src" )
set (PROJECT_TEST_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/test" )
set (PROJECT_DOC_DIR     "${PROJECT_BINARY_DIR}/doc" )

set (INSTALL_LIBRARY_DIR   "${CMAKE_INSTALL_PREFIX}/lib"
	CACHE PATH "Installation directory for library (.so) files" )

set (INSTALL_INCLUDE_DIR   "${CMAKE_INSTALL_PREFIX}/include"
	CACHE PATH "Installation directory for include (.hpp) files" )

set (INSTALL_PKGCONFIG_DIR "${CMAKE_INSTALL_PREFIX}/lib/pkgconfig"
	CACHE PATH "Installation directory for pkgconfig (.pc) files" )

## Commented out: find project's cmake modules
#set (CMAKE_MODULE_PATH     "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" )
#message (STATUS "Changed CMAKE_MODULE_PATH to: ${CMAKE_MODULE_PATH}" )


## --- Add Sources

list (APPEND SOURCES "${PROJECT_SOURCE_DIR}/calculate.cpp" )
list (APPEND SOURCES "${PROJECT_SOURCE_DIR}/checksum.cpp" )
list (APPEND SOURCES "${PROJECT_SOURCE_DIR}/match.cpp" )
list (APPEND SOURCES "${PROJECT_SOURCE_DIR}/identifier.cpp" )
list (APPEND SOURCES "${PROJECT_SOURCE_DIR}/logging.cpp" )
list (APPEND SOURCES "${PROJECT_SOURCE_DIR}/parse.cpp" )


## --- Optional: Add include-what-you-use support (default: OFF)

if (IWYU)

	message (STATUS "Program include-what-you-use requested" )
	find_program (IWYU_PATH NAMES include-what-you-use iwyu )

	if (IWYU_PATH)

		## Set for all subsequent targets
		set (CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH} )

		## Commented out: Set for specific targets only
		#set_property(TARGET ${TARGET_NAME}
		#	PROPERTY CMAKE_CXX_INCLUDE_WHAT_YOU_USE ${IWYU_PATH})

		message (STATUS "Program include-what-you-use found: ${IWYU_PATH}" )

	else (IWYU_PATH)

		message (WARNING "Program include-what-you-use not found" )
	endif (IWYU_PATH)
endif (IWYU)


## --- Files configured by CMake

## Configure all .in files producing C++ headers files

set (CONFIGURED_HEADERS
	version.hpp )

foreach (_header IN LISTS CONFIGURED_HEADERS )

	configure_file (
	  "${PROJECT_SOURCE_DIR}/${_header}.in"
	  "${PROJECT_BINARY_DIR}/${_header}"
	  @ONLY
	)
endforeach()

## Configure .pc file for pkg-info

configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/libarcs.pc.in"
  "${PROJECT_BINARY_DIR}/libarcs.pc"
  @ONLY
)


## --- Exported headers

set (EXPORTED_HEADERS )
list (APPEND EXPORTED_HEADERS "${PROJECT_SOURCE_DIR}/calculate.hpp" )
list (APPEND EXPORTED_HEADERS "${PROJECT_SOURCE_DIR}/checksum.hpp" )
list (APPEND EXPORTED_HEADERS "${PROJECT_SOURCE_DIR}/match.hpp" )
list (APPEND EXPORTED_HEADERS "${PROJECT_SOURCE_DIR}/identifier.hpp" )
list (APPEND EXPORTED_HEADERS "${PROJECT_SOURCE_DIR}/logging.hpp" )
list (APPEND EXPORTED_HEADERS "${PROJECT_SOURCE_DIR}/parse.hpp" )

list (APPEND EXPORTED_HEADERS "${PROJECT_SOURCE_DIR}/checksumlist.tpp" )
list (APPEND EXPORTED_HEADERS "${PROJECT_SOURCE_DIR}/sampleblockiterator.tpp" )

## --- Add and configure project target

## Build libarcs shared library as project target

option (AS_STATIC "Build as static library" OFF )

if (AS_STATIC)

	message (STATUS "Build as static library" )
	add_library (${PROJECT_NAME} STATIC ${SOURCES} )
else (AS_STATIC)

	message (STATUS "Build as shared library" )
	add_library (${PROJECT_NAME} SHARED ${SOURCES} )
endif (AS_STATIC)

## Include generated sources
## (required to find version.hpp and the other configured headers)

target_include_directories (${PROJECT_NAME} PRIVATE ${PROJECT_BINARY_DIR} )

## Configure properties for project target

set_target_properties (${PROJECT_NAME} PROPERTIES
	CXX_STANDARD  14
	PUBLIC_HEADER "${EXPORTED_HEADERS}"
	VERSION       "${PROJECT_VERSION_STRING_WITHOUT_SUFFIX}"
	SOVERSION     "${PROJECT_VERSION_STRING_WITHOUT_SUFFIX}" )


## --- Install binary and exported header files

if (NOT SKIP_INSTALL_ALL)

	if (AS_STATIC)

		## install static library
		install (TARGETS ${PROJECT_NAME}
			LIBRARY        DESTINATION  "${INSTALL_LIBRARY_DIR}"
			ARCHIVE        DESTINATION  "${INSTALL_LIBRARY_DIR}"
			PUBLIC_HEADER  DESTINATION  "${INSTALL_INCLUDE_DIR}/arcs" )
	else (AS_STATIC)

		## install shared library
		install (TARGETS ${PROJECT_NAME}
			LIBRARY        DESTINATION  "${INSTALL_LIBRARY_DIR}"
			PUBLIC_HEADER  DESTINATION  "${INSTALL_INCLUDE_DIR}/arcs" )
	endif (AS_STATIC)

	if (NOT SKIP_INSTALL_FILES)

		install (FILES "${PROJECT_BINARY_DIR}/libarcs.pc"
			DESTINATION "${INSTALL_PKGCONFIG_DIR}" )
	endif()

endif (NOT SKIP_INSTALL_ALL)


## --- Compiler Specific Settings

option (WITH_NATIVE "Use platform specific optimization on compiling" OFF )

## Settings for g++ and clang

if (CMAKE_COMPILER_IS_GNUCXX
		OR CMAKE_CXX_COMPILER MATCHES ".*clang"
		OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")

	## Activate Warnings

	add_compile_options (-Wall -Wextra -Weffc++ -Wold-style-cast -Wpedantic )

	## Platform specific optimizations (default ON for Release, otherwise OFF)

	if (CMAKE_BUILD_TYPE STREQUAL "Release")

		set (WITH_NATIVE ON CACHE BOOL
			"Build with platform specific optimization" FORCE )

	endif (CMAKE_BUILD_TYPE STREQUAL "Release" )

	if (WITH_NATIVE )

		message (STATUS "Build with platform specific optimization")

		add_compile_options (-march=native -mtune=native )
	else()

		message (STATUS "Build without platform specific optimization")

	endif (WITH_NATIVE )

else()

	message (WARNING "No compile flags specified for the actual C++ compiler")
	message (WARNING "Try a generic compile without project specific flags")

endif()


## --- Optional: Build tests, using Catch2 testing framework

option (WITH_TESTS "Build unit tests" OFF )

if (WITH_TESTS)

	message (STATUS "Build with tests" )

	enable_testing()

	## Add Catch2 test framework
	add_subdirectory (thirdparty/catch2 EXCLUDE_FROM_ALL )

	## Add tests
	add_subdirectory (${PROJECT_TEST_DIR} )

else ()

	message (STATUS "Build without tests" )
endif (WITH_TESTS)


## --- Generate documentation

option (WITH_INTERNAL_DOCS "Build documentation also for internal APIs"   OFF )
option (WITH_IMPLONLY_DOCS "Build documentation also for implementations" OFF )
option (USE_MCSS           "Build documentation using m.css" OFF )

find_package (Doxygen 1.8.0 COMPONENTS REQUIRED dot )

if (DOXYGEN_FOUND)

	message (STATUS
		"Note: to build documentation do 'cmake --build . --target doc'" )

	set(DOC_COMMENT "Build documentation for APIs" )


	## Build internal docs if requested

	if (WITH_INTERNAL_DOCS)

		message (STATUS "Will build documentation also for internal APIs" )
		set (PROJECT_DOXYGEN_INTERNAL_DOCS "YES" )

		set(DOC_COMMENT "${DOC_COMMENT} (also internal APIs)" )
	else ()
		set (PROJECT_DOXYGEN_INTERNAL_DOCS "NO" )
	endif (WITH_INTERNAL_DOCS)


	## Build impl-only docs if requested (includes internal docs)

	if (WITH_IMPLONLY_DOCS)

		if (NOT WITH_INTERNAL_DOCS)

			## print message that has not been printed so far
			message (STATUS "Will build documentation also for internal APIs" )

		endif (NOT WITH_INTERNAL_DOCS)

		set (PROJECT_DOXYGEN_INTERNAL_DOCS "YES" )

		set(DOC_COMMENT "${DOC_COMMENT} and implementations" )

		message (STATUS "Will build documentation also for implementations" )

		set (PROJECT_DOXYGEN_ENABLED_SECTIONS "IMPL_ONLY" )

	else ()
		set (PROJECT_DOXYGEN_ENABLED_SECTIONS "" )
	endif (WITH_IMPLONLY_DOCS)


	## Create Doxyfile

	configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in
		${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
		@ONLY
	)


	## Use the fabulous m.css HTML5/CSS3 doxygen design from m.css

	if (USE_MCSS )

		if (NOT WIN32 )

			message (STATUS "Will build documentation with m.css" )

			set(DOC_COMMENT "${DOC_COMMENT}, using m.css" )

			add_subdirectory (thirdparty/m.css )

			add_subdirectory (thirdparty/python-virtualenv )

			configure_file (${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile-mcss.in
				${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-mcss
				@ONLY
			)

			## Run dox2html5.py

			message (STATUS "Sanbox directory: ${ENV_DIRECTORY}" )
			message (STATUS "Use python: ${PYTHON_CMD}" )
			message (STATUS "Use dox2html5: ${MCSS_DOX2HTML5}" )

			add_custom_target (doc
				COMMAND ${PYTHON_CMD} ${MCSS_DOX2HTML5} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-mcss
				DEPENDS ${ENV_TARGET}
				WORKING_DIRECTORY ${ENV_DIRECTORY}
				COMMENT ${DOC_COMMENT} VERBATIM
			)

		else (NOT WIN32 )

			message (WARNING "USE_MCSS is currently not available for Windows" )

		endif (NOT WIN32 )
	else()

		## Run doxygen

		add_custom_target (doc
			Doxygen::doxygen ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
			WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
			COMMENT ${DOC_COMMENT} VERBATIM
		)

	endif (USE_MCSS )

	## Commented out: example how to add target 'doc' to target 'all':
    ## add_custom_target (doc ALL ${DOXYGEN_EXECUTABLE} ... )

else ()

	message (STATUS "Doxygen not found. API documentation cannot be generated" )
endif (DOXYGEN_FOUND)

## Unset all cached option values to have their default values active
## on the next invokation of CMake

unset (AS_STATIC CACHE)
unset (IWYU CACHE)
unset (WITH_TESTS CACHE)
unset (WITH_INTERNAL_DOCS CACHE)
unset (WITH_IMPLONLY_DOCS CACHE)
unset (WITH_NATIVE CACHE)


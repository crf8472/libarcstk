## Build example application 'dbarparse'
## vim: fdm=marker

set (PROJECT_BUILD_DIR "${PROJECT_ROOT_DIR}/build" )

add_executable ( dbarparse dbarparse.cpp )

set_target_properties ( dbarparse PROPERTIES
	CXX_STANDARD          17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS        OFF
	BUILD_RPATH           "${CMAKE_BINARY_DIR}"
)

target_compile_options ( dbarparse PRIVATE -Wall -Wextra -pedantic )

## Always link to the libarcstk headers in the build/ dir - if another header
## is present, ignore it.
target_include_directories ( dbarparse
	PRIVATE $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/arcstk> )

## Always link to the libarcstk binary in the build/ dir - if another binary
## is present, ignore it.
target_link_libraries ( dbarparse PRIVATE libarcstk )

add_dependencies ( dbarparse libarcstk )


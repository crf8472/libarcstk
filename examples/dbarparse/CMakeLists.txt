## Build example application 'dbarparse'
## vim: fdm=marker

cmake_minimum_required (VERSION 3.10 )

add_executable (libarcstk_example_dbarparse EXCLUDE_FROM_ALL dbarparse.cpp )
add_dependencies (libarcstk_examples libarcstk_example_dbarparse )

set_target_properties (libarcstk_example_dbarparse PROPERTIES
	CXX_STANDARD          17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS        OFF
	OUTPUT_NAME           dbarparse
	BUILD_RPATH           "${LIBARCSTK_BINARY_DIR}"
	BUILD_WITH_INSTALL_RPATH    FALSE
	INSTALL_RPATH_USE_LINK_PATH TRUE
)

target_compile_options (libarcstk_example_dbarparse
	PRIVATE -Wall -Wextra -pedantic )

## Always link to the libarcstk headers in the build/ dir - if another header
## is present, ignore it.
target_include_directories (libarcstk_example_dbarparse
	PRIVATE $<BUILD_INTERFACE:${LIBARCSTK_BINARY_DIR}/include/arcstk> )

## Always link to the libarcstk binary in the build/ dir - if another binary
## is present, ignore it.
target_link_libraries (libarcstk_example_dbarparse PRIVATE libarcstk )


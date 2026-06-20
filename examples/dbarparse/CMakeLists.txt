## Build example application 'dbarparse'
## vim: fdm=marker

add_executable ( libarcstk_dbarparse dbarparse.cpp )

set_target_properties ( libarcstk_dbarparse PROPERTIES
	CXX_STANDARD          17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS        OFF
	BUILD_RPATH           "${LIBARCSTK_BINARY_DIR}"
)

target_compile_options ( libarcstk_dbarparse PRIVATE -Wall -Wextra -pedantic )

## Always link to the libarcstk headers in the build/ dir - if another header
## is present, ignore it.
target_include_directories ( libarcstk_dbarparse
	PRIVATE $<BUILD_INTERFACE:${LIBARCSTK_BINARY_DIR}/include/arcstk> )

## Always link to the libarcstk binary in the build/ dir - if another binary
## is present, ignore it.
target_link_libraries ( libarcstk_dbarparse PRIVATE libarcstk )


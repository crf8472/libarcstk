## Build example application 'albumverify'
## vim: fdm=marker

add_executable ( albumverify
	albumverify.cpp )

set_target_properties ( albumverify PROPERTIES
	CXX_STANDARD          17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS        OFF
	BUILD_RPATH           "${CMAKE_BINARY_DIR}"
)

target_compile_options ( albumverify PRIVATE -Wall -Wextra -pedantic )

## Always link to the libarcstk headers in the build/ dir - if another header
## is present, ignore it.
target_include_directories ( albumverify
	PRIVATE $<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/include/arcstk> )

## Always link to the libarcstk binary in the build/ dir - if another binary
## is present, ignore it.
target_link_libraries ( albumverify PRIVATE libarcstk )

add_dependencies ( albumverify libarcstk )


## Build example application 'albumverify'
## vim: fdm=marker

set (PROJECT_BUILD_DIR "${PROJECT_ROOT_DIR}/build" )

add_executable ( albumverify
	albumverify.cpp )

set_target_properties ( albumverify PROPERTIES
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS OFF )

target_compile_options ( albumverify PRIVATE -Wall -Wextra -pedantic )

## Always link to the libarcstk headers in the build/ dir - if another header
## is present, ignore it.
target_include_directories ( albumverify
	PRIVATE $<BUILD_INTERFACE:${PROJECT_BUILD_DIR}/arcstk> )

## Always link to the libarcstk binary in the build/ dir - if another binary
## is present, ignore it.
target_link_libraries ( albumverify
	PRIVATE "-Wl,--disable-new-dtags,-rpath,${PROJECT_BUILD_DIR}/" libarcstk )

add_dependencies ( albumverify libarcstk )


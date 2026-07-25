## CMake build script for configuring and building unit tests
##
## vim:fdm=marker

cmake_minimum_required (VERSION 3.10 )

add_test_suite (libarcstk_unit
	LABEL "unit"
	TIMEOUT 10 )

target_include_directories (libarcstk_unit_tests
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


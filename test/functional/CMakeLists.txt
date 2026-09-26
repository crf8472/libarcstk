## CMake build script for configuring and building functional tests
##
## vim:fdm=marker

cmake_minimum_required (VERSION 3.10 )

add_test_suite (libarcstk_functional
	LABEL "functional"
	TIMEOUT 10 )

target_include_directories (libarcstk_functional_tests
	PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"
)


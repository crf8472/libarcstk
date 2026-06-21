## CMake build script for configuring and building functional tests
##
## vim:fdm=marker

cmake_minimum_required (VERSION 3.10 )

add_test_suite (libarcstk_functional
	LABEL "functional"
	TIMEOUT 10 )


## CMake build script for configuring and building unit tests
##
## vim:fdm=marker

cmake_minimum_required (VERSION 3.10 )

add_test_suite (unit
	LABEL "unit"
	TIMEOUT 10 )


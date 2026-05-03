## CMake build script for configuring and building type traits tests
##
## vim:fdm=marker

cmake_minimum_required (VERSION 3.10 )

add_test_suite (type_traits
	LABEL "type_traits"
	TIMEOUT 10 )


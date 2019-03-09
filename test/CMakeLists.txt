## CMake file for configuring and running tests

cmake_minimum_required (VERSION 3.7 )


## Global settings

set (TEST_SOURCE_DIR "src" )
set (MAIN_TEST_CPP "${TEST_SOURCE_DIR}/main.test.cpp" )


## Define testsuites

set (TEST_SETS )

list (APPEND TEST_SETS calculate)

list (APPEND TEST_SETS calculate_calculation )
list (APPEND TEST_SETS calculate_calccontext )

list (APPEND TEST_SETS checksum_types )
list (APPEND TEST_SETS checksum_checksumlist )

list (APPEND TEST_SETS identifier_arid )
list (APPEND TEST_SETS identifier_toc )

list (APPEND TEST_SETS match )

list (APPEND TEST_SETS parse )

list (APPEND TEST_SETS samples )

set (TEST_EXE_NAMES )


## Add common configuration for all testsuites (catch2 + project dependencies)

foreach (_testcase ${TEST_SETS} )

	## Combine main and .cpp file to a single testsuite
	add_executable (${_testcase}_test
		${MAIN_TEST_CPP}
		"${TEST_SOURCE_DIR}/${_testcase}.cpp" )

	## Compile each test as C++14
	set_property(TARGET ${_testcase}_test PROPERTY CXX_STANDARD 14 )

	## For Catch2 (config must have been loaded before)
	target_link_libraries(${_testcase}_test Catch2::Catch2 )

	## For the project binaries
	target_link_libraries(${_testcase}_test ${PROJECT_NAME} )

	## For the project headers
	target_include_directories(${_testcase}_test PRIVATE ${PROJECT_SOURCE_DIR} )

	## For version.hpp and other CMake controlled header files
	target_include_directories(${_testcase}_test PRIVATE ${PROJECT_BINARY_DIR} )

	## Add test to ctest set
	add_test (
		NAME    ${_testcase}_test
		COMMAND ${_testcase}_test
			-o "${PROJECT_BINARY_DIR}/report.${_testcase}.xml"
			-r junit
		WORKING_DIRECTORY "${PROJECT_TEST_DIR}/data"
	)

	## Add the name of the test exe to lis
	list (APPEND TEST_EXE_NAMES ${_testcase}_test)

endforeach()


## Add each test executable to target 'check' so target 'check' rebuilds test
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
	DEPENDS ${TEST_EXE_NAMES} )


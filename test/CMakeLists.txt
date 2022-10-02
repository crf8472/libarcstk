## CMake file for configuring and running tests

cmake_minimum_required (VERSION 3.7 )


## Global settings

set (TEST_SOURCE_DIR "src" )
set (MAIN_TEST_CPP   "${TEST_SOURCE_DIR}/main.test.cpp" )


## Define testsuites

set (TEST_SETS )
list (APPEND TEST_SETS calculate)
list (APPEND TEST_SETS identifier )
list (APPEND TEST_SETS match )
list (APPEND TEST_SETS parse )
list (APPEND TEST_SETS samples )
list (APPEND TEST_SETS version )


## Adjust compile options for tests.
## (g++ issues many warnings when compiling Catch2 v3.x tests)

set (TEST_CXX_FLAGS_WARNINGS ${PROJECT_CXX_FLAGS_WARNINGS} )


## Remove flags from g++ that pollute the output

if (CMAKE_COMPILER_IS_GNUCXX )

	## Remove some flags from the actual compile options, if they are present
	list (APPEND GNUCXX_IGNORE_FLAGS
		-Weffc++
		-Wctor-dtor-privacy
		-Wuseless-cast
	)

	foreach (_flag ${GNUCXX_IGNORE_FLAGS} )
		message (STATUS
			"Ignore compile flag ${_flag} when compiling Catch2-tests with g++" )
		list (REMOVE_ITEM TEST_CXX_FLAGS_WARNINGS ${_flag} )
	endforeach()
endif()


## Provide main object file

add_library (catch2_main OBJECT "${MAIN_TEST_CPP}" )
set_property (TARGET catch2_main PROPERTY CXX_STANDARD 14 )

if (${CMAKE_VERSION} VERSION_LESS "3.13.0" )

	target_include_directories (catch2_main PUBLIC
	"${CMAKE_BINARY_DIR}/thirdparty/catch2/catch2-prefix/src/catch2/single_include/" )
else ()

	target_link_libraries (catch2_main PRIVATE Catch2::Catch2 )
endif ()

target_compile_options (catch2_main PUBLIC ${TEST_CXX_FLAGS_WARNINGS} )
target_compile_options (catch2_main PUBLIC ${PROJECT_CXX_FLAGS_OPTIMIZE} )

#get_target_property(COMP_FLAGS_MAIN catch2_main COMPILE_OPTIONS)
#message (STATUS "COMPILE_OPTIONS for catch2_main: ${COMP_FLAGS_MAIN}")


## Add common configuration for all testsuites (catch2 + project dependencies)

foreach (_testcase ${TEST_SETS} )

	set_property (
		SOURCE "${TEST_SOURCE_DIR}/${_testcase}.cpp"
		APPEND
		PROPERTY COMPILE_OPTIONS ${TEST_CXX_FLAGS_WARNINGS}
	)

	#get_property(COMP_FLAGS SOURCE "${TEST_SOURCE_DIR}/${_testcase}.cpp"
	#	PROPERTY COMPILE_OPTIONS )
	#message (STATUS "COMPILE_OPTIONS for ${_testcase}.cpp: ${COMP_FLAGS}")

	## Define an executable for each single testfile
	add_executable (${_testcase}_test
		$<TARGET_OBJECTS:catch2_main>
		"${TEST_SOURCE_DIR}/${_testcase}.cpp" )

	set_property (TARGET ${_testcase}_test PROPERTY CXX_STANDARD 14 )

	target_include_directories (${_testcase}_test
		PRIVATE ${PROJECT_INCLUDE_DIR}        ## public headers
		PRIVATE ${PROJECT_SOURCE_DIR}         ## private headers (*_details.hpp)
		PRIVATE ${PROJECT_BUILD_SOURCE_DIR} ) ## generated sources

	target_link_libraries (${_testcase}_test
		PRIVATE
		Catch2::Catch2WithMain ## Catch2
		${PROJECT_NAME} ) ## compiled libarcstk lib

	target_compile_options (${_testcase}_test
		PUBLIC ${PROJECT_CXX_FLAGS_OPTIMIZE} )

	## Add test to ctest set
	add_test (
		NAME    ${_testcase}_test
		COMMAND ${_testcase}_test
			-o "${PROJECT_BINARY_DIR}/report.${_testcase}.xml"
			-r junit
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/data"
	)

endforeach()


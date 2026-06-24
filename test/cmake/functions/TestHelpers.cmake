## libarcstk: CMake build script for configuring and building tests
## vim:fdm=marker

## Make existing list of flags compliant for libarcstk tests
function (_compile_flags_TEST OUT_VAR )

	cmake_parse_arguments (CFLAGS "" "" "FLAGS" ${ARGN} )

	set (_result "${CFLAGS_FLAGS}" )

	## --- g++: Remove flags that pollute the output with false positives
	if (CMAKE_COMPILER_IS_GNUCXX )

		list (REMOVE_ITEM _result -Weffc++ )
		list (REMOVE_ITEM _result -Wctor-dtor-privacy )
		list (REMOVE_ITEM _result -Wuseless-cast )
	endif()

	set (${OUT_VAR} "${_result}" PARENT_SCOPE )
endfunction ()

##
function (compile_flags OUT_VAR )

	set (options TESTCASE )
	set (multiValueArgs CURRENT )

	cmake_parse_arguments (CFLAGS "${options}" "" "${multiValueArgs}" ${ARGN} )

	##

	if (CFLAGS_TESTCASE AND CFLAGS_CURRENT )

		set (_actual_cxx_flags "${CFLAGS_CURRENT}" )

		#_compile_flags_TEST (_actual_cxx_flags FLAGS "${_actual_cxx_flags}" )
		_compile_flags_TEST (_actual_cxx_flags FLAGS ${_actual_cxx_flags} )

		set (${OUT_VAR} ${_actual_cxx_flags} PARENT_SCOPE )
		return ()
	endif()

endfunction ()

## Create a test executable from all .cpp files in a directory
## and register tests found with catch_discover_tests.
##
## Usage:
##   add_test_suite(unit
##       LABEL "unit"
##       TIMEOUT 10
##   )
##
## This produces a target called "unit_tests".
function (add_test_suite CATEGORY )

	set (options )
	set (oneValueArgs LABEL TIMEOUT )
	set (multiValueArgs )

	cmake_parse_arguments (SUITE
		"${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN} )

	## Collect all test source files in src/ directory
	file (GLOB TEST_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp" )

	if (NOT TEST_SOURCES )
		message (FATAL_ERROR
			"No test sources found in ${CMAKE_CURRENT_SOURCE_DIR}/src/" )
	endif()

	message (STATUS "Found ${CATEGORY} tests: ${TEST_SOURCES}" )

	## Create executable
	add_executable (${CATEGORY}_tests ${TEST_SOURCES} )

	## Standard configuration
	set_target_properties (${CATEGORY}_tests PROPERTIES
		CXX_STANDARD           17
		CXX_STANDARD_REQUIRED  ON
		BUILD_RPATH            "$<TARGET_FILE_DIR:${PROJECT_NAME}>"
		BUILD_RPATH_USE_ORIGIN ON
		SKIP_RPATH             OFF
	)

	## Use flags from main target as a starting point
	get_target_property (TEST_CXX_FLAGS ${PROJECT_NAME} COMPILE_OPTIONS )

	## Generate flag set for tests from flag set of main target
	compile_flags (TEST_CXX_FLAGS TESTCASE CURRENT ${TEST_CXX_FLAGS} )

	target_compile_options (${CATEGORY}_tests PRIVATE ${TEST_CXX_FLAGS} )

	get_target_property (${CATEGORY}_tests_CXX_FLAGS ${CATEGORY}_tests
		COMPILE_OPTIONS )
	message (STATUS "Compile flags for ${CATEGORY}_tests: "
		"${${CATEGORY}_tests_CXX_FLAGS}" )

	## Include paths
	target_include_directories (${CATEGORY}_tests
		PRIVATE "${LIBARCSTK_INCLUDE_SOURCE_DIR}" ## public headers
		PRIVATE "${LIBARCSTK_SOURCE_DIR}"         ## private headers
		PRIVATE "${LIBARCSTK_GENSRC_BINARY_DIR}"  ## generated sources
	)

	## Link libraries
	target_link_libraries (${CATEGORY}_tests
		PRIVATE Catch2::Catch2WithMain
		PRIVATE ${PROJECT_NAME} ## libarcstk from build-tree
	)

	## Set properties for all discovered tests

	set (TEST_PROPERTIES)

	if (SUITE_LABEL )
		list (APPEND TEST_PROPERTIES LABELS ${SUITE_LABEL} )
	endif()

	if (SUITE_TIMEOUT )
		list (APPEND TEST_PROPERTIES TIMEOUT ${SUITE_TIMEOUT} )
	endif()


	## Discover and register all tests from the executable to CTest
	catch_discover_tests (${CATEGORY}_tests
		TEST_PREFIX       "${CATEGORY}/"
		REPORTER          "junit"
		OUTPUT_DIR        "${LIBARCSTK_BINARY_DIR}/reports"
		OUTPUT_PREFIX     "report."
		OUTPUT_SUFFIX     ".xml"
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		PROPERTIES        ${TEST_PROPERTIES}
	)
endfunction()


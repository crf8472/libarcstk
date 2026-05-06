## Create a test executable from all .cpp files in a directory
## and register tests found with catch_discover_tests.
##
## Usage:
##   add_test_suite(unit
##       LABEL "unit"
##       TIMEOUT 10
##   )
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
			"No test sources found in ${CMAKE_CURRENT_SOURCE_DIR}/src/")
	endif()

	message (STATUS "Found ${CATEGORY} tests: ${TEST_SOURCES}")

	## Create executable
	add_executable (${CATEGORY}_tests ${TEST_SOURCES} )

	## Standard configuration
	set_target_properties (${CATEGORY}_tests PROPERTIES
		CXX_STANDARD          17
		CXX_STANDARD_REQUIRED ON
		BUILD_RPATH           "${CMAKE_BINARY_DIR}"
	)

	target_compile_options (${CATEGORY}_tests
		PRIVATE ${TEST_CXX_FLAGS_WARNINGS}
		PRIVATE ${PROJECT_CXX_FLAGS_OPTIMIZE}
	)

	## Include paths
	target_include_directories (${CATEGORY}_tests
		PRIVATE "${PROJECT_INCLUDE_DIR}"               ## public headers
		PRIVATE "${PROJECT_SOURCE_DIR}"                ## private headers
		PRIVATE "${PROJECT_BUILD_SOURCE_DIR}"          ## generated sources
		PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include"  ## test suite includes
		#PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/fixtures" ## test suite fixtures
		#PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../include" ## test helpers
	)

	## Link libraries
	target_link_libraries (${CATEGORY}_tests
		PRIVATE Catch2::Catch2WithMain
		PRIVATE ${PROJECT_NAME}
		## libarcstk from build-tree
	)

	## RPATH handling (force to load from build tree)
	#if (UNIX AND NOT APPLE )
	#	target_link_options (${CATEGORY}_tests PRIVATE -Wl,--disable-new-dtags)
	#endif()

	## Set properties for all discovered tests

	set (TEST_PROPERTIES)

	if (SUITE_LABEL)
		list (APPEND TEST_PROPERTIES LABELS ${SUITE_LABEL})
	endif()

	if (SUITE_TIMEOUT)
		list (APPEND TEST_PROPERTIES TIMEOUT ${SUITE_TIMEOUT})
	endif()


	## Discover and register all tests from the executable to CTest
	catch_discover_tests (${CATEGORY}_tests
		TEST_PREFIX       "${CATEGORY}/"
		REPORTER          "junit"
		OUTPUT_DIR        "${PROJECT_BINARY_DIR}/reports"
		OUTPUT_PREFIX     "report."
		OUTPUT_SUFFIX     ".xml"
		WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
		PROPERTIES        ${TEST_PROPERTIES}
	)
endfunction()


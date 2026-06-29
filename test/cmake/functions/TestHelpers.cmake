## libarcstk: CMake build script for configuring and building tests
## vim:fdm=marker

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
function (add_test_suite CATEGORY ) # {{{1

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

	## Generate flag set for tests from flag set of main target
	libarcstk_apply_compiler_flags (TEST TARGET ${CATEGORY}_tests )

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

	## Windows: Copy runtime DLLs to directory with test executables
	if (WIN32 )
		add_custom_command (TARGET ${CATEGORY}_tests POST_BUILD
			COMMAND ${CMAKE_COMMAND} -E copy_if_different
				$<TARGET_RUNTIME_DLLS:${CATEGORY}_tests>
				$<TARGET_FILE_DIR:${CATEGORY}_tests>
			COMMAND_EXPAND_LISTS
		)
	endif ()

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
# 1}}}


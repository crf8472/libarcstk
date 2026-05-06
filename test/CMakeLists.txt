## CMake build script for configuring and building tests
##
## vim:fdm=marker

cmake_minimum_required (VERSION 3.10 )

set (CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE )
set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE )

# Specific for Catch2 made available by FetchContent {{{1
# TODO Precondition: CMAKE_MODULE_PATH must contain ${catch2_SOURCE_DIR}/extras
include(CTest)
include(Catch)
#1}}}

add_custom_command (
	OUTPUT  "${PROJECT_BINARY_DIR}/reports"
	COMMAND "${CMAKE_COMMAND}"
	ARGS    -E make_directory "${PROJECT_BINARY_DIR}/reports"
	VERBATIM
)

add_custom_target (libarcstk_create_reports_dir ALL
	DEPENDS "${PROJECT_BINARY_DIR}/reports" )


## Include common test helpers
include ("${CMAKE_CURRENT_SOURCE_DIR}/cmake/functions/SetupEnvironment.cmake" )
include ("${CMAKE_CURRENT_SOURCE_DIR}/cmake/functions/TestHelpers.cmake" )


## Configure common test settings
configure_test_environment()

## Add tests
add_subdirectory (units )
add_subdirectory (functional )
add_subdirectory (type_traits )

## Add test suites for use on CLI
add_custom_target (test_units
    COMMAND ${CMAKE_CTEST_COMMAND} -L "units" --verbose
)
add_custom_target(test_functional
    COMMAND ${CMAKE_CTEST_COMMAND} -L "functional" --verbose
)
add_custom_target(test_type_traits
    COMMAND ${CMAKE_CTEST_COMMAND} -L "type_traits" --verbose
)
add_custom_target (test_all
    COMMAND ${CMAKE_CTEST_COMMAND} --verbose
)


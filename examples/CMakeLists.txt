## Build examples
## vim: fdm=marker

cmake_minimum_required (VERSION 3.10 )

## If set to 'ON' the examples target will be build with the default target,
## 'OFF' means the examples target does not exist.
option (WITH_EXAMPLES "Build examples" OFF )

if (WITH_EXAMPLES )
	message (STATUS "Build the examples" )
	add_custom_target (libarcstk_examples ALL )

	## Search path for cmake modules
	## (Note that this does _not_ go to PARENT_SCOPE!)
	list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" )

	add_subdirectory (albumcalc   )
	add_subdirectory (albumid     )
	add_subdirectory (albumverify )
	add_subdirectory (dbarparse   )
endif()


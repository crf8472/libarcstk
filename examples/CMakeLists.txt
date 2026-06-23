## Build examples
## vim: fdm=marker

cmake_minimum_required (VERSION 3.10 )

#set (CMAKE_BUILD_WITH_INSTALL_RPATH    FALSE )
#set (CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE  )

## If set to 'ON' the examples target will be build with the default target,
## 'OFF' means the examples target has to be built explicitly.
option (WITH_EXAMPLES "Build examples" OFF )

if (WITH_EXAMPLES )
	message (STATUS "Build the examples" )
	add_custom_target (libarcstk_examples ALL )
else()
	add_custom_target (libarcstk_examples )
endif()

## Search path for cmake modules (note that this does _not_ go to PARENT_SCOPE!)
list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules" )

add_subdirectory (albumcalc   )
add_subdirectory (albumid     )
add_subdirectory (albumverify )
add_subdirectory (dbarparse   )

#add_dependencies ( ${PROJECT_NAME} libarcstk_examples )


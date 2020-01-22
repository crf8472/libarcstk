## CMake  sphinx
##
## Use sphinx to build the Read-the-docs documentation from Doxygen XML output

## Prerequisites from parent:
## - 'doc' target must exist
## - Variables: DOC_GEN_DIR, DOC_COMMENT

if (WIN32 )

	message (FATAL_ERROR
		"Sphinx support is currently not available for Windows" )
endif()


cmake_minimum_required (VERSION 3.0.2 )

## Doxygen is already required in parent

message (STATUS "Will build documentation with sphinx" )



## --- Overwrite default values from parent

set (DOXYGEN_OUTPUT_DIRECTORY "${DOC_GEN_DIR}/read-the-docs" )
set (DOC_COMMENT              "${DOC_COMMENT}, using sphinx" )

## --- Some variables

set (OUTPUT_MAINPAGE   "${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html" )
set (OUTPUT_INDEX      "${DOXYGEN_OUTPUT_DIRECTORY}/xml/index.xml" )
set (DOXYFILE_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in" )
set (DOXYFILE          "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" )

set (SPHINX_BINARY_DIR        "${DOXYGEN_OUTPUT_DIRECTORY}" )
set (SPHINX_LIBRARY_ROOT_PAGE "${SPHINX_BINARY_DIR}/html/api/library_root.html" )



## --- Install files and directories in sphinx build directory

file (MAKE_DIRECTORY "${SPHINX_BINARY_DIR}/source/_static" )
file (MAKE_DIRECTORY "${SPHINX_BINARY_DIR}/source/_templates" )
#file (MAKE_DIRECTORY "${SPHINX_BINARY_DIR}/build" )

configure_file ("docroot/source/conf.py.in"
	"${SPHINX_BINARY_DIR}/source/conf.py"
	@ONLY
)

configure_file ("docroot/Makefile.in"
	"${SPHINX_BINARY_DIR}/Makefile"
	@ONLY
)

configure_file ("docroot/source/index.rst.in"
	"${SPHINX_BINARY_DIR}/source/index.rst"
	@ONLY COPYONLY
)


add_custom_target (rtd_doc DEPENDS "${OUTPUT_MAINPAGE}" )


## --- Set up python environment

add_requirements_target (rtd_requirements
	"${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
)



## --- Prepare Doxygen XML generation

configure_file ("${DOXYFILE_TEMPLATE}" "${DOXYFILE}" @ONLY )

add_doxygen_target (rtd_doxygen "${DOXYFILE}" "${DOXYFILE_TEMPLATE}"
	"${OUTPUT_INDEX}"
)

if (DOXYGEN_NEEDS_PYTHON )

	add_dependencies (rtd_doxygen rtd_requirements )
	add_dependencies (rtd_doc     rtd_doxygen )
else()

	add_dependencies (rtd_doc     rtd_requirements )
endif()



# --- Run sphinx to generate Read-the-docs documentation from doxygen's output

add_custom_command (
	OUTPUT  "${OUTPUT_MAINPAGE}" "${SPHINX_LIBRARY_ROOT_PAGE}"
	COMMAND make html
	DEPENDS "${OUTPUT_INDEX}" "${SPHINX_BINARY_DIR}/Makefile"
	WORKING_DIRECTORY "${SPHINX_BINARY_DIR}"
	COMMENT "${DOC_COMMENT}"
	VERBATIM
)

add_dependencies  (doc rtd_doc )


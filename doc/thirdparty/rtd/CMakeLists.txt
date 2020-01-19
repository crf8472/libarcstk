## CMake  sphinx
##
## Use sphinx to build the Read-the-docs documentation from Doxygen XML output

## Prerequisites from parent:
## - 'doc' target must exist
## - Variables: DOC_BINARY_DIR, DOC_DOXYFILE_BASE

if (WIN32 )

	message (FATAL_ERROR
		"Sphinx support is currently not available for Windows" )
endif()


cmake_minimum_required (VERSION 3.0.2 )

## Doxygen is already required in parent

message (STATUS "Will build documentation with sphinx" )



## --- Overwrite default values from parent

set (DOXYGEN_OUTPUT_DIRECTORY "${DOC_BINARY_DIR}/read-the-docs" )
set (DOC_OUTPUT_MAINPAGE      "${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html" )
set (DOC_OUTPUT_INDEX         "${DOXYGEN_OUTPUT_DIRECTORY}/xml/index.xml" )
set (DOC_DOXYFILE             "${DOXYGEN_OUTPUT_DIRECTORY}/Doxyfile" )
set (DOC_COMMENT              "${DOC_COMMENT}, using sphinx" )

## Define list of sources for replacement in Doxyfile
string (REPLACE ";" " " DOXYGEN_INPUT
	"${DOC_SOURCES} ${PROJECT_BUILD_SOURCE_DIR}/version.hpp" )

set (PYTHON_REQUIREMENTS      "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt" )
#set (PYTHON_SANITIZE_SCRIPT   "dedup_index.py" )  ## sanitize doxygen >= 1.8.16
#set (PYTHON_SANITIZE_SHEET    "dedup_index.xsl" )



## --- sphinx identifiers

set (SPHINX_BINARY_DIR        "${DOXYGEN_OUTPUT_DIRECTORY}" )
set (SPHINX_DOXYFILE_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in" )
set (SPHINX_LIBRARY_ROOT_PAGE "${SPHINX_BINARY_DIR}/html/api/library_root.html" )


## --- Prepare Doxygen XML generation

configure_file ("${SPHINX_DOXYFILE_TEMPLATE}" "${DOC_DOXYFILE}" @ONLY )

# Run doxygen to generate XML output
add_custom_command (
	OUTPUT  "${DOC_OUTPUT_INDEX}"
	COMMAND Doxygen::doxygen "${DOC_DOXYFILE}"
	#COMMAND "${PYTHON_CMD}" "${DOC_BINARY_DIR}/${PYTHON_SANITIZE_SCRIPT}"
	DEPENDS "${DOC_DOXYFILE_BASE_TEMPLATE}"
			"${DOC_DOXYFILE_BASE}"
			"${SPHINX_DOXYFILE_TEMPLATE}"
			"${DOC_DOXYFILE}"
			${DOC_SOURCES}
	WORKING_DIRECTORY "${SPHINX_BINARY_DIR}"
	COMMENT "${DOC_COMMENT}"
	VERBATIM
)
## Exhale could very well do this for us, but we need to intercept the XML
## produced by doxygen >=1.8.16 to sanitize inline namespaces.

#add_custom_target (doxygen_xml DEPENDS "${DOC_OUTPUT_INDEX}" )



## --- Set up python environment and doxygen settings

## Note: PYTHON_REQUIREMENTS must have been set before this point
add_subdirectory ("${PROJECT_DOC_DIR}/thirdparty/python-virtualenv"
	"${PROJECT_BINARY_DIR}/doc/thirdparty/python-virtualenv" )



## --- Install files and directories in sphinx build directory

file (MAKE_DIRECTORY "${SPHINX_BINARY_DIR}/source" )
file (MAKE_DIRECTORY "${SPHINX_BINARY_DIR}/source/_static" )
file (MAKE_DIRECTORY "${SPHINX_BINARY_DIR}/source/_templates" )
#file (MAKE_DIRECTORY "${SPHINX_BINARY_DIR}/build" )

configure_file ("docroot/source/conf.py.in"
	"${SPHINX_BINARY_DIR}/source/conf.py"
	@ONLY
)

configure_file ("docroot/source/index.rst.in"
	"${SPHINX_BINARY_DIR}/source/index.rst"
	@ONLY
)

# Note: python-virtualenv must have been processed before this point
configure_file ("docroot/Makefile.in"
	"${SPHINX_BINARY_DIR}/Makefile"
	@ONLY
)

set (DOXYFILE "${SPHINX_BINARY_DIR}/source/Doxyfile" )

configure_file ("${SPHINX_DOXYFILE_TEMPLATE}" "${DOXYFILE}" @ONLY )



## --- Created Read-the-docs target

## Create RTD docs from doxygen XML
add_custom_command (
	OUTPUT  "${DOC_OUTPUT_MAINPAGE}" "${SPHINX_LIBRARY_ROOT_PAGE}"
	COMMAND make html
	DEPENDS "${DOC_OUTPUT_INDEX}" "${SPHINX_BINARY_DIR}/Makefile"
	WORKING_DIRECTORY "${SPHINX_BINARY_DIR}"
	COMMENT "${DOC_COMMENT}"
	VERBATIM
)

add_custom_target (doc_rtd DEPENDS "${DOC_OUTPUT_MAINPAGE}" )
add_dependencies (doc_rtd python-virtualenv )

add_dependencies (doc doc_rtd )


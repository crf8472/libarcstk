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
message (WARNING "Sphinx support for Read-the-docs is experimental and broken" )



## --- Overwrite default values from parent

set (DOXYGEN_OUTPUT_DIRECTORY "${DOC_GEN_DIR}/read-the-docs" )
set (DOC_COMMENT              "${DOC_COMMENT}, using sphinx" )

## Some variables

set (OUTPUT_MAINPAGE   "${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html" )
set (OUTPUT_INDEX      "${DOXYGEN_OUTPUT_DIRECTORY}/xml/index.xml" )
set (DOXYFILE_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in" )

set (SPHINX_BINARY_DIR        "${DOXYGEN_OUTPUT_DIRECTORY}" )
set (SPHINX_LIBRARY_ROOT_PAGE "${SPHINX_BINARY_DIR}/html/api/library_root.html" )



## --- Install files and directories in sphinx build directory

file (MAKE_DIRECTORY "${SPHINX_BINARY_DIR}/source/_static" )
file (MAKE_DIRECTORY "${SPHINX_BINARY_DIR}/source/_templates" )
#file (MAKE_DIRECTORY "${SPHINX_BINARY_DIR}/build" )

configure_file ("docroot/Makefile.in"
	"${SPHINX_BINARY_DIR}/Makefile"
	@ONLY
)

configure_file ("docroot/source/index.rst.in"
	"${SPHINX_BINARY_DIR}/source/index.rst"
	@ONLY COPYONLY
)



## --- Set up python environment

add_requirements_target (rtd_requirements
	"${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
)



## --- Set up targets

add_custom_target (rtd_doc DEPENDS "${OUTPUT_MAINPAGE}" )
add_dependencies  (rtd_doc rtd_requirements )

add_dependencies  (doc rtd_doc )



## --- Prepare Doxygen XML generation


set (EXHALE_EXECUTES_DOXYGEN "True" )


if (DOXYGEN_XML_NEEDS_DEDUP )

	# --- Run doxygen in a separate target and make exhale depend on its output

	set (EXHALE_EXECUTES_DOXYGEN "False" )
	set (DOXYFILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" )

	add_doxygen_target (rtd_doxygen "${DOXYFILE}" "${DOXYFILE_TEMPLATE}"
		"${OUTPUT_INDEX}"
	)

	add_dependencies (rtd_doc rtd_doxygen )

	add_custom_command (
		OUTPUT  "${OUTPUT_MAINPAGE}" "${SPHINX_LIBRARY_ROOT_PAGE}"
		COMMAND make html
		DEPENDS "${OUTPUT_INDEX}"
				"${SPHINX_BINARY_DIR}/Makefile"
				"${SPHINX_BINARY_DIR}/source/index.rst"
				"${SPHINX_BINARY_DIR}/source/conf.py"
				"${DOC_DOXYFILE_BASE_TEMPLATE}"
				"${DOC_DOXYFILE_BASE}"
				"${DOXYFILE_TEMPLATE}"
				"${DOXYFILE}"
				${DOC_SOURCES}
		WORKING_DIRECTORY "${SPHINX_BINARY_DIR}"
		COMMENT "${DOC_COMMENT}"
		VERBATIM
	)
else()

	# --- Let exhale run doxygen

	set (DOXYFILE "${SPHINX_BINARY_DIR}/source/Doxyfile" )

	add_custom_command (
		OUTPUT  "${OUTPUT_MAINPAGE}" "${SPHINX_LIBRARY_ROOT_PAGE}"
		COMMAND make html
		DEPENDS "${SPHINX_BINARY_DIR}/Makefile"
				"${SPHINX_BINARY_DIR}/source/index.rst"
				"${SPHINX_BINARY_DIR}/source/conf.py"
				"${DOC_DOXYFILE_BASE_TEMPLATE}"
				"${DOC_DOXYFILE_BASE}"
				"${DOXYFILE_TEMPLATE}"
				"${DOXYFILE}"
				${DOC_SOURCES}
		WORKING_DIRECTORY "${SPHINX_BINARY_DIR}"
		COMMENT "${DOC_COMMENT}"
		VERBATIM
	)
endif()


configure_file ("docroot/source/conf.py.in"
	"${SPHINX_BINARY_DIR}/source/conf.py"
	@ONLY
)

configure_file ("${DOXYFILE_TEMPLATE}" "${DOXYFILE}" @ONLY )


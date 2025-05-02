## CMake file for sphinx/exhale
##
## Use sphinx to build the Read-the-docs documentation from Doxygen XML output
##
## Prerequisites from PARENT:
## - Targets:   'doc'
## - Variables: DOC_COMMENT, DOC_DOXYFILE_BASE, DOC_DOXYFILE_BASE_TEMPLATE,
##              DOC_GEN_DIR, DOC_SOURCES, DOXYGEN_NEEDS_XML_DEDUP
##
## Provides:
## - Targets: 'libarcstk_rtd_doc'

if (WIN32 )

	message (FATAL_ERROR
		"Sphinx support is currently not available for Windows" )
endif()


cmake_minimum_required (VERSION 3.10 )

message (STATUS "Will build documentation with sphinx" )
message (WARNING "Sphinx support for Read-the-docs is experimental and broken" )



## --- Overwrite default values from parent

set (DOXYGEN_OUTPUT_DIRECTORY "${DOC_GEN_DIR}/read-the-docs" )
set (DOC_COMMENT              "${DOC_COMMENT}, using sphinx" )

## --- Some variables

set (OUTPUT_MAINPAGE   "${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html" )
set (OUTPUT_INDEX      "${DOXYGEN_OUTPUT_DIRECTORY}/xml/index.xml" )
set (DOXYFILE_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in" )

set (SPHINX_BINARY_DIR        "${DOXYGEN_OUTPUT_DIRECTORY}" )
set (SPHINX_LIBRARY_ROOT_PAGE "${SPHINX_BINARY_DIR}/html/api/library_root.html" )



## --- Set up targets

add_custom_target (libarcstk_rtd_doc )

add_custom_target (libarcstk_rtd_generate_html DEPENDS "${OUTPUT_MAINPAGE}" )
add_dependencies  (libarcstk_rtd_doc libarcstk_rtd_generate_html )


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

add_requirements_target (libarcstk_rtd_requirements
	"${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
)

add_dependencies  (libarcstk_rtd_generate_html libarcstk_rtd_requirements )



## --- Build rule for running exhale

set (EXHALE_EXECUTES_DOXYGEN "True" )

if (DOXYGEN_NEEDS_XML_DEDUP )

	## Doxygen needs to run separately from exhale since we want to post-process
	## its XML output.
	## So run doxygen in a separate target and make exhale depend on its output.

	set (EXHALE_EXECUTES_DOXYGEN "False" )
	set (DOXYFILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" )

	add_doxygen_target (libarcstk_rtd_generate_xml ${DOXYFILE} ${OUTPUT_INDEX} )

	add_dependencies (libarcstk_rtd_doc libarcstk_rtd_generate_xml )

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

	## Doxygen is called by exhale.

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


## Create Doxyfile for sphinx
configure_file ("${DOXYFILE_TEMPLATE}" "${DOXYFILE}" @ONLY )


## Create conf.py for sphinx
configure_file ("docroot/source/conf.py.in"
	"${SPHINX_BINARY_DIR}/source/conf.py"
	@ONLY
)

add_dependencies  (libarcstk_doc libarcstk_rtd_doc )


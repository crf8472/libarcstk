## CMake file for sphinx/exhale
##
## vim:fdm=marker
##
## Use sphinx to build the Read-the-docs documentation from Doxygen XML output
##
## Prerequisites from PARENT:
## - Targets:   'doc'
## - Variables: DOC_COMMENT, DOC_DOXYFILE_BASE, DOC_DOXYFILE_BASE_TEMPLATE,
##              DOC_GEN_DIR, DOC_SOURCES
##
## Provides:
## - Targets: 'libarcstk_rtd_doc'

cmake_minimum_required (VERSION 3.10 )

message (STATUS "Will build documentation with sphinx" )

if (WIN32 )

	message (FATAL_ERROR
		"Sphinx support is currently not available for Windows" )
endif()

message (WARNING "Sphinx support for Read-the-docs is experimental and broken" )


## --- Overwrite Default Values From Parent {{{1

set (DOXYGEN_OUTPUT_DIRECTORY "${DOC_GEN_DIR}/read-the-docs" )
set (DOC_COMMENT              "${DOC_COMMENT}, using sphinx" )



## --- Some Variables {{{1

set (INDEX_HTML               "${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html" )
set (INDEX_XML                "${DOXYGEN_OUTPUT_DIRECTORY}/xml/index.xml" )
set (DOXYFILE_TEMPLATE        "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in" )
set (SPHINX_BINARY_DIR        "${DOXYGEN_OUTPUT_DIRECTORY}" )
set (SPHINX_LIBRARY_ROOT_PAGE "${SPHINX_BINARY_DIR}/html/api/library_root.html" )



## --- Set up Targets {{{1

add_custom_target (libarcstk_rtd_generate_html DEPENDS "${INDEX_HTML}" )
add_dependencies  (libarcstk_doc_generate_html libarcstk_rtd_generate_html )



## --- Install Files and Directories in Sphinx Build Directory {{{1

file (MAKE_DIRECTORY "${SPHINX_BINARY_DIR}/source/_static" )
file (MAKE_DIRECTORY "${SPHINX_BINARY_DIR}/source/_templates" )

configure_file ("docroot/Makefile.in"
	"${SPHINX_BINARY_DIR}/Makefile"
	@ONLY
)

configure_file ("docroot/source/index.rst.in"
	"${SPHINX_BINARY_DIR}/source/index.rst"
	@ONLY COPYONLY
)



## --- Set up Python Environment {{{1

add_requirements_target (libarcstk_rtd_requirements
	"${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
)

add_dependencies  (libarcstk_rtd_generate_html
	libarcstk_rtd_requirements
	libarcstk_doxygen_provide_xml
)



## --- Build Rule for Running Exhale {{{1

set (EXHALE_EXECUTES_DOXYGEN "True" )

if (DOXYGEN_XML_POSTPROCESSING_TASKS )

	## Doxygen needs to run separately from exhale since we want to post-process
	## its XML output.
	## So run doxygen in a separate target and make exhale depend on its output.

	set (EXHALE_EXECUTES_DOXYGEN "False" )
	set (DOXYFILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" )

	add_doxygen_xml_target (libarcstk_rtd_generate_xml
		"${DOXYFILE}" "${INDEX_XML}" )

	add_custom_command (
		OUTPUT  "${INDEX_HTML}" "${SPHINX_LIBRARY_ROOT_PAGE}"
		COMMAND make html
		DEPENDS "${INDEX_XML}"
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

	## Since we do not use add_doxygen_target, we have to take care for the
	## dependencies manually
	add_dependencies (libarcstk_rtd_generate_html
		## add deps from PARENT
		libarcstk_create_output_dir
		libarcstk_create_log_dir
		libarcstk_static_pages_files
		## add deps from here
		libarcstk_mcss_requirements
	)

	add_custom_command (
		OUTPUT  "${INDEX_HTML}" "${SPHINX_LIBRARY_ROOT_PAGE}"
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


## --- Configure Doxyfile for sphinx {{{1
configure_file ("${DOXYFILE_TEMPLATE}" "${DOXYFILE}" @ONLY )


## --- Configure conf.py for sphinx {{{1
configure_file ("docroot/source/conf.py.in"
	"${SPHINX_BINARY_DIR}/source/conf.py"
	@ONLY
)


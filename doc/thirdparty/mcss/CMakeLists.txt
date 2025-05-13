## CMake file for m.css
##
## Use HTML5/CSS3 from m.css for generating HTML from Doxygen XML output
##
## Prerequisites from PARENT:
## - Targets:   'doc'
## - Variables: DOC_COMMENT, DOC_DOXYFILE_BASE, DOC_GEN_DIR, DOC_SOURCES
##              DOXYGEN_XML_DEDUP_INDEX_REQUIRED
##
## Provides:
## - Targets: 'libarcstk_mcss_doc'

if (WIN32 )

	message (FATAL_ERROR
		"m.css support is currently not available for Windows" )
endif()


cmake_minimum_required (VERSION 3.10 )

message (STATUS "Will build documentation with m.css" )

find_package (Git REQUIRED )



## --- Overwrite default values from parent

set (DOXYGEN_OUTPUT_DIRECTORY "${DOC_GEN_DIR}/mcss" )
set (DOC_COMMENT              "${DOC_COMMENT}, using m.css" )

## --- Some variables

set (INDEX_HTML        "${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html" )
set (INDEX_XML         "${DOXYGEN_OUTPUT_DIRECTORY}/xml/index.xml" )
set (DOXYFILE_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in" )
set (DOXYFILE          "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" )
set (CONFPY            "${CMAKE_CURRENT_BINARY_DIR}/conf.py" )

set (MCSS_REPO_URL       "https://github.com/mosra/m.css.git" )
set (MCSS_SCRIPT_RELPATH "documentation/doxygen.py" )
## Commented out:
## Before commit 3f568563a8b70fc49a479250d51a2fdc71a66943:
## set (MCSS_SCRIPT_RELPATH "doxygen/dox2html5.py" )

add_custom_target (libarcstk_mcss_generate_html DEPENDS "${INDEX_HTML}" )

add_dependencies  (libarcstk_doc_generate_html libarcstk_mcss_generate_html )


## --- Define m.css to be checked out/updated while configuring

if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.11.0" )

	## --- Use FetchContent if possible

	## This will NOT update the cloned m.css on build!

	include (FetchContent )

	FetchContent_Declare (m_css
		GIT_REPOSITORY    "${MCSS_REPO_URL}"
		#GIT_TAG          ... use always HEAD of default branch
		UPDATE_COMMAND    ${GIT_EXECUTABLE} pull
	)

	if (${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.14" )

		FetchContent_MakeAvailable (m_css )

	else()

		if (NOT m_css_POPULATED )

			FetchContent_Populate (m_css )

		endif (NOT m_css_POPULATED )
	endif()

	set (MCSS_SOURCE_DIR "${m_css_SOURCE_DIR}" )
	set (MCSS_CMD "${m_css_SOURCE_DIR}/${MCSS_SCRIPT_RELPATH}" )

else() ## No FetchContent module available

	## --- Define m.css as target (will be updated on build)

	include (ExternalProject )

	ExternalProject_Add (libarcstk_mcss_download
		GIT_REPOSITORY    ${MCSS_REPO_URL}
		#GIT_TAG          ... use always HEAD of default branch
		GIT_SHALLOW       true
		GIT_PROGRESS      true
		TIMEOUT           10
		# DOWNLOAD_COMMAND use default
		PATCH_COMMAND     ""
		UPDATE_COMMAND    ${GIT_EXECUTABLE} pull
		CONFIGURE_COMMAND ""
		BUILD_COMMAND     ""
		INSTALL_COMMAND   ""
		TEST_COMMAND      ""
		LOG_DOWNLOAD      ON
	)

	ExternalProject_Get_Property (mcss_download SOURCE_DIR )

	set (MCSS_SOURCE_DIR "${SOURCE_DIR}" )
	set (MCSS_CMD "${SOURCE_DIR}/${MCSS_SCRIPT_RELPATH}" )

	unset(SOURCE_DIR )

	add_dependencies (libarcstk_mcss_generate_html libarcstk_mcss_download )
endif()



## Create Doxyfile for m.css
configure_file ("${DOXYFILE_TEMPLATE}"  "${DOXYFILE}"  @ONLY )


## Create conf.py for m.css
set (CONFPY_SOURCE "${CMAKE_CURRENT_SOURCE_DIR}/conf.py.in" )
configure_file ("${CONFPY_SOURCE}" "${CONFPY}"  @ONLY )


## --- Configure custom colors
#add_subdirectory (custom-colors )
## TODO Activate this for custom colors



## --- Set up python environment

add_requirements_target (libarcstk_mcss_requirements
	"${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
)

add_dependencies (libarcstk_mcss_generate_html libarcstk_mcss_requirements )



## --- Build rule for running m.css

set (MCSS_LOG_FILE "${PROJECT_LOG_DIR}/MCSS.log" )

if (DOXYGEN_XML_DEDUP_INDEX_REQUIRED )

	## Doxygen needs to run separately from m.css since we want to post-process
	## its XML output.

	add_doxygen_target (libarcstk_mcss_generate_xml
		${DOXYFILE}
		${INDEX_XML}
	)

	add_dependencies (libarcstk_mcss_generate_html libarcstk_mcss_generate_xml )

	add_custom_command (
		OUTPUT  "${INDEX_HTML}"
		COMMAND "${PYTHON_CMD}"
		ARGS    "${MCSS_CMD}" --no-doxygen "${CONFPY}" 2> "${MCSS_LOG_FILE}"
		DEPENDS ${INDEX_XML} ${DOC_SOURCES}
		WORKING_DIRECTORY "${DOC_GEN_DIR}"
		COMMENT "${DOC_COMMENT}"
		VERBATIM
	)
else()

	## Doxygen is called by m.css.

	## Since we do not use add_doxygen_target, we have to take care for the
	## dependencies manually
	add_dependencies (libarcstk_mcss_generate_html
		## add deps from PARENT
		libarcstk_create_output_dir
		libarcstk_create_log_dir
		libarcstk_static_pages_files
		## add deps from here
		libarcstk_mcss_requirements
	)

	add_custom_command (
		OUTPUT  "${INDEX_HTML}"
		COMMAND "${PYTHON_CMD}"
		ARGS    "${MCSS_CMD}" "${CONFPY}" 2> "${MCSS_LOG_FILE}"
		DEPENDS "${DOC_DOXYFILE_BASE}"
				"${DOXYFILE}"
				${DOC_SOURCES}
		WORKING_DIRECTORY "${DOC_GEN_DIR}"
		COMMENT "${DOC_COMMENT}"
		VERBATIM
	)
endif()

#add_dependencies (libarcstk_doc libarcstk_mcss_doc )


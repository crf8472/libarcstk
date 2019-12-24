## CMake file for m.css
##
## Use HTML5/CSS3 from m.css for generating HTML from Doxygen XML output

## Prerequisites from parent:
## - 'doc' target must exist
## - Variables: DOC_BINARY_DIR, DOC_DOXYFILE_BASE

if (WIN32 )

	message (FATAL_ERROR
		"M.CSS support is currently not available for Windows" )
endif()


cmake_minimum_required (VERSION 3.0.2 )

find_package (Git REQUIRED )

message (STATUS "Will build documentation with m.css" )



## --- Overwrite default values from parent

set (DOXYGEN_OUTPUT_DIRECTORY "${DOC_BINARY_DIR}/mcss" )
set (DOC_OUTPUT_MAINPAGE      "${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html" )
set (DOC_DOXYFILE             "${DOXYGEN_OUTPUT_DIRECTORY}/Doxyfile" )
set (DOC_COMMENT              "${DOC_COMMENT}, using m.css" )
set (PYTHON_REQUIREMENTS      "${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt" )



## --- m.css identifiers

set (MCSS_DOXYFILE_TEMPLATE   "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in" )
set (MCSS_REPO_URL "https://github.com/mosra/m.css.git" )
set (MCSS_SCRIPT_RELPATH "documentation/doxygen.py" )
## Commented out:
## Before commit 3f568563a8b70fc49a479250d51a2fdc71a66943:
## set (MCSS_SCRIPT_RELPATH "doxygen/dox2html5.py" )
set (MCSS_TARGET provide_mcss ) ## just an initial dummy value



## --- Define m.css to be checked out/updated while configuring

if (${CMAKE_VERSION} VERSION_LESS "3.11.0" ) ## No FetchContent module available

	## --- Define m.css as target

	include (ExternalProject )

	ExternalProject_Add (${MCSS_TARGET}
		GIT_REPOSITORY    ${MCSS_REPO_URL}
		#GIT_TAG          ... use always HEAD of master
		GIT_SHALLOW       true
		GIT_PROGRESS      true
		TIMEOUT           10
		# DOWNLOAD_COMMAND use default
		PATCH_COMMDAND    ""
		UPDATE_COMMAND    ${GIT_EXECUTABLE} pull
		CONFIGURE_COMMAND ""
		BUILD_COMMAND     ""
		INSTALL_COMMAND   ""
		TEST_COMMAND      ""
		LOG_DOWNLOAD      ON
	)

	add_dependencies (doc provide_mcss ) ## 'doc' target must already exist!

	ExternalProject_Get_Property (${MCSS_TARGET} SOURCE_DIR )

	set (MCSS_CMD "${SOURCE_DIR}/${MCSS_SCRIPT_RELPATH}" )

	unset(SOURCE_DIR )

else()

	## --- Fetch m.css

	include (FetchContent )

	FetchContent_Declare (${MCSS_TARGET}
		GIT_REPOSITORY    "${MCSS_REPO_URL}"
		#GIT_TAG          ... use always HEAD of master
	)

	if (${CMAKE_VERSION} VERSION_LESS "3.14" )

		#FetchContent_GetProperties (${MCSS_TARGET} )

		if (NOT ${MCSS_TARGET}_POPULATED )

			FetchContent_Populate (${MCSS_TARGET} )

		endif()
	else()

		# with 3.14 comes:
		FetchContent_MakeAvailable (${MCSS_TARGET} )

	endif()

	set (MCSS_CMD "${${MCSS_TARGET}_SOURCE_DIR}/${MCSS_SCRIPT_RELPATH}" )

endif()



## --- Set up python environment and doxygen settings

## Note: PYTHON_REQUIREMENTS must have been set before this point
add_subdirectory ("${PROJECT_DOC_DIR}/thirdparty/python-virtualenv"
	"${PROJECT_BINARY_DIR}/doc/thirdparty/python-virtualenv" )
## Note: The binary dir is unused, so CMAKE_CURRENT_SOURCE_DIR is ok

configure_file ("${MCSS_DOXYFILE_TEMPLATE}" "${DOC_DOXYFILE}" @ONLY )



## --- Provide debug output from m.css if requested

if (MCSS_DEBUG )

	set (MCSS_FLAGS "--debug" )
endif (MCSS_DEBUG )



## --- Generate DOC_OUTPUT_MAINPAGE by running m.css command

# Run m.css command to generate documentation
add_custom_command (
	OUTPUT  "${DOC_OUTPUT_MAINPAGE}"
	COMMAND "${PYTHON_CMD}" "${MCSS_CMD}" ${MCSS_FLAGS} "${DOC_DOXYFILE}"
	DEPENDS "${DOC_DOXYFILE_BASE}" "${MCSS_DOXYFILE_TEMPLATE}" ${DOC_SOURCES}
	WORKING_DIRECTORY "${ENV_DIRECTORY}"
	COMMENT "${DOC_COMMENT}"
	VERBATIM
)

add_custom_target (doc_mcss DEPENDS "${DOC_OUTPUT_MAINPAGE}" )
add_dependencies(doc_mcss python-virtualenv )

add_dependencies(doc doc_mcss )

## CMake file for m.css
##
## Use HTML5/CSS3 from m.css for generating HTML from Doxygen XML output

## Prerequisites from parent:
## - 'doc' target must exist
## - Variables: DOC_GEN_DIR, DOC_COMMENT

if (WIN32 )

	message (FATAL_ERROR
		"M.CSS support is currently not available for Windows" )
endif()


cmake_minimum_required (VERSION 3.0.2 )

find_package (Git REQUIRED )

message (STATUS "Will build documentation with m.css" )



## --- Overwrite default values from parent

set (DOXYGEN_OUTPUT_DIRECTORY "${DOC_GEN_DIR}/mcss" )
set (DOC_COMMENT              "${DOC_COMMENT}, using m.css" )

## --- Some variables

set (OUTPUT_MAINPAGE   "${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html" )
set (OUTPUT_INDEX      "${DOXYGEN_OUTPUT_DIRECTORY}/xml/index.xml" )
set (DOXYFILE_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in" )
set (DOXYFILE          "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" )

set (MCSS_REPO_URL       "https://github.com/mosra/m.css.git" )
set (MCSS_SCRIPT_RELPATH "documentation/doxygen.py" )
## Commented out:
## Before commit 3f568563a8b70fc49a479250d51a2fdc71a66943:
## set (MCSS_SCRIPT_RELPATH "doxygen/dox2html5.py" )



## --- Define m.css to be checked out/updated while configuring

if (${CMAKE_VERSION} VERSION_LESS "3.11.0" ) ## No FetchContent module available

	## --- Define m.css as target

	include (ExternalProject )

	ExternalProject_Add (mcss_provide
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

	add_dependencies (doc mcss_provide ) ## 'doc' target must already exist!

	ExternalProject_Get_Property (mcss_provide SOURCE_DIR )

	set (MCSS_CMD "${SOURCE_DIR}/${MCSS_SCRIPT_RELPATH}" )

	unset(SOURCE_DIR )

else()

	## --- Fetch m.css

	include (FetchContent )

	FetchContent_Declare (mcss_provide
		GIT_REPOSITORY    "${MCSS_REPO_URL}"
		#GIT_TAG          ... use always HEAD of master
	)

	if (${CMAKE_VERSION} VERSION_LESS "3.14" )

		#FetchContent_GetProperties (mcss_provide )

		if (NOT mcss_provide_POPULATED )

			FetchContent_Populate (mcss_provide )

		endif()
	else()

		# with 3.14 comes:
		FetchContent_MakeAvailable (mcss_provide )

	endif()

	set (MCSS_CMD "${mcss_provide_SOURCE_DIR}/${MCSS_SCRIPT_RELPATH}" )

endif()



## --- Set up python environment and doxygen settings

add_requirements_target (mcss_requirements
	"${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
)


## --- Run doxygen to generate XML

configure_file ("${DOXYFILE_TEMPLATE}" "${DOXYFILE}" @ONLY )

add_doxygen_target (mcss_doxygen "${DOXYFILE}" "${DOXYFILE_TEMPLATE}"
	"${OUTPUT_INDEX}"
)


# --- Run m.css to generate documentation from doxygen's output

add_custom_command (
	OUTPUT  "${OUTPUT_MAINPAGE}"
	COMMAND "${PYTHON_CMD}" "${MCSS_CMD}" --no-doxygen "${DOXYFILE}"
			2> "${PROJECT_LOG_DIR}/MCSS.log"
	DEPENDS "${OUTPUT_INDEX}"
	WORKING_DIRECTORY "${DOC_GEN_DIR}"
	COMMENT "${DOC_COMMENT}"
	VERBATIM
)

add_custom_target (mcss_doc DEPENDS "${OUTPUT_MAINPAGE}" )
add_dependencies  (mcss_doc mcss_requirements mcss_doxygen )

add_dependencies  (doc mcss_doc )


## CMake file for m.css

cmake_minimum_required (VERSION 3.0.2 )

find_package (Git REQUIRED )


## --- m.css identifiers


set(MCSS_REPO_URL "https://github.com/mosra/m.css.git" )
set(MCSS_TAG      "" )
set(MCSS_SCRIPT_RELPATH "documentation/doxygen.py" )
## Commented out: pre commit 3f568563a8b70fc49a479250d51a2fdc71a66943, it is:
#set(MCSS_SCRIPT_RELPATH "doxygen/dox2html5.py" )
set(MCSS_TARGET provide_mcss )


## --- Define m.css to be checked out/updated while configuring

if (${CMAKE_VERSION} VERSION_LESS "3.11.0" ) ## No FetchContent module available


	## --- Define m.css as target

	include (ExternalProject )

	ExternalProject_Add (${MCSS_TARGET}
		GIT_REPOSITORY    ${MCSS_REPO_URL}
		#GIT_TAG           ${MCSS_TAG}
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

	set (MCSS_CMD "${SOURCE_DIR}/${MCSS_SCRIPT_RELPATH}" PARENT_SCOPE )

	unset(SOURCE_DIR )

else()

	## --- Fetch m.css

	include (FetchContent )

	FetchContent_Declare (${MCSS_TARGET}
		GIT_REPOSITORY    "${MCSS_REPO_URL}"
		#GIT_TAG           "${MCSS_TAG}"
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

	set (MCSS_CMD "${${MCSS_TARGET}_SOURCE_DIR}/${MCSS_SCRIPT_RELPATH}"
		PARENT_SCOPE )

endif()


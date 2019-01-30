## CMake file for single-header version of m.css

cmake_minimum_required (VERSION 3.0.2 )

find_package (Git REQUIRED )


## --- m.css identifiers


set(MCSS_REPO_URL    "https://github.com/mosra/m.css.git" )
set(MCSS_VERSION_TAG "" )


## --- Define m.css to be checked out/updated while configuring

if (${CMAKE_VERSION} VERSION_LESS "3.11.0" ) ## No FetchContent module available


	## --- Define m.css as project

	include (ExternalProject )

	ExternalProject_Add(
		catch2
		GIT_REPOSITORY    ${MCSS_REPO_URL}
		GIT_TAG           ${MCSS_VERSION_TAG}
		GIT_SHALLOW       true
		GIT_PROGRESS      true
		TIMEOUT           10
		UPDATE_COMMAND    ${GIT_EXECUTABLE} pull
		CONFIGURE_COMMAND ""
		BUILD_COMMAND     ""
		INSTALL_COMMAND   ""
		LOG_DOWNLOAD      ON
	)

	ExternalProject_Get_Property(mcss SOURCE_DIR BINARY_DIR TMP_DIR )


	## --- Actually fetch m.css using the config created by ExternalProject_Add

	execute_process(
		COMMAND ${CMAKE_COMMAND} -P ${TMP_DIR}/catch2-gitclone.cmake
		WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	)


	## --- Load m.css CMake config

	#add_subdirectory (${SOURCE_DIR} ${BINARY_DIR} )

	unset(SOURCE_DIR )
	unset(BINARY_DIR )
	unset(TMP_DIR )

else()

	## --- Fetch m.css

	include (FetchContent )

	FetchContent_Declare(
		m.css
		GIT_REPOSITORY    ${MCSS_REPO_URL}
		GIT_TAG           ${MCSS_VERSION_TAG}
	)

	FetchContent_GetProperties (m.css )

	if (NOT m.css_POPULATED )

		FetchContent_Populate (m.css )

		## --- Load m.css CMake config

		#add_subdirectory (${m.css_SOURCE_DIR} ${m.css_BINARY_DIR} )

	endif()

endif()


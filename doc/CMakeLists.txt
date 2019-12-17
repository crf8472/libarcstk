## CMake file for building the documentation

cmake_minimum_required (VERSION 3.7 )


find_package (Doxygen 1.8.0 COMPONENTS REQUIRED dot )


message (STATUS
	"Note: to build documentation do 'cmake --build . --target doc'" )

## Construct comment for doc target
set (DOC_COMMENT "Build documentation for APIs" )

## Define sources for documentation
set (DOC_SOURCES ${EXPORTED_HEADERS} )

## Define list of sources for replacement in Doxyfile
string (REPLACE ";" " " DOC_SOURCE_FILES "${DOC_SOURCES}" )

## Path to import texts
set (PROJECT_DOC_TEXTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/texts" )

## We consider file index.html as main doxygen output and dependency
set (DOC_OUTPUT_MAINPAGE "${PROJECT_BUILD_DOC_DIR}/html/index.html" )

## Define the target name and location of the base Doxyfile
set (DOXYFILE_BASE "${PROJECT_BUILD_DOC_DIR}/Doxyfile-base" )

## Define Doxyfile to explicitly use when generating the documentation
set (DOXYFILE "${PROJECT_BUILD_DOC_DIR}/Doxyfile" )

## The following variables are CMAKE controlled options in the Doxyfile template
set (PROJECT_DOXYGEN_FILE_PATTERNS "*.hpp *.md" )
#set (PROJECT_DOXYGEN_INPUT "" )
#set (PROJECT_DOXYGEN_STRIP_FROM_PATH "" )
#set (PROJECT_DOXYGEN_STRIP_FROM_INC_PATH "" )


## A doc target makes it possible to just remove the folder 'html' in the
## output directory to force a rebuild of the documentation.
add_custom_target ( doc DEPENDS "${DOC_OUTPUT_MAINPAGE}" )
## Note: it would be nice to just have dependencies on the Doxyfile. Since this
## file is build in the configure phase, it can not simply be added as a
## dependency to a command.


## --- Build internal docs if requested

if (WITH_INTERNAL_DOCS)

	message (STATUS "Will build documentation also for internal APIs" )
	set (PROJECT_DOXYGEN_INTERNAL_DOCS "YES" )

	set(DOC_COMMENT "${DOC_COMMENT} (also internal APIs)" )

	set (DOC_SOURCES ${DOC_SOURCES} ${NONPUBLIC_HEADERS} )

else ()

	set (PROJECT_DOXYGEN_INTERNAL_DOCS "NO" )

endif (WITH_INTERNAL_DOCS)



## Provide base Doxyfile

configure_file ("${PROJECT_DOC_DIR}/Doxyfile.in"  "${DOXYFILE_BASE}"  @ONLY )


## Configure documentation target

if (USE_MCSS ) ## Use HTML5/CSS3 doxygen design from m.css

	if (WIN32 )

		message (WARNING "USE_MCSS is currently not available for Windows" )

	else () ## Everything except WIN32

		message (STATUS "Will build documentation with m.css" )

		set(DOC_COMMENT "${DOC_COMMENT}, using m.css" )

		add_subdirectory (thirdparty/m.css EXCLUDE_FROM_ALL )
		add_subdirectory (thirdparty/python-virtualenv )

		configure_file ("${DOXYFILE_TEMPLATE}" "${DOXYFILE}" @ONLY )

		## Provide debug output from m.css if requested
		if (MCSS_DEBUG )
			set (MCSS_FLAGS "--debug" )
		endif (MCSS_DEBUG )

		## Run m.css executable
		add_custom_command (
			OUTPUT  "${DOC_OUTPUT_MAINPAGE}"
			COMMAND "${PYTHON_CMD}" "${MCSS_CMD}" ${MCSS_FLAGS} "${DOXYFILE}"
			DEPENDS "${ENV_TARGET}" ${DOC_SOURCES} ## TODO: "${DOXYFILE}"
			WORKING_DIRECTORY "${ENV_DIRECTORY}"
			COMMENT "${DOC_COMMENT}"
			VERBATIM
		)

	endif ()

else() ## Use standard doxygen

	set (DOXYFILE "${PROJECT_BUILD_DOC_DIR}/Doxyfile" )

	add_custom_command (
		OUTPUT  "${DOC_OUTPUT_MAINPAGE}"
		COMMAND Doxygen::doxygen "${DOXYFILE}"
		DEPENDS ${DOC_SOURCES} ## TODO "${DOXYFILE}"
		WORKING_DIRECTORY "${PROJECT_BUILD_DOC_DIR}"
		COMMENT "${DOC_COMMENT}"
		VERBATIM
	)

endif (USE_MCSS )


## CMake file for building the documentation

cmake_minimum_required (VERSION 3.7 )


find_package (Doxygen 1.8.0 COMPONENTS REQUIRED dot )


message (STATUS
	"Note: to build documentation do 'cmake --build . --target doc'" )

## Construct comment for doc target
set (DOC_COMMENT "Build documentation for APIs" )

## Define sources for documentation
set (DOC_SOURCES ${EXPORTED_HEADERS} )

## Path to import texts
set (PROJECT_DOC_TEXTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/texts" )

## We consider file index.html as main doxygen output and dependency
set (DOC_OUTPUT_MAINPAGE "${PROJECT_BUILD_DOC_DIR}/html/index.html" )

set (PROJECT_DOXYGEN_FILE_PATTERNS "*.hpp *.md" )


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



## --- Build impl-only docs if requested (includes internal docs)

if (WITH_IMPL_DOCS)

	if (NOT WITH_INTERNAL_DOCS)

		## print message that has not been printed so far
		message (STATUS "Will build documentation also for internal APIs" )

	endif (NOT WITH_INTERNAL_DOCS)

	message (STATUS "Will build documentation also for implementations" )

	set(DOC_COMMENT "${DOC_COMMENT} and implementations" )

	set (DOC_SOURCES ${DOC_SOURCES}
		${NONPUBLIC_HEADERS} ${EXPORTED_TEMPLATES} ${SOURCES} )

	## Set replacement values for Doxyfile

	set (PROJECT_DOXYGEN_INTERNAL_DOCS "YES" )
	set (PROJECT_DOXYGEN_IMPL_DOCS     "YES" )

	set (PROJECT_DOXYGEN_FILE_PATTERNS
		"${PROJECT_DOXYGEN_FILE_PATTERNS} *.cpp *.tpp" )

else ()

	set (PROJECT_DOXYGEN_IMPL_DOCS "NO" )

endif (WITH_IMPL_DOCS)


## Provide main Doxyfile

configure_file ("${PROJECT_DOC_DIR}/Doxyfile.in"
	"${PROJECT_BUILD_DOC_DIR}/Doxyfile"
	@ONLY
)


## Configure documentation target

if (USE_MCSS ) ## Use HTML5/CSS3 doxygen design from m.css

	if (WIN32 )

		message (WARNING "USE_MCSS is currently not available for Windows" )

	else () ## Everything except WIN32

		message (STATUS "Will build documentation with m.css" )

		set(DOC_COMMENT "${DOC_COMMENT}, using m.css" )

		add_subdirectory (thirdparty/m.css EXCLUDE_FROM_ALL )
		add_subdirectory (thirdparty/python-virtualenv )

		set (DOXYFILE "${PROJECT_BUILD_DOC_DIR}/Doxyfile-mcss" )

		## Provide Doxyfile for m.css
		configure_file ("${PROJECT_DOC_DIR}/Doxyfile-mcss.in"
			"${DOXYFILE}"
			@ONLY
		)

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
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		COMMENT "${DOC_COMMENT}"
		VERBATIM
	)

endif (USE_MCSS )


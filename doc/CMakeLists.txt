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

set (PROJECT_DOXYGEN_FILE_PATTERNS "*.hpp *.md" )



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


## Create Main Doxyfile

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

		add_subdirectory (thirdparty/m.css )
		add_subdirectory (thirdparty/python-virtualenv )

		set (DOXYFILE "${PROJECT_BUILD_DOC_DIR}/Doxyfile-mcss" )

		## Provide Doxygen configuration for m.css
		configure_file ("${PROJECT_DOC_DIR}/Doxyfile-mcss.in"
			"${DOXYFILE}"
			@ONLY
		)

		## Provide debug output from m.css if requested
		if (MCSS_DEBUG )
			set (MCSS_FLAGS "--debug" )
		else (MCSS_DEBUG )
			set (MCSS_FLAGS )
		endif (MCSS_DEBUG )

		## Run m.css executable
		add_custom_target ( doc
			COMMAND "${PYTHON_CMD}" "${MCSS_CMD}" ${MCSS_FLAGS} "${DOXYFILE}"
			DEPENDS "${ENV_TARGET}" "${DOXYFILE}" ${DOC_SOURCES}
			WORKING_DIRECTORY "${ENV_DIRECTORY}"
			COMMENT "${DOC_COMMENT}"
			VERBATIM
		)

	endif ()

else() ## Use standard doxygen

	set (DOXYFILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" )

	add_custom_target ( doc ## add 'ALL' if you wish to run doc on regular build
		Doxygen::doxygen "${DOXYFILE}"
		DEPENDS "${DOXYFILE}" ${DOC_SOURCES}
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		COMMENT "${DOC_COMMENT}"
		VERBATIM
	)

endif (USE_MCSS )


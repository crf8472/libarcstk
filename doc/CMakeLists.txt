## CMake file for building the documentation

cmake_minimum_required (VERSION 3.7 )


find_package (Doxygen 1.8.0 COMPONENTS REQUIRED dot )

set (PROJECT_BUILD_DOC_DIR "${PROJECT_BINARY_DIR}/doc" )

set (PROJECT_DOXYGEN_FILE_PATTERNS "*.hpp *.hpp.in *.md" )

message (STATUS
	"Note: to build documentation do 'cmake --build . --target doc'" )

## Construct comment for doc target
set(DOC_COMMENT "Build documentation for APIs" )


## Build internal docs if requested

if (WITH_INTERNAL_DOCS)

	message (STATUS "Will build documentation also for internal APIs" )
	set (PROJECT_DOXYGEN_INTERNAL_DOCS "YES" )

	set(DOC_COMMENT "${DOC_COMMENT} (also internal APIs)" )

else ()

	set (PROJECT_DOXYGEN_INTERNAL_DOCS "NO" )

endif (WITH_INTERNAL_DOCS)


## Build impl-only docs if requested (includes internal docs)

if (WITH_IMPLONLY_DOCS)

	if (NOT WITH_INTERNAL_DOCS)

		## print message that has not been printed so far
		message (STATUS "Will build documentation also for internal APIs" )

	endif (NOT WITH_INTERNAL_DOCS)

	message (STATUS "Will build documentation also for implementations" )

	set(DOC_COMMENT "${DOC_COMMENT} and implementations" )

	## Set replacement values for Doxyfile

	set (PROJECT_DOXYGEN_INTERNAL_DOCS    "YES" )

	set (PROJECT_DOXYGEN_ENABLED_SECTIONS "IMPL_ONLY" )

	set (PROJECT_DOXYGEN_FILE_PATTERNS
		"${PROJECT_DOXYGEN_FILE_PATTERNS} *.cpp *.tpp" )

else ()

	set (PROJECT_DOXYGEN_ENABLED_SECTIONS "" )

endif (WITH_IMPLONLY_DOCS)


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

		set (DOXYFILE "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-mcss" )

		## Provide Doxygen configuration for m.css
		configure_file ("${PROJECT_DOC_DIR}/Doxyfile-mcss.in"
			"${PROJECT_BUILD_DOC_DIR}/Doxyfile-mcss"
			@ONLY
		)

		## Run dox2html5.py
		add_custom_target ( doc
			COMMAND ${PYTHON_CMD} ${MCSS_DOX2HTML5} "${DOXYFILE}"
			DEPENDS "${ENV_TARGET}" "${DOXYFILE}" "${PROJECT_SOURCE_DIR}"
			WORKING_DIRECTORY "${ENV_DIRECTORY}"
			COMMENT "${DOC_COMMENT}"
			VERBATIM
		)

	endif ()

else() ## Use standard doxygen

	add_custom_target ( doc ## add 'ALL' if you wish to run doc on regular build
		Doxygen::doxygen  "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile"
		WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
		COMMENT "${DOC_COMMENT}"
		VERBATIM
	)

endif (USE_MCSS )


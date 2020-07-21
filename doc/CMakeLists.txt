## CMake file for building the documentation

cmake_minimum_required (VERSION 3.7 )


find_package (Doxygen 1.8.0 ) #COMPONENTS REQUIRED dot )


message (STATUS
	"Note: to build documentation do 'cmake --build . --target doc'" )


## "Root directory for documentation config and sources" )
set (PROJECT_DOC_DIR      "${CMAKE_CURRENT_SOURCE_DIR}" )


## --- Organize subdirectories and build directories

## "Target directory for generated documentation"
set (DOC_GEN_DIR          "${PROJECT_BINARY_DIR}/generated-docs" )

## Target directory for 'doc' target (to be provided to subdirectory scripts)
set (DOC_BINARY_DIR       "${CMAKE_CURRENT_BINARY_DIR}" )

## "Target directory for configured documentation texts"
set (DOC_TEXTS_GEN_DIR    "${DOC_BINARY_DIR}/texts" )

## Path to import documentation texts
set (DOC_TEXTS_SOURCE_DIR "${PROJECT_DOC_DIR}/texts" )


## --- Organize documentation sources

## Construct comment for doc target
set (DOC_COMMENT          "Build documentation for APIs" )

## Deployed mainpage file
set (DOC_MAINPAGE         "${DOC_TEXTS_GEN_DIR}/MAINPAGE.md" )

## Define input for documentation
set (DOC_SOURCES           ${INTERFACE_HEADERS} ${DOC_MAINPAGE} )

## Define the target name and location of the base Doxyfile to be included
set (DOC_DOXYFILE_BASE    "${DOC_BINARY_DIR}/Doxyfile-base" )

## Define the target name and location of the base Doxyfile to be included
set (DOC_DOXYFILE_BASE_TEMPLATE "${PROJECT_DOC_DIR}/Doxyfile.in" )

## CMake controlled values in the Doxyfile base template
set (DOXYGEN_OUTPUT_DIRECTORY "${DOC_GEN_DIR}" )
string (REPLACE ";" " " DOXYGEN_STRIP_FROM_PATH
	"${PROJECT_INCLUDE_DIR}/;${PROJECT_BUILD_SOURCE_DIR}/" )
string (REPLACE ";" " " DOXYGEN_STRIP_FROM_INC_PATH
	"${PROJECT_INCLUDE_DIR}/;${PROJECT_BUILD_SOURCE_DIR}/" )
string (REPLACE ";" " " DOXYGEN_INPUT
	"${PROJECT_INCLUDE_DIR};${PROJECT_BUILD_SOURCE_DIR};${DOC_TEXTS_GEN_DIR}" )
set (DOXYGEN_FILE_PATTERNS "*.hpp *.md" )
set (DOXYGEN_USE_MDFILE_AS_MAINPAGE "${DOC_MAINPAGE}" )



## --- Generate documentation output directory

file (MAKE_DIRECTORY "${DOC_GEN_DIR}" )



## --- Build internal docs if requested

if (WITH_INTERNAL_DOCS)

	message (STATUS "Will build documentation also for internal APIs" )

	set (DOXYGEN_INTERNAL_DOCS "YES" )
	set (DOXYGEN_INPUT
		"${DOXYGEN_INPUT} ${PROJECT_SOURCE_DIR}" )
	set (DOXYGEN_STRIP_FROM_PATH
		"${DOXYGEN_STRIP_FROM_PATH} ${PROJECT_SOURCE_DIR}/" )
	set (DOXYGEN_STRIP_FROM_INC_PATH
		"${DOXYGEN_STRIP_FROM_INC_PATH} ${PROJECT_SOURCE_DIR}/" )

	set (DOC_COMMENT "${DOC_COMMENT} (also internal APIs)" )
	set (DOC_SOURCES ${DOC_SOURCES} ${PRIVATE_HEADERS} )

else ()

	set (DOXYGEN_INTERNAL_DOCS "NO" )

endif (WITH_INTERNAL_DOCS)



## --- Configure doxyfile and static texts

configure_file ("${DOC_DOXYFILE_BASE_TEMPLATE}" "${DOC_DOXYFILE_BASE}" @ONLY )

configure_file ("${DOC_TEXTS_SOURCE_DIR}/MAINPAGE.md" "${DOC_MAINPAGE}" @ONLY )



## --- Activate requested documentation targets

## Add default doc target.
## Each documentation config in thirdparty/ may add its own concrete dependency
## target to 'doc' and do the concrete work.
## As a fallback, we provide target 'doc_plain' that generates doxygen's stock
## HTML output that gives as this nice, cosy feeling of the Nineties.

add_custom_target (doc )


## --- If some thirdparty documentation tool was requested, use it

list (APPEND GENERATORS MCSS RTD )

foreach (GENERATOR IN ITEMS ${GENERATORS} )

	if (USE_${GENERATOR} )

		string (TOLOWER ${GENERATOR} GENERATOR_DIR )

		list (APPEND LOAD_GENERATORS "${GENERATOR_DIR}" )
	endif()

endforeach()
unset (GENERATOR )
unset (GENERATOR_DIR )


if (LOAD_GENERATORS )

	## Deactivate stock Doxygen HTML
	set (SKIP_DOXYGEN_HTML TRUE )

	## Configure virtual Python Sandbox
	add_subdirectory ("thirdparty/python-virtualenv" EXCLUDE_FROM_ALL )
	## Now, PYTHON_ENV_AVAILABLE is "YES"
endif()



## --- Detect Doxygen version with duplication bug and setup deduplication

if (DOXYGEN_VERSION VERSION_GREATER_EQUAL 1.8.16 )

	set (DOXYGEN_XML_NEEDS_DEDUP  "YES" )
endif()


if (DOXYGEN_XML_NEEDS_DEDUP AND LOAD_GENERATORS )

	# If no generators are to be used, we fall back to doxygen's stock html.
	# In this case, no deduplication is required!

	if (NOT PYTHON_ENV_AVAILABLE )

		message (WARNING "Doxygen needs python for deduplication script" )
	endif()

	set (DEDUP_INDEX_SCRIPT "${DOC_BINARY_DIR}/dedup_index.py" )

	configure_file (
		"${PROJECT_DOC_DIR}/thirdparty/doxygen/dedup_index.py"
		"${DEDUP_INDEX_SCRIPT}"
		@ONLY
	)

	add_requirements_target (doxygen_requirements
		"${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/doxygen/requirements.txt"
	)

	set (DOXYGEN_PREFERS_SEPARATE "YES" ) ## To be checked in subdir cmakes
endif()


macro (add_doxygen_dedup DOXY_PRIMARY_TARGET XMLFILE )

	set (STAMP_FILE "${XMLFILE}.deduplicated" )

	add_custom_command (
		OUTPUT  "${STAMP_FILE}"
		COMMAND "${PYTHON_CMD}" "${DEDUP_INDEX_SCRIPT}" "${XMLFILE}"
		COMMAND ${CMAKE_COMMAND}
		ARGS    -E touch "${STAMP_FILE}"
		DEPENDS "${XMLFILE}"
		WORKING_DIRECTORY "${DOC_GEN_DIR}"
		COMMENT "Eliminate duplicate compounds in doxygen XML for target ${DOXY_PRIMARY_TARGET}"
		VERBATIM
	)

	set (DEDUP_TARGET "${DOXY_PRIMARY_TARGET}_dedup" )

	message (STATUS "Add doxygen dedup target ${DEDUP_TARGET}" )

	add_custom_target (${DEDUP_TARGET} DEPENDS "${STAMP_FILE}" )

	add_dependencies  (${DEDUP_TARGET} doxygen_requirements )

	add_dependencies  (${DEDUP_TARGET} ${DOXY_PRIMARY_TARGET} )

	unset (DOXY_PRIMARY_TARGET )
	unset (DEDUP_TARGET )
	unset (XMLFILE )
endmacro()



## --- Provide macro for subdir CMake configs to request doxygen runs

macro (add_doxygen_target DOXY_TARGET DOXYFILE DOXYTEMPLATE INDEXFILE )

	add_custom_command (
		OUTPUT  "${INDEXFILE}"
		COMMAND Doxygen::doxygen "${DOXYFILE}"
		DEPENDS "${DOC_DOXYFILE_BASE_TEMPLATE}"
				"${DOC_DOXYFILE_BASE}"
				"${DOXYTEMPLATE}"
				"${DOXYFILE}"
				${DOC_SOURCES}
		WORKING_DIRECTORY "${DOC_GEN_DIR}"
		COMMENT "Run doxygen for target ${DOXY_TARGET}"
		VERBATIM
	)

	add_custom_target (${DOXY_TARGET} )

	## Handle possible deduplication requirement
	if (DOXYGEN_XML_NEEDS_DEDUP AND LOAD_GENERATORS )

		set (DOXY_RUN_TARGET "${DOXY_TARGET}_run" )

		add_custom_target (${DOXY_RUN_TARGET} DEPENDS "${INDEXFILE}" )

		add_doxygen_dedup (${DOXY_RUN_TARGET} "${INDEXFILE}" )

		add_dependencies  (${DOXY_TARGET}     "${DOXY_RUN_TARGET}_dedup" )
	endif()
endmacro()


foreach (GENERATOR IN ITEMS ${LOAD_GENERATORS} )

	## Subdir CMake can attach target "python-virtualenv" as requirement
	add_subdirectory ("thirdparty/${GENERATOR}" EXCLUDE_FROM_ALL )
endforeach()



## --- If no variant is requested, use standard doxygen for target 'doc'

if (NOT SKIP_DOXYGEN_HTML )

	message (STATUS "Will build documentation with pure doxygen (HTML)" )

	set (DOC_OUTPUT_MAINPAGE "${DOC_GEN_DIR}/html/index.html" )

	add_doxygen_target (plain_doxygen "${DOC_DOXYFILE_BASE}"
		"${DOC_DOXYFILE_BASE_TEMPLATE}"
		"${DOC_OUTPUT_MAINPAGE}"
	) ## This will produce HTML but call deduplication needlessly

	add_custom_target (plain_doc DEPENDS "${DOC_OUTPUT_MAINPAGE}" )
	add_dependencies  (plain_doc plain_doxygen )

	add_dependencies  (doc plain_doc )

endif()


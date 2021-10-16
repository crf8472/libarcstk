## CMake file for building the documentation

cmake_minimum_required (VERSION 3.12 )


find_package (Doxygen 1.8.0 )


message (STATUS
	"Note: to build documentation do 'cmake --build . --target doc'" )


## "Root directory for documentation config and sources" )
set (PROJECT_DOC_DIR      ${CMAKE_CURRENT_SOURCE_DIR} )


## --- Organize subdirectories and build directories

## "Target directory for generated documentation"
set (DOC_GEN_DIR          ${PROJECT_BINARY_DIR}/generated-docs )

## Target directory for 'doc' target (to be provided to subdirectory scripts)
set (DOC_BINARY_DIR       ${CMAKE_CURRENT_BINARY_DIR} )

## "Target directory for configured documentation texts"
set (DOC_TEXTS_GEN_DIR    ${DOC_BINARY_DIR}/texts )

## Path to import documentation texts
set (DOC_TEXTS_SOURCE_DIR ${PROJECT_DOC_DIR}/texts )


## --- Organize documentation sources

## Construct comment for doc target
set (DOC_COMMENT          "Build documentation for APIs" )

## Deployed mainpage file
set (DOC_MAINPAGE         ${DOC_TEXTS_GEN_DIR}/MAINPAGE.md )

## Define input for documentation
set (DOC_SOURCES          ${INTERFACE_HEADERS} ${DOC_MAINPAGE} )

## Define the target name and location of the base Doxyfile to be included
set (DOC_DOXYFILE_BASE    ${DOC_BINARY_DIR}/Doxyfile-base )

## Define the target name and location of the base Doxyfile to be included
set (DOC_DOXYFILE_BASE_TEMPLATE ${PROJECT_DOC_DIR}/Doxyfile.in )



## --- Generate documentation output directory

add_custom_command (
	OUTPUT  ${DOC_GEN_DIR}
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND} -E make_directory ${DOC_GEN_DIR} )


add_custom_target (create_output_dir DEPENDS ${DOC_GEN_DIR} )


## --- Generate base doxyfile

## Set CMake controlled values in the Doxyfile base template

set (DOXYGEN_OUTPUT_DIRECTORY       "${DOC_GEN_DIR}/doxygen" )
set (DOXYGEN_FILE_PATTERNS          "*.hpp *.md" )
set (DOXYGEN_USE_MDFILE_AS_MAINPAGE "${DOC_MAINPAGE}" )


## Build internal docs if requested

if (WITH_INTERNAL_DOCS)

	message (STATUS "Will build documentation also for internal APIs" )

	set (DOXYGEN_STRIP_FROM_PATH
		"${DOXYGEN_STRIP_FROM_PATH} ${PROJECT_SOURCE_DIR}/" )

	set (DOXYGEN_STRIP_FROM_INC_PATH
		"${DOXYGEN_STRIP_FROM_INC_PATH} ${PROJECT_SOURCE_DIR}/" )

	set (DOXYGEN_INTERNAL_DOCS "YES" )

	set (DOXYGEN_INPUT "${DOXYGEN_INPUT} ${PROJECT_SOURCE_DIR}" )

	## Not passed to doxygen, cmake only
	set (DOC_SOURCES ${DOC_SOURCES} ${PRIVATE_HEADERS} )

	set (DOC_COMMENT "${DOC_COMMENT} (also internal APIs)" )

else ()

	string (REPLACE ";" " " DOXYGEN_STRIP_FROM_PATH
		"${PROJECT_INCLUDE_DIR}/;${PROJECT_BUILD_SOURCE_DIR}/" )

	string (REPLACE ";" " " DOXYGEN_STRIP_FROM_INC_PATH
		"${PROJECT_INCLUDE_DIR}/;${PROJECT_BUILD_SOURCE_DIR}/" )

	set (DOXYGEN_INTERNAL_DOCS "NO" )

	string (REPLACE ";" " " DOXYGEN_INPUT
		"${PROJECT_INCLUDE_DIR};${PROJECT_BUILD_SOURCE_DIR};${DOC_TEXTS_GEN_DIR}" )

endif (WITH_INTERNAL_DOCS)


## Add rule to build base Doxyfile

#configure_file ( ${DOC_DOXYFILE_BASE_TEMPLATE} ${DOC_DOXYFILE_BASE} @ONLY )

## Recreate base Doxyfile at buildtime.
add_custom_command (
	OUTPUT ${DOC_DOXYFILE_BASE}
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND}
		-D PROJECT_NAME=${PROJECT_NAME}
		-D PROJECT_VERSION_STRING=${PROJECT_VERSION_STRING}
		-D PROJECT_BRIEF_DESC=${PROJECT_BRIEF_DESC}
		-D DOXYGEN_OUTPUT_DIRECTORY=${DOXYGEN_OUTPUT_DIRECTORY}
		-D DOXYGEN_STRIP_FROM_PATH=${DOXYGEN_STRIP_FROM_PATH}
		-D DOXYGEN_STRIP_FROM_INC_PATH=${DOXYGEN_STRIP_FROM_INC_PATH}
		-D DOXYGEN_INTERNAL_DOCS=${DOXYGEN_INTERNAL_DOCS}
		-D PROJECT_LOG_DIR=${PROJECT_LOG_DIR}
		-D DOXYGEN_INPUT=${DOXYGEN_INPUT}
		-D DOXYGEN_FILE_PATTERNS=${DOXYGEN_FILE_PATTERNS}
		-D DOXYGEN_USE_MDFILE_AS_MAINPAGE=${DOXYGEN_USE_MDFILE_AS_MAINPAGE}
		-D _INFILE=${DOC_DOXYFILE_BASE_TEMPLATE}
		-D _OUTFILE=${DOC_DOXYFILE_BASE}
		-P ${PROJECT_CMAKE_DIR}/configure_file.cmake
	DEPENDS ${DOC_DOXYFILE_BASE_TEMPLATE} )



## --- Generate static mainpage

add_custom_command (
	OUTPUT ${DOC_MAINPAGE}
	PRE_BUILD
	COMMAND ${CMAKE_COMMAND}
		-D DOC_TEXTS_SOURCE_DIR=${DOC_TEXTS_SOURCE_DIR}
		-D DOC_TEXTS_GEN_DIR=${DOC_TEXTS_GEN_DIR}
		-P ${PROJECT_CMAKE_DIR}/update_mainpagemd.cmake
	DEPENDS ${DOC_TEXTS_SOURCE_DIR}/MAINPAGE.md.in )



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


## --- Detect Doxygen version with duplication bug and setup deduplication

if (LOAD_GENERATORS AND DOXYGEN_VERSION VERSION_GREATER_EQUAL 1.8.16 )

	## Indicates that 1. XML is requested and 2. XML requires deduplication
	set (DOXYGEN_NEEDS_XML_DEDUP TRUE ) ## Use this in subdirectories!
endif()


## DOXYGEN_NEEDS_XML_DEDUP must be set before actually loading the
## CMAKE scripts for generators

if (LOAD_GENERATORS )

	## --- Target summing up all prequisites
	##
	## When adding a generator target in a subdirectory, add THIS target as a
	## dependency to it as it is done in doc/thirdparty/mcss/CMakeLists.txt.
	add_custom_target (doc_prerequisites
		DEPENDS ${DOC_SOURCES} ${DOC_DOXYFILE_BASE} )
	add_dependencies (doc_prerequisites create_output_dir )

	## Configure virtual Python Sandbox
	add_subdirectory ("thirdparty/python-virtualenv" EXCLUDE_FROM_ALL )
	## Now, PYTHON_ENV_AVAILABLE is "YES"
endif()



if (DOXYGEN_NEEDS_XML_DEDUP )

	message (STATUS "Doxygen version requires index deduplication" )

	## Setup deduplication script. We don't know whether it is actually
	## used, but it's in place.

	set (DEDUP_INDEX_SCRIPT "${DOC_BINARY_DIR}/dedup_index.py" )

	configure_file (
		"${PROJECT_DOC_DIR}/thirdparty/doxygen/dedup_index.py"
		"${DEDUP_INDEX_SCRIPT}"
		@ONLY
	)

	## In either case, we need python for deduplication. Either a virtualenv is
	## setup for a generator (like m.css) or we try to use the systemwide
	## python.

	if (PYTHON_ENV_AVAILABLE )

		add_requirements_target (doxygen_requirements
			"${CMAKE_CURRENT_SOURCE_DIR}/thirdparty/doxygen/requirements.txt"
		)

	else()

		find_package (Python3 COMPONENTS Interpreter Development REQUIRED )

		set (PYTHON_CMD )

	endif (PYTHON_ENV_AVAILABLE )
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

	if (PYTHON_ENV_AVAILABLE )

		add_dependencies  (${DEDUP_TARGET} doxygen_requirements )
	endif()

	add_dependencies  (${DEDUP_TARGET} ${DOXY_PRIMARY_TARGET} )

	unset (DOXY_PRIMARY_TARGET )
	unset (DEDUP_TARGET )
	unset (XMLFILE )
endmacro()



## --- Provide macro for subdir CMake configs to request doxygen runs

macro (add_doxygen_target DOXY_TARGET DOXYFILE INDEXFILE )

	add_custom_command (
		OUTPUT  ${INDEXFILE}
		COMMAND Doxygen::doxygen ${DOXYFILE}
		DEPENDS ${DOC_DOXYFILE_BASE} ${DOXYFILE} ${DOC_SOURCES}
		WORKING_DIRECTORY ${DOC_GEN_DIR}
		COMMENT "Run doxygen for target ${DOXY_TARGET}"
		VERBATIM
	)

	## Handle possible deduplication requirement
	if (DOXYGEN_NEEDS_XML_DEDUP )

		add_custom_target (${DOXY_TARGET}_run DEPENDS ${INDEXFILE} )

		add_doxygen_dedup (${DOXY_TARGET}_run ${INDEXFILE} )


		add_custom_target (${DOXY_TARGET} )

		add_dependencies  (${DOXY_TARGET}     ${DOXY_TARGET}_run_dedup )
	else()

		add_custom_target (${DOXY_TARGET} DEPENDS ${INDEXFILE} )
	endif()
endmacro()


foreach (GENERATOR IN ITEMS ${LOAD_GENERATORS} )

	## Subdir CMake can attach target "python-virtualenv" as requirement
	add_subdirectory ("thirdparty/${GENERATOR}" EXCLUDE_FROM_ALL )
endforeach()



## --- If no generator is requested, target 'doc' just produces stock HTML

if (NOT LOAD_GENERATORS )

	message (STATUS "Will build documentation with pure doxygen (HTML)" )

	set (DOC_OUTPUT_MAINPAGE ${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html )

	add_custom_command (
		OUTPUT  ${DOC_OUTPUT_MAINPAGE}
		COMMAND Doxygen::doxygen ${DOC_DOXYFILE_BASE}
		DEPENDS ${DOC_DOXYFILE_BASE} ${DOC_SOURCES}
		WORKING_DIRECTORY ${DOC_GEN_DIR}
		COMMENT "Run doxygen to create HTML"
		VERBATIM
	)

	add_custom_target (plain_doc DEPENDS ${DOC_OUTPUT_MAINPAGE} )
	add_dependencies (plain_doc create_output_dir )

	add_dependencies  (doc plain_doc )

endif()


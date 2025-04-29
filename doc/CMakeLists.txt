## CMake file for building the documentation
## vim:fdm=marker

cmake_minimum_required (VERSION 3.12 )
## --- Policies {{{1

## Support IN_LIST if() operator required by FindDoxygen.cmake
cmake_policy (SET CMP0057 NEW )
## 1}}}

find_package (Doxygen )
#find_package (Doxygen 1.9.1...<1.9.7 ) # 1.9.7 crashes m.css
#find_package (Doxygen 1.8.15...1.8.20 ) # OK too

add_custom_target (doc )

## --- Some Variables {{{1

## Target directory for generated documentation
set (DOC_GEN_DIR          ${PROJECT_BINARY_DIR}/generated-docs )

## Construct comment for doc target
set (DOC_COMMENT          "Build documentation for APIs" )

## Define input for documentation
set (DOC_SOURCES          ${INTERFACE_HEADERS} )

## Define the target name and location of the base Doxyfile to be included
set (DOC_DOXYFILE_BASE_TEMPLATE ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in )

## Define the target name and location of the base Doxyfile to be included
set (DOC_DOXYFILE_BASE    ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile-base )



## --- Add Default 'doc' Target {{{1

add_custom_target (libarcstk_doc )



## --- Generate Documentation Output Directory {{{1

add_custom_command (
	OUTPUT  ${DOC_GEN_DIR}
	COMMAND ${CMAKE_COMMAND}
	ARGS    -E make_directory ${DOC_GEN_DIR} )

add_custom_target (libarcstk_create_output_dir DEPENDS ${DOC_GEN_DIR} )



## --- Define Variables for Doxygen {{{1

## Set CMake controlled values in the Doxyfile base template.
## Doxygen may *only* use theses variables prefixed by "DOXYGEN_" and
## those from the root script prefixed by "PROJECT_".

set (DOXYGEN_OUTPUT_DIRECTORY       "${DOC_GEN_DIR}/doxygen" )
set (DOXYGEN_FILE_PATTERNS          "*.hpp *.md" )
set (DOXYGEN_STRIP_FROM_INC_PATH    "${PROJECT_INCLUDE_SOURCE_DIR}/" )
set (DOXYGEN_STRIP_FROM_PATH        "${PROJECT_INCLUDE_SOURCE_DIR}/" )
set (DOXYGEN_INPUT                  "${PROJECT_INCLUDE_SOURCE_DIR}/" )



## --- Provide Texts for Static Pages {{{1

add_subdirectory (texts ) ## Will update DOXYGEN_* variables or add more



## --- Setup Building Internal Docs if Requested {{{1

if (WITH_INTERNAL_DOCS )

	message (STATUS "Will build documentation also for internal APIs" )

	## passed to doxygen

	set (DOXYGEN_INTERNAL_DOCS "YES" )

	string (REPLACE ";" " " DOXYGEN_INPUT
		"${DOXYGEN_INPUT};${PROJECT_SOURCE_DIR}/;${PROJECT_GEN_SOURCES_DIR}/" )

	string (REPLACE ";" " " DOXYGEN_STRIP_FROM_INC_PATH
		"${DOXYGEN_STRIP_FROM_INC_PATH}/;${PROJECT_SOURCE_DIR}/;${PROJECT_GEN_SOURCES_DIR}/" )

	string (REPLACE ";" " " DOXYGEN_STRIP_FROM_PATH
		"${DOXYGEN_STRIP_FROM_PATH}/;${PROJECT_SOURCE_DIR}/;${PROJECT_GEN_SOURCES_DIR}/" )

	# commented out, no list of private headers available
	#set (DOC_SOURCES ${DOC_SOURCES} ${PRIVATE_HEADERS} )

	## not passed to doxygen, cmake only

	set (DOC_COMMENT "${DOC_COMMENT} (also internal APIs)" )

else ()

	set (DOXYGEN_INTERNAL_DOCS "NO" )

endif (WITH_INTERNAL_DOCS)



## --- Configure Doxyfile-base {{{1

## Normalize DOXYGEN_INPUT list
string (REPLACE ";" " " DOXYGEN_INPUT "${DOXYGEN_INPUT}/" )

configure_file ( ${DOC_DOXYFILE_BASE_TEMPLATE} ${DOC_DOXYFILE_BASE} @ONLY )



## --- Populate List of Thirdparty Documentation Tools Requested {{{1

get_filename_component (_fullpath "${CMAKE_CURRENT_SOURCE_DIR}" REALPATH )

foreach (_generator IN LISTS USE_DOC_TOOL )

	string (TOLOWER ${_generator} _generator_dir )

	message (STATUS "Check path ${_fullpath}/thirdparty/${_generator_dir}")

	if (EXISTS "${_fullpath}/thirdparty/${_generator_dir}" )

		message (STATUS "Generate documentation by ${_generator_dir}" )

		list (APPEND LOAD_GENERATORS "${_generator_dir}" )
	endif()

	unset (_generator_dir )
endforeach()
unset (_fullpath  )
unset (_generator )






## --- Configure Python Sandbox Used by Some Generators {{{1

if (LOAD_GENERATORS )

	## Configure virtual Python Sandbox
	add_subdirectory (thirdparty/python-virtualenv EXCLUDE_FROM_ALL )
	## Now, PYTHON_ENV_AVAILABLE is "YES"
endif()



## --- Setup Deduplication for Doxygen's XML if Required {{{1

## Variable DOXYGEN_NEEDS_XML_DEDUP can be used in subdirectory scopes.

## DOXYGEN_NEEDS_XML_DEDUP indicates that:
##   - XML output is requested (hence at least one thirdparty tool is used)
##   - XML output requires post processing
##
## Note: DOXYGEN_NEEDS_XML_DEDUP must be set _before_ actually loading the
## CMAKE scripts for generators with add_subdirectory()!

if (LOAD_GENERATORS AND DOXYGEN_VERSION VERSION_GREATER_EQUAL 1.8.16 )

	## The "duplication bug" was introduced in 1.8.16

	set (DOXYGEN_NEEDS_XML_DEDUP TRUE ) ## Use this in subdirectories!
endif()


if (DOXYGEN_NEEDS_XML_DEDUP )

	add_subdirectory (thirdparty/doxygen-dedup EXCLUDE_FROM_ALL )

else()

	if (NOT LOAD_GENERATORS )

		message (STATUS "No index deduplication since no generators requested" )
	else()

		message (STATUS "Doxygen version does not require index deduplication" )
	endif()
endif()



## --- Provide Macro for Subdir CMake Scripts to Request Doxygen Runs {{{1

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

		## Setup doxygen run target
		add_custom_target (${DOXY_TARGET}_run DEPENDS ${INDEXFILE} )
		add_dependencies  (${DOXY_TARGET}_run
			libarcstk_create_log_dir
			libarcstk_create_output_dir
			libarcstk_static_pages_files
		)

		set (STAMP_FILE ${INDEXFILE}.deduplicated )

		add_custom_command (
			OUTPUT  ${STAMP_FILE}
			COMMAND ${PYTHON_CMD}
			ARGS    ${DOXYGEN_XML_DEDUP_SCRIPT} ${INDEXFILE}
			COMMAND ${CMAKE_COMMAND}
			ARGS    -E touch ${STAMP_FILE}
			DEPENDS ${INDEXFILE}
			WORKING_DIRECTORY "${DOC_GEN_DIR}"
			COMMENT "Remove duplicate compounds in ${INDEXFILE}"
			VERBATIM
		)

		## Setup deduplication target
		add_custom_target (${DOXY_TARGET}_dedup DEPENDS ${STAMP_FILE} )
		add_dependencies  (${DOXY_TARGET}_dedup ${DOXY_TARGET}_run )
		if (PYTHON_ENV_AVAILABLE )

			add_dependencies  (${DOXY_TARGET}_dedup
				libarcstk_doxygen-dedup_requirements )
		endif()

		## Setup doxygen target
		add_custom_target (${DOXY_TARGET} )
		add_dependencies  (${DOXY_TARGET} ${DOXY_TARGET}_dedup )

		unset (STAMPFILE )
	else()

		add_custom_target (${DOXY_TARGET} DEPENDS ${INDEXFILE} )
		add_dependencies  (${DOXY_TARGET}
			libarcstk_create_log_dir
			libarcstk_create_output_dir
			libarcstk_static_pages_files
		)
	endif()
endmacro()



## --- Actually Load Generators {{{1

foreach (_generator IN ITEMS ${LOAD_GENERATORS} )

	## Subdir CMake can attach target "python-virtualenv" as requirement
	add_subdirectory ("thirdparty/${_generator}" EXCLUDE_FROM_ALL )
endforeach()
unset (_generator )



## --- Define libarcstk_doxygen_create_html for Doxygen Stock HTML {{{1

## If no generator is requested, just produce stock HTML

if (NOT LOAD_GENERATORS )

	message (STATUS "Will build documentation with pure doxygen (HTML)" )


	## -- Target for output mainpage

	set (_index_html ${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html )

	add_custom_target (libarcstk_doxygen_create_html DEPENDS ${_index_html} )


	## -- Load stylesheets if any

	## Fallback in case subdirectory does not overwrite it
	set (DOC_DOXYFILE_INPUT ${DOC_DOXYFILE_BASE} )

	## Comment or uncomment this line to use a alternative layouts to the
	## doxygen stock HTML layout. The subscope may re-set DOC_DOXYFILE_INPUT
	## and add dependencies to target 'libarcstk_doxygen_create_html'.
	#add_subdirectory ("thirdparty/doxygen-awesome-css" )

	message (STATUS "Use doxygen file: ${DOC_DOXYFILE_INPUT}" )


	## -- Add commands + organize targets

	add_custom_command (
		OUTPUT  ${_index_html}
		COMMAND Doxygen::doxygen ${DOC_DOXYFILE_INPUT}
		DEPENDS ${DOC_DOXYFILE_INPUT} ${DOC_SOURCES}
		WORKING_DIRECTORY ${DOC_GEN_DIR}
		COMMENT "Run doxygen to create HTML"
		VERBATIM
	)

	add_dependencies  (libarcstk_doxygen_create_html
							libarcstk_create_log_dir
							libarcstk_create_output_dir )

	add_dependencies  (libarcstk_doc libarcstk_doxygen_create_html )

endif()

## 1}}}

add_dependencies (doc libarcstk_create_log_dir libarcstk_doc )


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



## --- Setup Building Internal Docs if Requested (TODO broken) {{{1

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



## --- Declare doc targets for HTML {{{1

add_custom_target (libarcstk_doc_generate_html )
add_dependencies  (libarcstk_doc libarcstk_doc_generate_html )



## --- Provide Macro for Subdir CMake Scripts to Request Doxygen Runs {{{1
## Note that this target can produce XML as well as HTML, depends on Doxyfile.

macro (add_doxygen_target DOXY_TARGET DOXYFILE INDEXFILE )

	add_custom_target (${DOXY_TARGET} DEPENDS ${INDEXFILE} )

	add_dependencies  (${DOXY_TARGET}
			libarcstk_create_log_dir
			libarcstk_create_output_dir
			libarcstk_static_pages_files
	)

	## Produce INDEXFILE (either XML or HTML) by running doxygen
	add_custom_command (
		OUTPUT  ${INDEXFILE}
		DEPENDS ${DOC_DOXYFILE_BASE} ${DOXYFILE} ${DOC_SOURCES}
		COMMAND Doxygen::doxygen
		ARGS    ${DOXYFILE}
		WORKING_DIRECTORY ${DOC_GEN_DIR}
		COMMENT "Run doxygen for target ${DOXY_TARGET}"
		VERBATIM
	)
endmacro()



## --- Configure Python Sandbox Used by Some Generators {{{1

if (LOAD_GENERATORS )

	## Configure virtual Python Sandbox
	add_subdirectory (thirdparty/python-virtualenv EXCLUDE_FROM_ALL )
	## Now, PYTHON_ENV_AVAILABLE is "YES"

	## Each generator requires XML
	add_subdirectory (doxygen-xml )
	## Now, DOXYGEN_XML_DEDUP_INDEX_REQUIRED is either "YES" or "NO"
endif()


## --- Actually Load Generators {{{1

foreach (_generator IN ITEMS ${LOAD_GENERATORS} )

	## Subdir CMake can attach target "python-virtualenv" as requirement
	add_subdirectory ("thirdparty/${_generator}" EXCLUDE_FROM_ALL )
endforeach()
unset (_generator )



## --- Define libarcstk_doxygen_generate_html for Doxygen Stock HTML {{{1

## If no generator is requested, just produce stock HTML

if (NOT LOAD_GENERATORS )

	message (STATUS "Will build documentation with pure doxygen (HTML)" )

	## -- Load stylesheets if any

	## Fallback in case subdirectory does not overwrite it
	set (DOC_DOXYFILE_INPUT ${DOC_DOXYFILE_BASE} )

	## Comment or uncomment this line to use a alternative layouts to the
	## doxygen stock HTML layout. The subscope may re-set DOC_DOXYFILE_INPUT
	## and add dependencies to target 'libarcstk_doxygen_generate_html'.
	#add_subdirectory ("thirdparty/doxygen-awesome-css" )

	message (STATUS "Use doxygen file: ${DOC_DOXYFILE_INPUT}" )


	## -- Add commands + organize targets

	set (_index_html ${DOXYGEN_OUTPUT_DIRECTORY}/html/index.html )

	add_custom_command (
		OUTPUT  ${_index_html}
		DEPENDS ${DOC_DOXYFILE_INPUT} ${DOC_SOURCES}
		COMMAND Doxygen::doxygen ${DOC_DOXYFILE_INPUT}
		WORKING_DIRECTORY ${DOC_GEN_DIR}
		COMMENT "Run doxygen to create HTML"
		VERBATIM
	)

	add_custom_target (libarcstk_doxygen_generate_html DEPENDS ${_index_html} )

	add_dependencies  (libarcstk_doxygen_generate_html
							libarcstk_create_log_dir
							libarcstk_create_output_dir )

	add_dependencies  (libarcstk_doc_generate_html
							libarcstk_doxygen_generate_html )

endif()

## 1}}}

add_dependencies (doc libarcstk_create_log_dir libarcstk_doc )


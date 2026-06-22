## CMake script for providing the main page and all static page files
##
## vim:fdm=marker
##
## Provides:
##
## - Target: libarcstk_static_pages_files
## - Variables: DOXYGEN_USE_MDFILE_AS_MAINPAGE, DOXYGEN_INPUT

cmake_minimum_required (VERSION 3.12 )

add_custom_target (libarcstk_static_pages_files )
add_dependencies  (libarcstk_static_pages_files libarcstk_create_output_dir )


## --- Configure Static Mainpage {{{1

set (DOC_TEXTS_MAINPAGE        "${CMAKE_CURRENT_BINARY_DIR}/MAINPAGE.md" )

libarcstk_git_get_commit_id (LIBARCSTK_GIT_COMMIT_ID )

configure_file (MAINPAGE.md.in "${DOC_TEXTS_MAINPAGE}" @ONLY )



## --- Provide Text Files from Project Root Folder {{{1

foreach (FILE IN ITEMS "BUILD" ) # TODO Add "CODING"

	set (DOC_SOURCES  ${DOC_SOURCES} "${LIBARCSTK_ROOT_DIR}/${FILE}.md" )

	add_custom_command(
		TARGET libarcstk_static_pages_files PRE_BUILD
		COMMAND "${CMAKE_COMMAND}"
		ARGS -E copy
			"${LIBARCSTK_ROOT_DIR}/${FILE}.md"
			"${CMAKE_CURRENT_BINARY_DIR}/${FILE}.md"
		VERBATIM
	)
endforeach()


## --- Provide Static Pages {{{1

# TODO Write and include page "Accuraterip_algorithms"
foreach (FILE IN ITEMS "How_to_calculate.md" "How_to_verify.md"
		"How_to_get_ids.md" "How_to_parse_DBARs.md" "snippets.dox" )

	set (DOC_SOURCES  ${DOC_SOURCES} "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}" )
	string (REPLACE "." "_" PAGE_TITLE ${FILE})

	add_custom_target(libarcstk_provide_page_${PAGE_TITLE}
		DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/${FILE}"
		COMMAND "${CMAKE_COMMAND}" -E copy
			"${CMAKE_CURRENT_SOURCE_DIR}/${FILE}"
			"${CMAKE_CURRENT_BINARY_DIR}/${FILE}"
		COMMENT "Provide static page file ${FILE}"
		VERBATIM
	)

	add_dependencies (libarcstk_static_pages_files
		libarcstk_provide_page_${PAGE_TITLE} )
endforeach()

set (DOC_SOURCES ${DOC_SOURCES} "${DOC_TEXTS_MAINPAGE}" )



## --- Update Doxygen Configuration Variables {{{1

set (DOXYGEN_USE_MDFILE_AS_MAINPAGE "${DOC_TEXTS_MAINPAGE}" PARENT_SCOPE )

set (DOXYGEN_FILE_PATTERNS          "${DOXYGEN_FILE_PATTERNS} *.md *.dox"
	PARENT_SCOPE )
set (DOXYGEN_INPUT "${DOXYGEN_INPUT} ${CMAKE_CURRENT_BINARY_DIR}/"
	PARENT_SCOPE )

## 1}}}


## CMake
## vim:fdm=marker
##
## Provides:
##
## - Target: libarcstk_static_pages_files


set (DOC_TEXTS_MAINPAGE        "${CMAKE_CURRENT_BINARY_DIR}/MAINPAGE.md" )


## --- Provide Static Mainpage

configure_file (MAINPAGE.md.in ${DOC_TEXTS_MAINPAGE} @ONLY )


add_custom_target (libarcstk_static_pages_files )
add_dependencies  (libarcstk_static_pages_files libarcstk_create_output_dir )


## --- Provide Markdown Files from Project Root Folder {{{1

foreach (FILE IN ITEMS "BUILD" "CODING" )

	add_custom_command(
		TARGET libarcstk_static_pages_files PRE_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS -E copy
			"${CMAKE_SOURCE_DIR}/${FILE}.md"
			"${CMAKE_CURRENT_BINARY_DIR}/${FILE}.md" )
endforeach()


## --- Provide Static Pages {{{1

foreach (FILE IN ITEMS "Accuraterip_algorithms" )

	add_custom_command(
		TARGET libarcstk_static_pages_files PRE_BUILD
		COMMAND ${CMAKE_COMMAND}
		ARGS -E copy
			"${CMAKE_CURRENT_SOURCE_DIR}/${FILE}.md"
			"${CMAKE_CURRENT_BINARY_DIR}/${FILE}.md"
	)
endforeach()



## --- Add Static Page Folder to DOXYGEN_INPUT {{{1

set (DOXYGEN_USE_MDFILE_AS_MAINPAGE
	"${DOC_TEXTS_MAINPAGE}" PARENT_SCOPE )

set (DOXYGEN_INPUT
	"${DOXYGEN_INPUT} ${CMAKE_CURRENT_BINARY_DIR}" PARENT_SCOPE )

set (DOC_SOURCES
	${DOC_SOURCES} ${DOC_TEXTS_MAINPAGE} PARENT_SCOPE )

## 1}}}


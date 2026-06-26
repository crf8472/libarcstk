## libarcstk: CMake functions for drawing an include graph
## vim:fdm=marker

cmake_minimum_required (VERSION 3.18 )

## Enable include-dependency graph functionality
function (libarcstk_enable_clang_include_graph OUT_VAR ) # {{{1

	set (${OUT_VAR} FALSE PARENT_SCOPE )

	set (_options        KEEP_DOT_FILES )
	set (_one_value_args FORMAT OUTPUT_DIR )

	cmake_parse_arguments (GRAPH
		"${options}" "${one_value_args}" "" ${ARGN} )

	## Find clang-include-graph

	find_program (CLANG_INCLUDE_GRAPH_EXECUTABLE clang-include-graph )

	if (NOT CLANG_INCLUDE_GRAPH_EXECUTABLE )
		message (WARNING "Target ${PROJECT_NAME}_include-graph: not provided"
			" since executable 'clang-include-graph' was not found."
			" This message is for developers only,"
			" ignore it for regular builds."
		)
		return()
	endif()

	message (STATUS
		"clang-include-graph found: ${CLANG_INCLUDE_GRAPH_EXECUTABLE}" )

	## Find dot

	find_program (DOT_EXECUTABLE dot )

	if (NOT DOT_EXECUTABLE )
		message (WARNING "Target ${PROJECT_NAME}_include-graph: not provided"
			" since dot (graphviz) not found."
			" This message is for developers only,"
			" ignore it for regular builds."
		)
		return()
	endif()

	message (STATUS "dot found: ${DOT_EXECUTABLE}" )

	## Defaults

	if (NOT GRAPH_FORMAT )
		set (GRAPH_FORMAT "png" )
	endif()

	if (NOT GRAPH_OUTPUT_DIR )
		set (GRAPH_OUTPUT_DIR "${LIBARCSTK_BINARY_DIR}/clang_include_graph" )
	endif()

	## Setup files and directories

	set (DOT_FILE   "${GRAPH_OUTPUT_DIR}/${PROJECT_NAME}.dot" )
	set (IMAGE_FILE
		"${GRAPH_OUTPUT_DIR}/${PROJECT_NAME}_includes.${GRAPH_FORMAT}" )

	file (MAKE_DIRECTORY ${GRAPH_OUTPUT_DIR} )

	## Collect commands

	set (GRAPH_COMMANDS
		COMMAND ${CLANG_INCLUDE_GRAPH_EXECUTABLE}
			-J 4
			--output ${DOT_FILE}
			--compilation-database-dir ${LIBARCSTK_BINARY_DIR}
			--relative-to ${LIBARCSTK_INCLUDE_BINARY_DIR}
			--relative-only
			--exclude-system-headers
			--graphviz
		COMMAND ${DOT_EXECUTABLE} -T${GRAPH_FORMAT} ${DOT_FILE} -o ${IMAGE_FILE}
	)

	## Optional remove dot file
	if (NOT GRAPH_KEEP_DOT_FILES )
		list (APPEND GRAPH_COMMANDS
			COMMAND ${CMAKE_COMMAND} -E rm -f ${DOT_FILE} )
	endif()

	add_custom_target (${PROJECT_NAME}_include-graph
		${GRAPH_COMMANDS}
		WORKING_DIRECTORY ${LIBARCSTK_BINARY_DIR}
		BYPRODUCTS ${IMAGE_FILE}
		COMMENT "Generating target dependency graph (${IMAGE_FILE})"
	)

	set(${OUT_VAR} TRUE PARENT_SCOPE)
endfunction() # 1}}}


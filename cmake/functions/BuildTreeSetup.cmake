## libarcstk: Setup build tree
## vim:fdm=marker

function (setup_build_tree)

## -- Log Directory {{{1

add_custom_command (
	OUTPUT  "${LIBARCSTK_LOG_BINARY_DIR}"
	COMMAND "${CMAKE_COMMAND}"
	ARGS    -E make_directory "${LIBARCSTK_LOG_BINARY_DIR}"
	VERBATIM
)

add_custom_target (libarcstk_create_log_dir ALL
	DEPENDS "${LIBARCSTK_LOG_BINARY_DIR}" )
##}}}1

## -- Build tree include Directory + Link {{{1

add_custom_command (
	OUTPUT  "${LIBARCSTK_BINARY_DIR}/include"
	COMMAND "${CMAKE_COMMAND}"
	ARGS    -E make_directory "${LIBARCSTK_BINARY_DIR}/include"
	VERBATIM
)

add_custom_target (libarcstk_create_include_dir
	DEPENDS "${LIBARCSTK_BINARY_DIR}/include"
	VERBATIM
)

## Create build-tree include directory (symlink)
## This is useful for finding the includes when used as a subproject.
add_custom_target (libarcstk_link_to_headers
	COMMAND "${CMAKE_COMMAND}"
		-E create_symlink
		"${LIBARCSTK_INCLUDE_SOURCE_DIR}"
		"${LIBARCSTK_INCLUDE_BINARY_DIR}"
	VERBATIM
)

add_dependencies (libarcstk_link_to_headers libarcstk_create_include_dir )

endfunction()
##}}}1


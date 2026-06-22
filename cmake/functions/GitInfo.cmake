## libarcstk: Acquire git version info
## vim:fdm=marker

find_package (Git QUIET REQUIRED )

## Execute git
function (_libarcstk_git_execute_command OUT_VAR )

	set (${OUT_VAR} "UNKNOWN" PARENT_SCOPE )

	# ARGN contains all arguments after OUT_VAR
	execute_process (
		COMMAND "${GIT_EXECUTABLE}" ${ARGN}
		WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
		RESULT_VARIABLE GIT_EXIT_CODE
		OUTPUT_VARIABLE GIT_OUTPUT
		OUTPUT_STRIP_TRAILING_WHITESPACE
	)

	if (GIT_EXIT_CODE EQUAL 0)
		set (${OUT_VAR} "${GIT_OUTPUT}" PARENT_SCOPE )
		return()
	endif()

endfunction()


# Get git version string
function (libarcstk_git_get_version_string VERSION_VAR )

	set (${VERSION_VAR} "v0.0.0-nogit" PARENT_SCOPE )

	_libarcstk_git_execute_command (GIT_VERSION describe --always HEAD )

	if (GIT_VERSION STREQUAL "UNKNOWN" )
		message (WARNING "Git describe failed, using fallback: ${VERSION_VAR}" )
	else()
		message (STATUS "git describe --always HEAD: ${GIT_VERSION}" )
	endif()

	set (${VERSION_VAR} "${GIT_VERSION}" PARENT_SCOPE )
endfunction()

# Get git commit id
function (libarcstk_git_get_commit_id COMMIT_VAR )

	set (${COMMIT_VAR} "00000000" PARENT_SCOPE )

	_libarcstk_git_execute_command (GIT_COMMIT_ID rev-parse HEAD )

	if (GIT_COMMIT_ID STREQUAL "UNKNOWN")
		message (WARNING "Git rev-parse failed, using fallback: ${COMMIT_VAR}" )
	else()
		message (STATUS "git rev-parse HEAD: ${GIT_COMMIT_ID}" )
	endif()

	set (${COMMIT_VAR} "${GIT_COMMIT_ID}" PARENT_SCOPE )
endfunction()


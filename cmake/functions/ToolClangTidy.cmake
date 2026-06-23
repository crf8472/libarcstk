## libarcstk: CMake functions for static code analysis
## vim:fdm=marker

cmake_minimum_required (VERSION 3.18 )

## Enable static analysis functionality
function (libarcstk_enable_clang_tidy OUT_VAR ) # {{{1

	set (${OUT_VAR} FALSE PARENT_SCOPE )

	find_program (CLANG_TIDY_BINARY
		NAMES clang-tidy
		DOC "Path to clang-tidy excecutable"
	)

	if (NOT CLANG_TIDY_BINARY )
		message (WARNING "Target ${PROJECT_NAME}_clang-tidy: not provided"
			" since clang-tidy was not found."
			" This message is for developers only,"
			" ignore it for regular builds."
		)
		return()
	endif()

	message (STATUS "clang-tidy found: ${CLANG_TIDY_BINARY}" )

	set (CLANG_TIDY_SCRIPT
		"${CMAKE_CURRENT_SOURCE_DIR}/cmake/scripts/run_clang_tidy.cmake" )

	if (NOT EXISTS "${CLANG_TIDY_SCRIPT}" )
		message (WARNING "Target ${PROJECT_NAME}_clang-tidy: not provided"
			" since clang-tidy script not found: ${CLANG_TIDY_SCRIPT}."
			" This message is for developers only, ignore it for regular builds."
		)
		return()
	endif()

	set (COMPILEDB_DIR "${LIBARCSTK_BINARY_DIR}/" )
	set (INCLUDE_DIR   "${LIBARCSTK_INCLUDE_BINARY_DIR}" )
	set (REPORT_FILE   "${LIBARCSTK_BINARY_DIR}/clang_tidy_report.txt" )
	set (LOG_FILE      "${LIBARCSTK_BINARY_DIR}/clang_tidy_report.log" )

	add_custom_target(${PROJECT_NAME}_clang-tidy
		COMMAND ${CMAKE_COMMAND}
			-DCLANG_TIDY_BINARY="${CLANG_TIDY_BINARY}"
			-DSOURCES_DIR="${LIBARCSTK_SOURCE_DIR}"
			-DCOMPILEDB_DIR="${COMPILEDB_DIR}"
			-DINCLUDE_DIR="${INCLUDE_DIR}"
			-DREPORT_FILE="${REPORT_FILE}"
			-DLOG_FILE="${LOG_FILE}"
			-P "${CLANG_TIDY_SCRIPT}"
		WORKING_DIRECTORY "${LIBARCSTK_ROOT_DIR}"
		BYPRODUCTS "${REPORT_FILE}" "${LOG_FILE}"
		COMMENT "Running clang-tidy..."
	)

	set(${OUT_VAR} TRUE PARENT_SCOPE)
endfunction() # 1}}}


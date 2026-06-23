## libarcstk: CMake functions for using code coverage
## vim:fdm=marker

cmake_minimum_required (VERSION 3.18 )

## Enable code coverage by gcovr
function (libarcstk_enable_code_coverage_by_gcovr OUT_VAR ) # {{{1

	set (${OUT_VAR} FALSE PARENT_SCOPE )

	find_program (GCOVR_BINARY gcovr )

	if (NOT GCOVR_BINARY )
		message (STATUS "gcovr not found" )
		return()
	endif()

	message (STATUS "gcovr found: ${GCOVR_BINARY}" )

	add_custom_target (${PROJECT_NAME}_coverage
		COMMAND ${CMAKE_COMMAND} -E make_directory coverage-report
		COMMAND ${GCOVR_BINARY}
			--root .
			--filter '${LIBARCSTK_SOURCE_DIR}/.*\.[ch]pp'
			--filter '${LIBARCSTK_INCLUDE_SOURCE_DIR}/.*'
			--filter '${LIBARCSTK_GENSRC_BINARY_DIR}/.*\.[ch]pp'
			--html-details coverage-report/index.html
		COMMAND ${CMAKE_COMMAND} -E echo
			"Report file: ${LIBARCSTK_BINARY_DIR}/coverage-report/index.html"
		WORKING_DIRECTORY "${LIBARCSTK_BINARY_DIR}"
		COMMENT "Generating code coverage report with gcovr..."
	)

	set (${OUT_VAR} TRUE PARENT_SCOPE )
endfunction() # 1}}}

## Enable code coverage by lcov
function (libarcstk_enable_code_coverage_by_lcov OUT_VAR ) # {{{1

	set (${OUT_VAR} FALSE PARENT_SCOPE )

	find_program (LCOV_BINARY    lcov    )
	find_program (GENHTML_BINARY genhtml )

	if (NOT LCOV_BINARY OR NOT GENHTML_BINARY )
		return()
	endif()

	add_custom_target (${PROJECT_NAME}_coverage
		COMMAND ${CMAKE_COMMAND} -E remove -f coverage.info
		COMMAND ${LCOV_BINARY}
			--ignore-errors inconsistent,source
			--directory .
			--capture
			--output-file coverage.info
		COMMAND ${LCOV_BINARY}
			--remove coverage.info '_deps/*' '/usr/*'
			--output-file coverage.info
			--ignore-errors inconsistent,source
		COMMAND ${GENHTML_BINARY}
			--synthesize-missing
			--ignore-errors source
			coverage.info
			--output-directory coverage-report
		COMMAND ${CMAKE_COMMAND} -E echo
			"Report file: ${LIBARCSTK_BINARY_DIR}/coverage-report/index.html"
		WORKING_DIRECTORY "${LIBARCSTK_BINARY_DIR}"
		COMMENT "Generating code coverage report with lcov..."
	)

	set (${OUT_VAR} TRUE PARENT_SCOPE )
endfunction() # 1}}}

## Add compiler switches for code coverage
macro (libarcstk_activate_code_coverage_on_compiler ) # {{{1
	add_compile_options (--coverage)
	add_link_options    (--coverage)
endmacro() # 1}}}

## Enable code coverage functionality
function (libarcstk_enable_code_coverage OUT_VAR ) # {{{1

	set(${OUT_VAR} FALSE PARENT_SCOPE )

	if (NOT CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang" )

		message (WARNING "Target ${PROJECT_NAME}_coverage not provided"
			" since code coverage is only supported by g++/clang++" )
		return()
	endif()

	message (STATUS "Compiler supports code coverage" )
	## Check tools before activating

	## Preferred: use gcovr

	libarcstk_enable_code_coverage_by_gcovr (GCOVR_ENABLED )

	if (GCOVR_ENABLED )
		message (STATUS "Target ${PROJECT_NAME}_coverage:"
			" Use gcovr for code coverage" )

		libarcstk_activate_code_coverage_on_compiler()
		set (${OUT_VAR} TRUE PARENT_SCOPE )
		return()

	endif()

	## Fallback: use lcov + genhtml

	libarcstk_enable_code_coverage_by_lcov (LCOV_ENABLED )

	if (LCOV_ENABLED )
		message (STATUS "Target ${PROJECT_NAME}_coverage:"
			" Use lcov for code coverage" )

		libarcstk_activate_code_coverage_on_compiler()
		set (${OUT_VAR} TRUE PARENT_SCOPE )
		return()
	endif()

	## No coverage tool available

	message (WARNING "Target ${PROJECT_NAME}_coverage: not provided"
		" since no coverage tool was found."
		" This message is for developers only, ignore it for regular builds."
	)

endfunction() # 1}}}


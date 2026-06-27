## CMake script for executing clang-tidy
##
## Note that clang-tidy does not have an option to specify file output. One has
## capture the output and pipe it to a file. This should be done platform
## independent by execute_process. Since we must use execute_process we use
## this standalone script tied to a custom target.

set (IGNORE_ISSUES FALSE CACHE BOOL "Ignore issues and always return 0" )

file (WRITE "${REPORT_FILE}" "")

file (GLOB_RECURSE ALL_SOURCES "${SOURCES_DIR}/*.cpp" )

execute_process(
	COMMAND ${CLANG_TIDY_BINARY} ${ALL_SOURCES}
		--config-file=${CLANG_TIDY_CONFIG}
		-p ${COMPILEDB_DIR}
		--
		-I${INCLUDE_DIR}
		-std=c++17
	OUTPUT_FILE "${REPORT_FILE}"
	ERROR_FILE  "${LOG_FILE}"
)

message (STATUS "clang-tidy report written to: ${REPORT_FILE}" )
message (STATUS "clang-tidy log written to: ${LOG_FILE}" )

file (SIZE "${REPORT_FILE}" FILE_SIZE )

## Determine exit code
if (IGNORE_ISSUES )
	set (EXIT_CODE 0 )
else ()
	set (EXIT_CODE ${FILE_SIZE} )
endif ()

## Fail on error
if (EXIT_CODE GREATER 0 )
	file (READ "${REPORT_FILE}" CLANG_TIDY_ISSUES )
	message (STATUS "clang-tidy report: ${CLANG_TIDY_ISSUES}" )
	message (FATAL_ERROR "clang-tidy issues found (exit code: ${EXIT_CODE})" )
else ()
	message (STATUS "clang-tidy did not find any issues" )
endif ()


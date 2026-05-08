## CMake script for executing clang-tidy
##
## Note that clang-tidy does not have an option to specify file output. One has
## capture the output and pipe it to a file. This should be done platform
## independent by execute_process. Since we must use execute_process we use
## this standalone script tied to a custom target.

file (WRITE "${REPORT_FILE}" "")

file (GLOB_RECURSE ALL_SOURCES "${SOURCES_DIR}/*.cpp" )

execute_process(
	COMMAND ${CLANG_TIDY_BINARY} ${ALL_SOURCES}
		-p ${COMPILEDB_DIR}
		--
		-I${INCLUDE_DIR}
		-std=c++17
	OUTPUT_FILE "${REPORT_FILE}"
	ERROR_FILE  "${LOG_FILE}"
)

message(STATUS "clang-tidy report written to: ${REPORT_FILE}")
message(STATUS "clang-tidy log written to: ${LOG_FILE}")


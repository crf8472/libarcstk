## Produce an updated instance of MAINPAGE.md
##
## Note that all variables that are supposed to be substituted in MAINPAGE.md
## must be passed by -D when calling this script.

find_package (Git QUIET REQUIRED )

## Run git to update info
execute_process (
	COMMAND ${GIT_EXECUTABLE} describe --always HEAD
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	RESULT_VARIABLE GIT_EXIT_CODE
	OUTPUT_VARIABLE GIT_VERSION_STRING
	OUTPUT_STRIP_TRAILING_WHITESPACE
)

## Did running git work?
if (GIT_EXIT_CODE EQUAL 0 )
	message (STATUS "Git calls version: ${GIT_VERSION_STRING}" )
else()
	message (WARNING "Failed to run git describe --always HEAD" )
endif()

## Run git to update info
execute_process (
	COMMAND ${GIT_EXECUTABLE} --no-pager log --no-color -1 --format="%H"
	WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
	RESULT_VARIABLE GIT_EXIT_CODE
	OUTPUT_VARIABLE GIT_COMMIT_ID
	OUTPUT_STRIP_TRAILING_WHITESPACE
	)

## Did running git work?
if (GIT_EXIT_CODE EQUAL 0 )
	message (STATUS "Git supposes commit: ${GIT_VERSION_STRING}" )
else()
	message (WARNING "Failed to run git log" )
endif()

configure_file ( ${DOC_TEXTS_SOURCE_DIR}/MAINPAGE.md.in
	${DOC_TEXTS_GEN_DIR}/MAINPAGE.md
	@ONLY
)


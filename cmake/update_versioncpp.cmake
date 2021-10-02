## Produce an updated instance of version.cpp

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

configure_file ( ${PROJECT_SOURCE_DIR}/version.cpp.in
	${PROJECT_BUILD_SOURCE_DIR}/version.cpp
	@ONLY
)


## CMake file for building the documentation

cmake_minimum_required (VERSION 3.7 )


find_package (Doxygen 1.8.0 ) #COMPONENTS REQUIRED dot )


message (STATUS
	"Note: to build documentation do 'cmake --build . --target doc'" )


## --- Organize subdirectories and build directories

## "Target directory for generated documentation"
set (DOC_BINARY_DIR    "${PROJECT_BINARY_DIR}/generated-docs" )

## "Target directory for configured documentation texts"
set (DOC_TEXTS_BINARY_DIR "${DOC_BINARY_DIR}/texts" )

## Path to import documentation texts
set (DOC_TEXTS_SOURCE_DIR "${PROJECT_DOC_DIR}/texts" )


## --- Organize documentation sources

## Construct comment for doc target
set (DOC_COMMENT "Build documentation for APIs" )

## Deployed mainpage file
set (DOC_MAINPAGE "${DOC_TEXTS_BINARY_DIR}/MAINPAGE.md" )

## Define input for documentation
set (DOC_SOURCES ${EXPORTED_HEADERS} ${DOC_MAINPAGE} )

## Define the target name and location of the base Doxyfile to be included
set (DOC_DOXYFILE_BASE "${DOC_BINARY_DIR}/Doxyfile-base" )

## Define the target name and location of the base Doxyfile to be included
set (DOC_DOXYFILE_BASE_TEMPLATE "${PROJECT_DOC_DIR}/Doxyfile.in" )

## CMake controlled values in the Doxyfile base template
set (DOXYGEN_OUTPUT_DIRECTORY "${DOC_BINARY_DIR}" )
string (REPLACE ";" " " DOXYGEN_STRIP_FROM_PATH
	"${PROJECT_INCLUDE_DIR}/;${PROJECT_BUILD_SOURCE_DIR}/" )
string (REPLACE ";" " " DOXYGEN_STRIP_FROM_INC_PATH
	"${PROJECT_INCLUDE_DIR}/;${PROJECT_BUILD_SOURCE_DIR}/" )
string (REPLACE ";" " " DOXYGEN_INPUT
	"${PROJECT_INCLUDE_DIR};${PROJECT_BUILD_SOURCE_DIR};${DOC_TEXTS_BINARY_DIR}" )
set (DOXYGEN_FILE_PATTERNS "*.hpp *.md" )
set (DOXYGEN_USE_MDFILE_AS_MAINPAGE "${DOC_MAINPAGE}" )

## Configure documentation texts
configure_file ("${DOC_TEXTS_SOURCE_DIR}/MAINPAGE.md" "${DOC_MAINPAGE}" @ONLY )



## --- Build internal docs if requested

if (WITH_INTERNAL_DOCS)

	message (STATUS "Will build documentation also for internal APIs" )
	set (DOXYGEN_INTERNAL_DOCS "YES" )

	set(DOC_COMMENT "${DOC_COMMENT} (also internal APIs)" )

	set (DOC_SOURCES ${DOC_SOURCES} ${NONPUBLIC_HEADERS} )

else ()

	set (DOXYGEN_INTERNAL_DOCS "NO" )

endif (WITH_INTERNAL_DOCS)



## --- Provide base Doxyfile

configure_file ("${DOC_DOXYFILE_BASE_TEMPLATE}" "${DOC_DOXYFILE_BASE}" @ONLY )



## --- Activate requested documentation targets

## Add default doc target.
## Each documentation config in thirdparty/ may add its own concrete dependency
## target to 'doc' and do the concrete work.
## As a fallback, we provide target 'doc_plain' that generates doxygen's stock
## HTML output that gives as this nice, cosy feeling of the Nineties.

add_custom_target (doc )



## --- If some thirdparty documentation tool was requested, use it

set (VARIANTS MCSS RTD )

foreach (VARIANT IN ITEMS ${VARIANTS} )

	if (USE_${VARIANT} )

		string(TOLOWER ${VARIANT} DOC_DIR )

		add_subdirectory ("thirdparty/${DOC_DIR}" EXCLUDE_FROM_ALL )

		set (SKIP_DOXYGEN_HTML TRUE )
	endif()

endforeach()


## --- If no variant is requested, use standard doxygen for target 'doc'

if (NOT SKIP_DOXYGEN_HTML )

	message (STATUS "Will build documentation with pure doxygen (HTML)" )

	set (DOC_OUTPUT_MAINPAGE "${DOC_BINARY_DIR}/html/index.html" )

	add_custom_command (
		OUTPUT  "${DOC_OUTPUT_MAINPAGE}"
		COMMAND Doxygen::doxygen "${DOC_DOXYFILE_BASE}"
		DEPENDS "${DOC_DOXYFILE_BASE_TEMPLATE}"
				"${DOC_DOXYFILE_BASE}"
				${DOC_SOURCES}
		WORKING_DIRECTORY "${DOC_BINARY_DIR}"
		COMMENT "${DOC_COMMENT}"
		VERBATIM
	)

	add_custom_target (doc_plain DEPENDS "${DOC_OUTPUT_MAINPAGE}" )

	add_dependencies (doc doc_plain )

endif()


## CMake script for deduplicating doxygen's XML
## vim:fdm=marker


message (STATUS "Doxygen version requires index deduplication" )


set (DOXYGEN_XML_DEDUP_SCRIPT ${CMAKE_CURRENT_BINARY_DIR}/dedup_index.py )
set (DOXYGEN_XML_DEDUP_SCRIPT ${DOXYGEN_XML_DEDUP_SCRIPT} PARENT_SCOPE )

## --- Setup deduplication script

configure_file (
	"${CMAKE_CURRENT_SOURCE_DIR}/dedup_index.py.in"
	${DOXYGEN_XML_DEDUP_SCRIPT}
	@ONLY
)

## In either case, we need python for deduplication. Either a virtualenv is
## setup for a generator (like m.css) or we try to use the systemwide
## python.

if (PYTHON_ENV_AVAILABLE )

	add_requirements_target (libarcstk_doxygen-dedup_requirements
		"${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
	)

else()

	find_package (Python3 COMPONENTS Interpreter Development REQUIRED )

	set (PYTHON_CMD )

endif (PYTHON_ENV_AVAILABLE )


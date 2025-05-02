## CMake script for applying XSLT transformations to doxygen's XML
## vim:fdm=marker

message (STATUS "Doxygen XSLT transformations required" )


## We need python for deduplication. Either a virtualenv is setup for a
## generator (like m.css) or we try to use the systemwide python.

if (PYTHON_ENV_AVAILABLE )

	add_requirements_target (libarcstk_doxygen-dedup_requirements
		"${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
	)

else()

	find_package (Python3 COMPONENTS Interpreter Development REQUIRED )

	set (PYTHON_CMD )

endif (PYTHON_ENV_AVAILABLE )


## CMake script for addings postprocessing tasks to doxygen's XML
## vim:fdm=marker

## --- Setup Python target libarcstk_doxygen_xml_env {{{1

## Target for preparing the doxygen XML run (python etc.)
add_custom_target (libarcstk_doxygen_xml_env )

if (PYTHON_ENV_AVAILABLE )

	## A virtualenv was already setup for a HTML static site generator
	## (like m.css).

	add_requirements_target (libarcstk_doxygen_xml_python_requirements
		"${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt"
	)

	add_dependencies  (libarcstk_doxygen_xml_env
		libarcstk_doxygen_xml_python_requirements )

else()

	## Try to use the systemwide python.

	## TODO Test this, it may be just unsuitable!

	## Will fail if python requirements were not met. We cannot install
	## requirements systemwide.

	find_package (Python3 COMPONENTS Interpreter Development REQUIRED )

	set (PYTHON_CMD )

endif (PYTHON_ENV_AVAILABLE )
## 1}}}



## --- Add all required tasks {{{1

foreach (_task IN LISTS XML_POSTPROC_TASKS )

	add_subdirectory (${_task} )
endforeach()
unset (_task )


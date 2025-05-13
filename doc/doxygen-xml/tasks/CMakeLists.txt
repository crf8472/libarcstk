## CMake script for addings postprocessing tasks to doxygen's XML
## vim:fdm=marker

## --- Setup Python target libarcstk_doxygen_xml_env {{{1

## Target for preparing the doxygen XML run (python etc.)
add_custom_target (libarcstk_doxygen_xml_env )
add_dependencies  (libarcstk_doxygen_xml_env
							libarcstk_create_log_dir
							libarcstk_create_output_dir )

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

## --- Provide function for Subdir CMake Scripts to perform XML tasks {{{1

function (add_doxygen_xml_task DOXY_TASK_TARGET XML_TASK_NAME XML_FILE )

	## Setup doxygen task target {{{2

	add_dependencies (${DOXY_TASK_TARGET} libarcstk_doxygen_xml_env )

	add_custom_command (
		TARGET  ${DOXY_TASK_TARGET} POST_BUILD
		COMMAND ${PYTHON_CMD}
		ARGS    ${CMAKE_SOURCE_DIR}/doc/doxygen-xml/tasks/xsl_transform.py
				${XML_FILE}
				${CMAKE_SOURCE_DIR}/doc/doxygen-xml/tasks/${XML_TASK_NAME}/${XML_TASK_NAME}.xsl
				${XML_FILE}
		WORKING_DIRECTORY "${DOC_GEN_DIR}"
		COMMENT "Applying task ${XML_TASK_NAME} to ${XML_FILE}"
		VERBATIM
	)
	## 2}}}

endfunction()


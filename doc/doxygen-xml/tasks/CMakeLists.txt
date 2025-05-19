## CMake script for addings postprocessing tasks to doxygen's XML
##
## vim:fdm=marker

message (STATUS "Doxygen XSLT postprocessing is required" )


## --- Setup Python target libarcstk_doxygen_xml_env {{{1

## Target for preparing the doxygen XML run (python etc.)
add_custom_target (libarcstk_doxygen_xml_env )
add_dependencies (libarcstk_doxygen_provide_xml libarcstk_doxygen_xml_env )

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

endif()
## 1}}}
## --- Load Postprocessing Tasks from Subdirectories {{{1
foreach (_task IN LISTS XML_POSTPROC_TASKS )

	add_subdirectory (${_task} )
endforeach()
unset (_task )



## --- Functions to Add XML Postprocessing Tasks {{{1

## -- Function: Add a XSLT post processing comand {{{2
function (add_doxygen_xslt_task TASK DOXYFILE INDEX_XML )

	## "Chain" a target: task-target depends on doxygen_generate_xml and is
	## itself a dependency of doxygen_provide_xml. The actual posprocessing
	## will be a POST_BUILD command to this target.

	set (TASK_TARGET libarcstk_doxygen_task_${TASK} )

	add_custom_target (${TASK_TARGET} )

	add_dependencies (${TASK_TARGET}
		libarcstk_doxygen_generate_xml
		libarcstk_doxygen_xml_env
	)

	add_dependencies (libarcstk_doxygen_provide_xml ${TASK_TARGET} )

	## When doxygen_generate_xml is completed, postprocess the file

	add_custom_command (
		TARGET  ${TASK_TARGET} POST_BUILD
		COMMAND "${PYTHON_CMD}"
		ARGS    "${PROJECT_ROOT_DIR}/doc/doxygen-xml/tasks/xsl_transform.py"
				"${INDEX_XML}"
				"${PROJECT_ROOT_DIR}/doc/doxygen-xml/tasks/${TASK}/${TASK}.xsl"
				"${INDEX_XML}"
		WORKING_DIRECTORY "${DOC_GEN_DIR}"
		COMMENT "Remove duplicate compounds in ${INDEX_XML}"
		VERBATIM
	)

endfunction()


## -- Function: Add all post processing comands in given list {{{2
function (add_doxygen_xslt_tasks DOXY_TARGET DOXYFILE INDEX_XML TASK_LIST )

	foreach (_task IN LISTS TASK_LIST )

		add_doxygen_xslt_task (${_task} "${DOXYFILE}" "${INDEX_XML}" )
	endforeach()
	unset (_task )

endfunction()


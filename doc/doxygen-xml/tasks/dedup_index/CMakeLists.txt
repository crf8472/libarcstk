## CMake script for deduplicating doxygen's XML
## vim:fdm=marker

## In index.xml, every class in an inline namespace occurrs twice: once as
## member of the owning namespace where they are inlined to and a second time as
## members of the specific inline namespace. It did not find a way to suppress
## or avoid this.
## https://github.com/doxygen/doxygen/issues/5914
## https://github.com/doxygen/doxygen/pull/10070


message (STATUS "Doxygen version requires index deduplication" )


## TODO
## This file should just add a function that can be called by the
## add_doxygen_xml_target function


add_custom_target (libarcstk_doxygen_task_dedup_index )

add_dependencies  (libarcstk_doxygen_task_dedup_index
												libarcstk_doxygen_generate_xml )
add_dependencies  (libarcstk_doxygen_task_dedup_index
													libarcstk_doxygen_xml_env )

add_custom_command (
	TARGET  libarcstk_doxygen_task_dedup_index POST_BUILD
	COMMAND ${PYTHON_CMD}
	ARGS    ${CMAKE_SOURCE_DIR}/doc/doxygen-xml/tasks/xsl_transform.py
			${INDEX_XML}
			${CMAKE_SOURCE_DIR}/doc/doxygen-xml/tasks/dedup_index/dedup_index.xsl
			${INDEX_XML}
	WORKING_DIRECTORY "${DOC_GEN_DIR}"
	COMMENT "Remove duplicate compounds in ${INDEX_XML}"
	VERBATIM
)

add_dependencies  (libarcstk_doxygen_provide_xml
										libarcstk_doxygen_task_dedup_index )


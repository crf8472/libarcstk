## CMake script for deduplicating doxygen's XML
## vim:fdm=marker

## In index.xml, every class in an inline namespace occurrs twice: once as
## member of the owning namespace where they are inlined to and a second time as
## members of the specific inline namespace. It did not find a way to suppress
## or avoid this.
## https://github.com/doxygen/doxygen/issues/5914
## https://github.com/doxygen/doxygen/pull/10070


message (STATUS "Doxygen version requires index deduplication" )


## Setup doxygen index deduplication target {{{2

add_dependencies (${DOXY_TARGET} libarcstk_doxygen_xml_env )

add_custom_command (#${DOXY_TARGET}_dedup
	TARGET  ${DOXY_TARGET} POST_BUILD
	#DEPENDS ${INDEXFILE}
	COMMAND ${PYTHON_CMD}
	ARGS    ${CMAKE_SOURCE_DIR}/doc/doxygen-xml/xsl_transform.py
			${INDEXFILE}
			${CMAKE_SOURCE_DIR}/doc/doxygen-xml/tasks/dedup_index/dedup_index.xsl
			${INDEXFILE}
	WORKING_DIRECTORY "${DOC_GEN_DIR}"
	COMMENT "Remove duplicate compounds in ${INDEXFILE}"
	VERBATIM
)

	#add_dependencies (${DOXY_TARGET}_dedup libarcstk_doxygen_xml_env )
	#add_dependencies (${DOXY_TARGET}_dedup ${DOXY_TARGET}_run )

	#add_dependencies (${DOXY_TARGET}       ${DOXY_TARGET}_dedup )
	## 2}}}


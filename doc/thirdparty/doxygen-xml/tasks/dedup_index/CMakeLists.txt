## CMake script for deduplicating doxygen's XML
## vim:fdm=marker

## In index.xml, every class in an inline namespace occurrs twice: once as
## member of the owning namespace where they are inlined to and a second time as
## members of the specific inline namespace. It did not find a way to suppress
## or avoid this.
## https://github.com/doxygen/doxygen/issues/5914
## https://github.com/doxygen/doxygen/pull/10070


message (STATUS "Doxygen version requires index deduplication" )


## --- Provide Macro for Subdir CMake Scripts to Request Doxygen Runs {{{1

macro (add_doxygen_xml_target_with_dedup DOXY_TARGET DOXYFILE INDEXFILE )

	## Setup doxygen run target
	add_custom_target (${DOXY_TARGET}_run DEPENDS ${INDEXFILE} )
	add_dependencies  (${DOXY_TARGET}_run
		libarcstk_create_log_dir
		libarcstk_create_output_dir
		libarcstk_static_pages_files
	)

	set (STAMP_FILE ${INDEXFILE}.deduplicated )

	add_custom_command (
		OUTPUT  ${STAMP_FILE}
		COMMAND ${PYTHON_CMD}
		ARGS    ${CMAKE_SOURCE_DIR}/doc/thirdparty/doxygen-xml/xsl_transform.py
				${INDEXFILE}
				${CMAKE_SOURCE_DIR}/doc/thirdparty/doxygen-xml/tasks/dedup_index/dedup_index.xsl
				${INDEXFILE}
		COMMAND ${CMAKE_COMMAND}
		ARGS    -E touch ${STAMP_FILE}
		DEPENDS ${INDEXFILE}
		WORKING_DIRECTORY "${DOC_GEN_DIR}"
		COMMENT "Remove duplicate compounds in ${INDEXFILE}"
		VERBATIM
	)

	## Setup deduplication target
	add_custom_target (${DOXY_TARGET}_dedup DEPENDS ${STAMP_FILE} )
	add_dependencies  (${DOXY_TARGET}_dedup ${DOXY_TARGET}_run )
	if (PYTHON_ENV_AVAILABLE ) # TODO if ...dedup_requirements is a TARGET...

		add_dependencies  (${DOXY_TARGET}_dedup
			libarcstk_doxygen-dedup_requirements )
	endif()

	## Setup doxygen target
	add_custom_target (${DOXY_TARGET} )
	add_dependencies  (${DOXY_TARGET} ${DOXY_TARGET}_dedup )

	unset (STAMPFILE )
endmacro()


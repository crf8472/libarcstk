## CMake script for doxygen XML
## vim:fdm=marker

message (STATUS "Doxygen XML postprocessing is used" )

## Target for preparing the doxygen XML run (python etc.)
add_custom_target (libarcstk_doxygen_xml_env       )

## Target for actually running the doxygen XML run along with post processing
add_custom_target (libarcstk_doxygen_generate_xml  )
add_dependencies  (libarcstk_doxygen_generate_xml libarcstk_doxygen_xml_env )

## --- Setup Python {{{1

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

## --- Determine whether Deduplication for Doxygen's XML if Required {{{1

## Variable DOXYGEN_NEEDS_XML_DEDUP can be used in subdirectory scopes.

## DOXYGEN_NEEDS_XML_DEDUP indicates that:
##   - XML output is requested (hence at least one thirdparty tool is used)
##   - XML output requires post processing
##
## Note: DOXYGEN_NEEDS_XML_DEDUP must be set _before_ actually loading the
## CMAKE scripts for generators with add_subdirectory()!

if (DOXYGEN_VERSION VERSION_GREATER_EQUAL 1.8.16 )

	set (DOXYGEN_NEEDS_XML_DEDUP TRUE PARENT_SCOPE )
	set (DOXYGEN_REMOVE_INLINE_NAMESPACE_NAMES TRUE PARENT_SCOPE )
endif()
## 1}}}


if (DOXYGEN_NEEDS_XML_DEDUP )
	add_subdirectory (tasks/dedup_index )
endif()

if (DOXYGEN_REMOVE_INLINE_NAMESPACE_NAMES )
	#add_subdirectory (tasks/inline_ns_name )
endif()


set (XSL_TRANSFORM "${CMAKE_CURRENT_SOURCE_DIR}/xsl_transform.py" )
set (PP_TASKS_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/tasks" )


## --- Add macro for doxygen XML target with post-processing {{{1

macro (add_doxygen_xml_target DOXY_TARGET DOXYFILE INDEXFILE )

	add_custom_target (${DOXY_TARGET} )

	## Setup doxygen run target {{{2

	add_doxygen_target (${DOXY_TARGET}_run ${DOXYFILE} ${INDEXFILE} )
	add_dependencies   (${DOXY_TARGET}     ${DOXY_TARGET}_run )
	## 2}}}

	if (DOXYGEN_NEEDS_XML_DEDUP ) # {{{2

		add_dependencies (${DOXY_TARGET} libarcstk_doxygen_xml_env )

		add_custom_command (
			TARGET  ${DOXY_TARGET}_run POST_BUILD
			COMMAND ${PYTHON_CMD}
			ARGS    ${CMAKE_SOURCE_DIR}/doc/doxygen-xml/xsl_transform.py
					${INDEXFILE}
					${CMAKE_SOURCE_DIR}/doc/doxygen-xml/tasks/dedup_index/dedup_index.xsl
					${INDEXFILE}
			WORKING_DIRECTORY "${DOC_GEN_DIR}"
			COMMENT "Remove duplicate entries in ${INDEXFILE}"
			VERBATIM
		)
	endif()
	## 2}}}

	if (DOXYGEN_REMOVE_INLINE_NAMESPACE_NAMES ) # {{{2

		# add_dependencies (${DOXY_TARGET} libarcstk_doxygen_xml_env )
		#
		# add_custom_command (
		# 	TARGET  ${DOXY_TARGET}_run POST_BUILD
		# 	COMMAND ${CMAKE_COMMAND}
		# 	ARGS    -DDOXY_TARGET=${DOXY_TARGET}
		# 			-DPYTHON_CMD=${PYTHON_CMD}
		# 			-DSCRIPTS_DIR=${CMAKE_SOURCE_DIR}/doc/doxygen-xml/
		# 			-DWORKING_DIR=${DOC_GEN_DIR}/mcss/html ## FIXME
		# 			-P ${CMAKE_SOURCE_DIR}/doc/doxygen-xml/tasks/inline_ns_name/apply.cmake
		# 	WORKING_DIRECTORY ${DOC_GEN_DIR}/mcss/html
		# 	COMMENT "Remove inline namespace names from compound definitions"
		# 	VERBATIM
		# )

	endif()
	## 2}}}

endmacro()
## 1}}}


add_dependencies  (libarcstk_doxygen_xml_env
							libarcstk_create_log_dir
							libarcstk_create_output_dir )

add_dependencies  (libarcstk_doc_generate_html libarcstk_doxygen_generate_xml )


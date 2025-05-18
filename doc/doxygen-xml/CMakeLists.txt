## CMake script for doxygen XML
## vim:fdm=marker

message (STATUS "Doxygen XML postprocessing is used" )


## --- Determine whether Deduplication for Doxygen's index.xml is Required {{{1

if (DOXYGEN_VERSION VERSION_GREATER_EQUAL 1.8.16 )

	## Variable DOXYGEN_XML_DEDUP_INDEX_REQUIRED can be used in subdirectories.

	## DOXYGEN_XML_DEDUP_INDEX_REQUIRED indicates that:
	##   - XML output is requested (hence at least one thirdparty tool is used)
	##   - XML output requires post processing
	##
	## Note: DOXYGEN_XML_DEDUP_INDEX_REQUIRED must be set _before_ actually
	## loading # the CMAKE scripts for generators with add_subdirectory()!

	list (APPEND XML_POSTPROC_TASKS "dedup_index" )
	set  (DOXYGEN_XML_DEDUP_INDEX_REQUIRED TRUE PARENT_SCOPE )
endif()
## 1}}}


## Target for actually running the doxygen XML run along with post processing
add_custom_target (libarcstk_doxygen_generate_xml )

## Target for providing XML for generator
add_custom_target (libarcstk_doxygen_provide_xml )


## --- If there any postproc tasks are required, support tasks {{{1
if (XML_POSTPROC_TASKS )

	add_subdirectory (tasks )
endif()


function (add_doxygen_xml_target DOXY_TARGET DOXYFILE INDEXFILE )

	add_doxygen_target (${DOXY_TARGET} ${DOXYFILE} ${INDEX_XML} )

endfunction()


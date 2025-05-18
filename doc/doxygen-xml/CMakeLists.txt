## CMake script for doxygen XML
## vim:fdm=marker

message (STATUS "Doxygen XML generation is required" )


## --- Determine whether Deduplication for Doxygen's index.xml is Required {{{1

if (DOXYGEN_VERSION VERSION_GREATER_EQUAL 1.8.16 )

	list (APPEND XML_POSTPROC_TASKS "dedup_index" )
endif()
## 1}}}



## --- List of Postprocessing Tasks that is Visible to Generators {{{1
set (DOXYGEN_XML_POSTPROCESSING_TASKS ${XML_POSTPROC_TASKS} PARENT_SCOPE )


## Target for actually running the doxygen XML run along with post processing
add_custom_target (libarcstk_doxygen_generate_xml )

## Target for providing XML for generator
add_custom_target (libarcstk_doxygen_provide_xml )

add_dependencies (libarcstk_doxygen_provide_xml libarcstk_doxygen_generate_xml )


## --- If any postprocessing tasks are required, support them {{{1
if (XML_POSTPROC_TASKS )

	## Use this in generator build scripts to decide whether XML is required
	## as an intermediate step or HTML can be produced directly.
	set (DOXYGEN_XML_REQUIRED TRUE PARENT_SCOPE )

	add_subdirectory (tasks )
endif()



## --- Function for generating XML in the CMakeLists.txt of the Generators {{{1
function (add_doxygen_xml_target DOXY_TARGET DOXYFILE INDEX_XML )

	add_doxygen_target (${DOXY_TARGET} ${DOXYFILE} ${INDEX_XML} )

	add_dependencies (libarcstk_doxygen_generate_xml ${DOXY_TARGET} )

	if (DOXYGEN_XML_POSTPROCESSING_TASKS )

		#message (WARNING "Tasks: ${DOXYGEN_XML_POSTPROCESSING_TASKS}" )

		add_doxygen_xslt_tasks (libarcstk_doxygen_provide_xml
				${DOXYFILE} ${INDEX_XML} ${DOXYGEN_XML_POSTPROCESSING_TASKS} )
	endif()

	## NOTE that this is only the target! Generator build script MUST provide
	## the custom_command to actually create INDEX_XML!

endfunction()


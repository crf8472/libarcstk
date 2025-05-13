## CMake script for doxygen XML
## vim:fdm=marker

message (STATUS "Doxygen XML postprocessing is used" )

## Target for actually running the doxygen XML run along with post processing
add_custom_target (libarcstk_doxygen_generate_xml )
add_dependencies  (libarcstk_doc_generate_html libarcstk_doxygen_generate_xml )

## --- Determine whether Deduplication for Doxygen's index.xml is Required {{{1

## Variable DOXYGEN_XML_DEDUP_INDEX_REQUIRED can be used in subdirectory scopes.

## DOXYGEN_XML_DEDUP_INDEX_REQUIRED indicates that:
##   - XML output is requested (hence at least one thirdparty tool is used)
##   - XML output requires post processing
##
## Note: DOXYGEN_XML_DEDUP_INDEX_REQUIRED must be set _before_ actually loading
## the CMAKE scripts for generators with add_subdirectory()!

if (DOXYGEN_VERSION VERSION_GREATER_EQUAL 1.8.16 )

	## FIXME This include must be done exactly once and in this file
	add_subdirectory (tasks )

	list (APPEND XML_POSTPROC_TASKS "dedup_index" )
	set (DOXYGEN_XML_DEDUP_INDEX_REQUIRED TRUE PARENT_SCOPE )
endif()
## 1}}}

## If there any postproc tasks are required, support tasks
if ("${XML_POSTPROC_TASKS}" )
	add_subdirectory (tasks )
endif()

## Add all required tasks
foreach (_task IN LISTS "${XML_POSTPROC_TASKS}" )
	add_subdirectory (tasks/${_task} )
endforeach()
unset (_task )


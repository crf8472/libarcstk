## CMake script for removing inline namespace names doxygen's class*.xml files
##
## vim:fdm=marker

cmake_minimum_required (VERSION 3.12 )

function (register_remove_inline_ns_names XML_INPUT_PATH )

	message (STATUS "Add doxygen Task: remove inline namespace names" )


	set (_v_ns_ "arcstk_1_1v__1__0__0_1_1" )

	set (_XML_CLASSFILES
		# required for success:
		"${XML_INPUT_PATH}/struct${_v_ns_}SampleInputIterator_1_1Concept.xml"
		"${XML_INPUT_PATH}/struct${_v_ns_}SampleInputIterator_1_1Model.xml"
		# in tab 'Classes':
		"${XML_INPUT_PATH}/class${_v_ns_}AlbumVerifier.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}Algorithm.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}Appender.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}ARId.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}AudioSize.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}Calculation.xml"
		"${XML_INPUT_PATH}/struct${_v_ns_}CDDA.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}Checksum.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}ChecksumSet.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}ChecksumSource.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}ChecksumSourceOf.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}DBAR.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}DBARBlock.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}DBARBlockHeader.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}DBARBuilder.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}DBARErrorHandler.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}DBARSource.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}DBARTriplet.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}InvalidMetadataException.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}Log.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}Logger.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}Logging.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}ParseErrorHandler.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}ParseHandler.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}SampleInputIterator.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}SampleSequence.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}Settings.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}StreamParseException.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}ToC.xml"
		"${XML_INPUT_PATH}/class${_v_ns_}TracksetVerifier.xml"
		# 7 class files are ignored: DBARForwardIterator, SampleIterator,
		# details::SampleSequenceImplBase, accuraterip::details::AccurateRipCS,
		# accuraterip::details::ARCSAlgorithm, SampleSequence<false>,
		# SampleSequence<true>.
		# 13 struct files are ignored, CDDA is the only exception.
	)

	## TODO May be we can glob exactly the relevant files? Like:
	#
	#file (GLOB remove_inline_ns_names_CLASSFILES
	#	"${XML_INPUT_PATH}/classarcstk*.xml")
	#
	#message (STATUS "classfiles: ${remove_inline_ns_names_CLASSFILES}" )
	#
	#add_doxygen_xslt_task ("remove_inline_ns_names"
	#	"${remove_inline_ns_names_CLASSFILES}" "${XML_INPUT_PATH}" )

	add_doxygen_xslt_task ("remove_inline_ns_names"
		"${_XML_CLASSFILES}" "${XML_INPUT_PATH}" )

	unset (_XML_CLASSFILES)

endfunction()



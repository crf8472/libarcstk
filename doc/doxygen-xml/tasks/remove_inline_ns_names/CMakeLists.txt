## CMake script for removing inline namespace names doxygen's class*.xml files
##
## vim:fdm=marker

cmake_minimum_required (VERSION 3.12 )

function (register_remove_inline_ns_names XML_INPUT_PATH )

	message (STATUS "Add doxygen Task: remove inline namespace names" )

	## Note that '_1_1' denotes '::' and each original underscore within a
	## namespace name is escaped with another underscore, thus
	## 'arcstk::v_1_0_0::' becomes 'arcstk_1_1v__1__0__0_1_1'
	set (_sep   "_1_1" )
	set (_v100 "arcstk${_sep}v__1__0__0${_sep}" )

	## For template classes
	set (_langle "_3_01" )
	set (_rangle "_01_4" )
	set (_ttype  "T_00_01" )
	set (_tf     "F_00_01" )

	set (_XML_CLASSFILES
		## Classes
		## Listed in tab 'Classes':
		"${XML_INPUT_PATH}/class${_v100}AlbumVerifier.xml"
		"${XML_INPUT_PATH}/class${_v100}Algorithm.xml"
		"${XML_INPUT_PATH}/class${_v100}Appender.xml"
		"${XML_INPUT_PATH}/class${_v100}ARId.xml"
		"${XML_INPUT_PATH}/class${_v100}AudioSize.xml"
		"${XML_INPUT_PATH}/class${_v100}Calculation.xml"
		"${XML_INPUT_PATH}/class${_v100}Checksum.xml"
		"${XML_INPUT_PATH}/class${_v100}ChecksumSet.xml"
		"${XML_INPUT_PATH}/class${_v100}ChecksumSource.xml"
		"${XML_INPUT_PATH}/class${_v100}ChecksumSourceOf.xml"
		"${XML_INPUT_PATH}/class${_v100}DBAR.xml"
		"${XML_INPUT_PATH}/class${_v100}DBARBlock.xml"
		"${XML_INPUT_PATH}/class${_v100}DBARBlockHeader.xml"
		"${XML_INPUT_PATH}/class${_v100}DBARBuilder.xml"
		"${XML_INPUT_PATH}/class${_v100}DBARErrorHandler.xml"
		"${XML_INPUT_PATH}/class${_v100}DBARSource.xml"
		"${XML_INPUT_PATH}/class${_v100}DBARTriplet.xml"
		"${XML_INPUT_PATH}/class${_v100}InvalidMetadataException.xml"
		"${XML_INPUT_PATH}/class${_v100}Log.xml"
		"${XML_INPUT_PATH}/class${_v100}Logger.xml"
		"${XML_INPUT_PATH}/class${_v100}Logging.xml"
		"${XML_INPUT_PATH}/class${_v100}ParseErrorHandler.xml"
		"${XML_INPUT_PATH}/class${_v100}ParseHandler.xml"
		"${XML_INPUT_PATH}/class${_v100}SampleInputIterator.xml"
		"${XML_INPUT_PATH}/class${_v100}SampleSequence.xml"
		"${XML_INPUT_PATH}/class${_v100}Settings.xml"
		"${XML_INPUT_PATH}/class${_v100}StreamParseException.xml"
		"${XML_INPUT_PATH}/class${_v100}ToC.xml"
		"${XML_INPUT_PATH}/class${_v100}TracksetVerifier.xml"
		"${XML_INPUT_PATH}/class${_v100}VerificationResult.xml"
		"${XML_INPUT_PATH}/class${_v100}Verifier.xml"
		## The following 7 class files are not required for success:
		#"${XML_INPUT_PATH}/class${_v100}DBARForwardIterator.xml"
		#"${XML_INPUT_PATH}/class${_v100}SampleIterator.xml"
		#"${XML_INPUT_PATH}/class${_v100}SampleSequence${_langle}${_ttype}false${_rangle}.xml"
		#"${XML_INPUT_PATH}/class${_v100}SampleSequence${_langle}${_ttype}true${_rangle}.xml"
		#"${XML_INPUT_PATH}/class${_v100}details${_sep}SampleSequenceImplBase.xml"
		#"${XML_INPUT_PATH}/class${_v100}accuraterip${_sep}details${_sep}AccurateRipCS.xml"
		#"${XML_INPUT_PATH}/class${_v100}accuraterip${_sep}details${_sep}ARCSAlgorithm.xml"
		#
		## Structs
		## Listed in tab 'Classes':
		"${XML_INPUT_PATH}/struct${_v100}CDDA.xml"
		## Required for success:
		"${XML_INPUT_PATH}/struct${_v100}SampleInputIterator${_sep}Concept.xml"
		"${XML_INPUT_PATH}/struct${_v100}SampleInputIterator${_sep}Model.xml"
		## The following 11 struct files are not required for success:
		#"${XML_INPUT_PATH}/struct${_v100}Comparable.xml"
		#"${XML_INPUT_PATH}/struct${_v100}TotallyOrdered.xml"
		#"${XML_INPUT_PATH}/struct${_v100}accuraterip${_sep}details${_sep}NUM__SKIP__SAMPLES.xml"
		#"${XML_INPUT_PATH}/struct${_v100}accuraterip${_sep}details${_sep}Subtotals.xml"
		#"${XML_INPUT_PATH}/struct${_v100}accuraterip${_sep}details${_sep}Update${_langle}checksum${_sep}type${_sep}ARCS1_00_86465f11b80806719cfd291dcb57dddf.xml"
		#"${XML_INPUT_PATH}/struct${_v100}accuraterip${_sep}details${_sep}Update${_langle}checksum${_sep}type${_sep}ARCS1${_rangle}.xml"
		#"${XML_INPUT_PATH}/struct${_v100}accuraterip${_sep}details${_sep}Update${_langle}checksum${_sep}type${_sep}ARCS2${_rangle}.xml"
		#"${XML_INPUT_PATH}/struct${_v100}accuraterip${_sep}details${_sep}Update.xml"
		#"${XML_INPUT_PATH}/struct${_v100}conv${_sep}factor__impl${_langle}${_tf}${_ttype}false${_rangle}.xml"
		#"${XML_INPUT_PATH}/struct${_v100}conv${_sep}factor__impl${_langle}${_tf}${_ttype}true${_rangle}.xml"
		#"${XML_INPUT_PATH}/struct${_v100}conv${_sep}factor__impl.xml"
	)

	add_doxygen_xslt_task ("remove_inline_ns_names"
		"${_XML_CLASSFILES}" "${XML_INPUT_PATH}" )

	unset (_ttype )
	unset (_tf )
	unset (_rangle )
	unset (_langle )
	unset (_v100 )
	unset (_sep )

	unset (_XML_CLASSFILES )

	## TODO May be we can glob exactly the relevant files?
	## Like:
	##
	## file (GLOB _XMLFILES
	##	"${XML_INPUT_PATH}/structarcstk*.xml"
	##	"${XML_INPUT_PATH}/classarcstk*.xml")
	##
	## But at the time when this function is executed, they are not yet created.

endfunction()


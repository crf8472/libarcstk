## CMake script for removing inline namespace to some compounds' names
##
## vim:fdm=marker

cmake_minimum_required (VERSION 3.12 )

function (register_compound-remove_vns XML_INPUT_PATH )

	message (STATUS
		"Add doxygen Task: fix inline namespaces in compound names" )

	set (_sep_    "_1_1" )
	set (_arcstk_ "arcstk")
	set (_v100_   "v__1__0__0" )
	set (_arcstk_v100_ "${_arcstk_}${_sep_}${_v100_}${_sep_}" )

	## refids of the compounds to be fixed.
	## Only the compounds in the Classes/'annotated' tab are considered.
	set (_XML_COMPOUNDS
		"class${_arcstk_v100_}AlbumVerifier" ;
		"class${_arcstk_v100_}Algorithm" ;
		"class${_arcstk_v100_}Appender" ;
		"class${_arcstk_v100_}ARId" ;
		"class${_arcstk_v100_}AudioSize" ;
		"class${_arcstk_v100_}Calculation" ;
		"class${_arcstk_v100_}Checksum" ;
		"class${_arcstk_v100_}ChecksumSet" ;
		"class${_arcstk_v100_}ChecksumSource" ;
		"class${_arcstk_v100_}ChecksumSourceOf" ;
		"class${_arcstk_v100_}DBAR" ;
		"class${_arcstk_v100_}DBARBlock" ;
		"class${_arcstk_v100_}DBARBlockHeader" ;
		"class${_arcstk_v100_}DBARBuilder" ;
		"class${_arcstk_v100_}DBARErrorHandler" ;
		"class${_arcstk_v100_}DBARSource" ;
		"class${_arcstk_v100_}DBARTriplet" ;
		"class${_arcstk_v100_}InvalidMetadataException" ;
		"class${_arcstk_v100_}Log" ;
		"class${_arcstk_v100_}Logger" ;
		"class${_arcstk_v100_}Logging" ;
		"class${_arcstk_v100_}ParseErrorHandler" ;
		"class${_arcstk_v100_}ParseHandler" ;
		"class${_arcstk_v100_}SampleSequence" ;
		"class${_arcstk_v100_}Settings" ;
		"class${_arcstk_v100_}StreamParseException" ;
		"class${_arcstk_v100_}ToC" ;
		"class${_arcstk_v100_}TracksetVerifier" ;
		#"class${_arcstk_v100_}VerificationResult" ; # no fix required
		#"class${_arcstk_v100_}Verifier" ;           # no fix required
		"struct${_arcstk_v100_}CDDA" ;
		#
		#"namespace${_arcstk_v100_}AccurateRip"
		#"namespace${_arcstk_v100_}checksum"
		#"namespace${_arcstk_v100_}toc"
		#"namespace${_arcstk_v100_}conv"
		#
		#"struct${_arcstk_v100_}conv${_sep_}factor__impl" ;
		#"struct${_arcstk_v100_}conv${_sep_}factor__impl_3_01F_00_01T_00_01false_01_4" ;
		#"struct${_arcstk_v100_}conv${_sep_}factor__impl_3_01F_00_01T_00_01true_01_4" ;
	)
	## It seems, every name that is lexicographically after 'v_1_0_0' is ok.

	## Expand the compounds' refids to XML filenames
	list (TRANSFORM _XML_COMPOUNDS REPLACE "(.+)" "${XML_INPUT_PATH}/\\1.xml" )

	add_doxygen_xslt_task ("compound-remove_vns" "${_XML_COMPOUNDS}"
		"${XML_INPUT_PATH}" )

	unset (_XML_COMPOUNDS )
	unset (_sep_ )
	unset (_arcstk_ )
	unset (_v100_ )
	unset (_arcstk_v100_ )

endfunction()


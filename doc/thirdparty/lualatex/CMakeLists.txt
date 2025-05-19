## CMake  lualatex
##
## vim:fdm=marker
##
## Use lualatex to build the documentation manual from Doxygen latex output

## Prerequisites from parent:
## - 'doc' target must exist
## - Variables: DOC_GEN_DIR, DOC_COMMENT

cmake_minimum_required (VERSION 3.10 )

if (WIN32 )

	message (FATAL_ERROR
		"Lualatex support is currently not available for Windows" )
endif()

message (STATUS "Will build documentation with lualatex" )

message (WARNING "Lualatex support for the manual is experimental and broken" )


## --- Overwrite Default Values From Parent {{{1

set (DOC_COMMENT       "${DOC_COMMENT}, using lualatex" )


## --- Setup LaTeX Process {{{1

set (OUTPUT_LTX_DIR    "${DOXYGEN_OUTPUT_DIRECTORY}/lualatex" )
set (OUTPUT_MAINFILE   "${OUTPUT_LTX_DIR}/refman.tex" )
set (OUTPUT_MANUAL_PDF "${OUTPUT_LTX_DIR}/refman.pdf" )

add_custom_command(
	OUTPUT "${OUTPUT_MANUAL_PDF}"
	COMMAND make
	WORKING_DIRECTORY "${OUTPUT_LTX_DIR}"
	DEPENDS "${OUTPUT_MAINFILE}"
)


## --- Setup Doxygen Run {{{1

set (DOXYFILE_TEMPLATE "${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in" )
set (DOXYFILE          "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile" )

add_doxygen_target (libarcstk_lltx_doxygen ${DOXYFILE} ${OUTPUT_MAINFILE} )


## --- Setup doc Targets {{{1

add_custom_target (libarcstk_lltx_generate_pdf DEPENDS "${OUTPUT_MANUAL_PDF}" )

add_dependencies (libarcstk_lltx_generate_pdf
		libarcstk_lltx_doxygen
		libarcstk_create_output_dir
		libarcstk_create_log_dir
)

add_dependencies (libarcstk_doc_generate_pdf libarcstk_lltx_generate_pdf )


## --- Configure Doxyfile {{{1

configure_file ("${DOXYFILE_TEMPLATE}" "${DOXYFILE}" @ONLY )


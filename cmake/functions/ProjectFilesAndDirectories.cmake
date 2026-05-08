## libarcstk: Define project directory layout and paths
## vim:fdm=marker

## INSTALL TREE

## Subdir for installed includes
set (LIBARCSTK_INCLUDE_INSTALL_SUBDIR "include/arcstk" )


## BUILD TREE

## Root directory for out-of-source build
set (LIBARCSTK_BINARY_DIR         "${CMAKE_CURRENT_BINARY_DIR}" )

## Target directory for non-cmake logs
set (LIBARCSTK_LOG_BINARY_DIR     "${LIBARCSTK_BINARY_DIR}/log" )

## Target directory for generated sources and configuration
set (LIBARCSTK_GENSRC_BINARY_DIR  "${LIBARCSTK_BINARY_DIR}/generated-sources" )

## Target directory for public/exported headers
set (LIBARCSTK_INCLUDE_BINARY_DIR
	"${LIBARCSTK_BINARY_DIR}/${LIBARCSTK_INCLUDE_INSTALL_SUBDIR}" )


## SOURCE TREE

## Project root directory
set (LIBARCSTK_ROOT_DIR           "${CMAKE_CURRENT_SOURCE_DIR}" )

## Root directory for sources
set (LIBARCSTK_SOURCE_DIR         "${LIBARCSTK_ROOT_DIR}/src" )

## Root directory for public/exported headers
set (LIBARCSTK_INCLUDE_SOURCE_DIR "${LIBARCSTK_ROOT_DIR}/include" )


## Exported/Public Headers

list (APPEND LIBARCSTK_PUBLIC_HEADERS
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/accuraterip.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/algorithms.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/bytes.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/calculate.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/checksum.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/dbar.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/identifier.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/logging.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/loglevel.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/metadata.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/mixins.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/samples.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/verify.hpp"
	"${LIBARCSTK_INCLUDE_SOURCE_DIR}/version.hpp"
)

## Define library sources

list (APPEND LIBARCSTK_SOURCES
	"${LIBARCSTK_SOURCE_DIR}/accuraterip.cpp"
	"${LIBARCSTK_SOURCE_DIR}/bytes.cpp"
	"${LIBARCSTK_SOURCE_DIR}/calculate.cpp"
	"${LIBARCSTK_SOURCE_DIR}/checksum.cpp"
	"${LIBARCSTK_SOURCE_DIR}/dbar.cpp"
	"${LIBARCSTK_SOURCE_DIR}/identifier.cpp"
	"${LIBARCSTK_SOURCE_DIR}/metadata.cpp"
	"${LIBARCSTK_SOURCE_DIR}/samples.cpp"
	"${LIBARCSTK_SOURCE_DIR}/verify.cpp"
)


## Find libsndfile
##
## Defines the following variables:
##
## SndFile_FOUND
## SndFile_INCLUDE_DIRS
## SndFile_LIBRARIES
## SndFile_VERSION

if (LIBSNDFILE_INCLUDE_DIR AND LIBSNDFILE_LIBRARY )

	# Already in cache, so be quiet
	set (libsndfile_FIND_QUIETLY TRUE )
endif ()

## 1: use pkg-config to generate hints

find_package (PkgConfig QUIET )

if (PkgConfig_FOUND )

	PKG_CHECK_MODULES (PC_LIBSNDFILE QUIET sndfile )

	set (SndFile_VERSION ${PC_LIBSNDFILE_VERSION} )
endif (PkgConfig_FOUND )

## 2: find includes

find_path (LIBSNDFILE_INCLUDE_DIR
	NAMES sndfile.hh ## Dedicated search for the C++ header
	HINTS
	${PC_LIBSNDFILE_INCLUDEDIR}
	${PC_LIBSNDFILE_INCLUDE_DIRS}
)

## 3: find library

find_library(LIBSNDFILE_LIBRARY
	NAMES sndfile libsndfile-1
	HINTS
	${PC_LIBSNDFILE_LIBDIR}
	${PC_LIBSNDFILE_LIBRARY_DIRS}
)

## 4: handle REQUIRED and QUIET options, set _FOUND VARIABLE

include (FindPackageHandleStandardArgs )

find_package_handle_standard_args (SndFile
	REQUIRED_VARS LIBSNDFILE_LIBRARY LIBSNDFILE_INCLUDE_DIR
	VERSION_VAR   SndFile_VERSION
    FAIL_MESSAGE  DEFAULT_MSG )

## 5: set declared variables

set (SndFile_LIBRARIES    ${LIBSNDFILE_LIBRARY} )
set (SndFile_INCLUDE_DIRS ${LIBSNDFILE_INCLUDE_DIR} )

mark_as_advanced (LIBSNDFILE_INCLUDE_DIR LIBSNDFILE_LIBRARY )


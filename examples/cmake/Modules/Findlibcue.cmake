## Find libcue
##
## Defines the following variables:
##
## libcue_FOUND
## libcue_VERSION
## libcue_INCLUDE_DIRS
## libcue_LIBRARIES

## Libcue ships without a version file. Thus, if pkg-config is not available,
## we cannot verify the version and have basically lost. There is a slight
## chance that the include path contains a version number. This module therefore
## tries to find a header and parse the version number from this header.

if (LIBCUE_INCLUDE_DIR AND LIBCUE_LIBRARY )

	# Already in cache, so be quiet
	set (libcue_FIND_QUIETLY TRUE )
endif ()


## 1: use pkg-config to generate path hints for finding includes and libs

find_package (PkgConfig QUIET )

if (PkgConfig_FOUND )

	PKG_CHECK_MODULES (PC_LIBCUE QUIET libcue )

	set (LIBCUE_VERSION      ${PC_LIBCUE_VERSION} )
	set (LIBCUE_INCLUDE_DIRS ${PC_LIBCUE_INCLUDE_DIRS} )
	set (LIBCUE_LIBRARIES    ${PC_LIBCUE_LIBRARIES} )

else ()

	message (WARNING "Tool pkg-config is missing, try to find libcue anyway" )

endif (PkgConfig_FOUND )

## 2: find includes (and maybe version)

## If pkgconfig is not available, we try to get the version number from
## the include path.
if (NOT LIBCUE_VERSION )

	## Search for subpaths containing a version number, e.g.
	## /usr/include/libcue-2.0/libcue/libcue.h
	set (PATHS_WITH_VERSION )
	foreach(_path IN LISTS CMAKE_SYSTEM_PREFIX_PATH )

		file (GLOB PREFIX_SUBPATH "${_path}/include/libcue-*" )
		list (APPEND PATHS_WITH_VERSION ${PREFIX_SUBPATH} )
	endforeach()
	unset (PREFIX_SUBPATH )

	list (REMOVE_DUPLICATES PATHS_WITH_VERSION )

	## Prefix every path with its version to make path list sortable in
	## descending order
	set (SUITABLE_PATHS )
	foreach(_path IN LISTS PATHS_WITH_VERSION )

		## Get version number (hopefully)
		string (REGEX REPLACE ".+-([0-9\.]+)$" "\\1" PATH_VERSION ${_path} )

		## Create a list of version paths with entries formatted as
		## 'version:path' that is easily sortable by the version prefix
		#if (PATH_VERSION VERSION_GREATER_EQUAL libcue_FIND_VERSION )

			list (APPEND SUITABLE_PATHS "${PATH_VERSION}:${_path}" )

		#endif()
	endforeach()
	unset (PATH_VERSION )
	unset (PATHS_WITH_VERSION )

	if (SUITABLE_PATHS ) ## Avoids misleading error message

		## Sort paths by version number in descending order
		list (SORT SUITABLE_PATHS )
		list (REVERSE SUITABLE_PATHS )

		## Remove the Sorting Prefixes
		string (REGEX REPLACE "[0-9\.]+:" "" CUSTOM_PATHS "${SUITABLE_PATHS}" )
	endif ()
	unset (SUITABLE_PATHS )

endif()


find_path (LIBCUE_INCLUDE_DIR
	NAMES "libcue/libcue.h"
	HINTS /usr/include /usr/local/include /opt/homebrew/include
	PATHS ${LIBCUE_INCLUDE_DIRS} ${CUSTOM_PATHS}
)


## If header is found but version is unknown, take the version from CUSTOM_PATHS
## and search for a matching library
if (LIBCUE_INCLUDE_DIR AND NOT LIBCUE_VERSION )

	string (REGEX REPLACE ".+-([0-9\.]+)$" "\\1" LIBCUE_VERSION
		${LIBCUE_INCLUDE_DIR} )

	string (REGEX REPLACE "^([0-9]+).*" "\\1" LIBCUE_VERSION_MAJOR
		${LIBCUE_VERSION} )

	## FIXME Does not seem to be best practise: with pkg-config, we will just
	## link against libcue.so because we can rely on the pc-metadata.
	## Without pkg-config, we could guess the version from the path, but
	## there might be different versions installed. Therefore we try to link to
	## a versioned .so object. This is not actually 'wrong' but it will be very
	## surprising that cmake varies the linking instruction changes dependent
	## on whether pkg-config is present or not.
	## Alternative: when pkg-config is not present, just give up and explode.
	## This will happen anyway when trying to find the other libs without
	## pkg-config.

	find_library (LIBCUE_LIBRARY
		NAMES libcue.dylib libcue.so libcue
		NAMES libcue.so.${LIBCUE_VERSION} libcue.so.${LIBCUE_VERSION_MAJOR}
		HINTS /usr/lib /usr/local/lib /opt/homebrew/lib
		PATHS ${PC_LIBCUE_LIBDIR} ${PC_LIBCUE_LIBRARY_DIRS}
	)
endif()


## 3: find library (if not yet found)

find_library (LIBCUE_LIBRARY
	NAMES cue libcue libcue.dylib libcue.so
	PATHS ${LIBCUE_LIBRARIES}
)

## 4: handle options

include (FindPackageHandleStandardArgs )

find_package_handle_standard_args (libcue
	REQUIRED_VARS LIBCUE_LIBRARY LIBCUE_VERSION LIBCUE_INCLUDE_DIR
	VERSION_VAR   LIBCUE_VERSION
	FAIL_MESSAGE  DEFAULT_MSG )

## 5: set declared variables

set (libcue_VERSION      ${LIBCUE_VERSION} )
set (libcue_INCLUDE_DIRS ${LIBCUE_INCLUDE_DIR} )
set (libcue_LIBRARIES    ${LIBCUE_LIBRARY} )

mark_as_advanced (
	LIBCUE_VERSION
	LIBCUE_INCLUDE_DIR
	LIBCUE_INCLUDE_DIRS
	LIBCUE_LIBRARY
	LIBCUE_LIBRARIES
)


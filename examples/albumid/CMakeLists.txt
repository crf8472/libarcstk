## Build example application 'albumid'
## vim: fdm=marker

cmake_minimum_required (VERSION 3.10 )

add_executable (libarcstk_example_albumid EXCLUDE_FROM_ALL albumid.cpp )
add_dependencies (libarcstk_examples  libarcstk_example_albumid )

set_target_properties (libarcstk_example_albumid PROPERTIES
	CXX_STANDARD          17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS        OFF
	OUTPUT_NAME           albumid
	BUILD_RPATH           "${LIBARCSTK_BINARY_DIR}"
	BUILD_WITH_INSTALL_RPATH    FALSE
	INSTALL_RPATH_USE_LINK_PATH TRUE
)

target_compile_options (libarcstk_example_albumid
	PRIVATE -Wall -Wextra -pedantic )

add_dependencies (libarcstk_example_albumid libarcstk )


## build with libsndfile for audio reading

find_package (SndFile 1.0.17 REQUIRED )

if (TARGET SndFile::sndfile )
	## found by libsndfile's cmake config
	target_link_libraries (libarcstk_example_albumid
		PRIVATE SndFile::sndfile )
else()
	## found by cmake/Modules/FindSndFile.cmake
	target_include_directories (libarcstk_example_albumid
		PRIVATE ${SndFile_INCLUDE_DIRS} )
	target_link_libraries      (libarcstk_example_albumid
		PUBLIC  ${SndFile_LIBRARIES} )
endif()


## build with libcue for metadata parsing

## commented out: version without Findlibcue.cmake worked on linux not on macos
#find_path    (LIBCUE_INCLUDE_DIR NAMES "libcue/libcue.h" REQUIRED )
#find_library (LIBCUE_LIBRARY     NAMES libcue.so REQUIRED )
find_package (libcue 2.0.0 REQUIRED )

## build

## Always link to the libarcstk headers in the build/ dir - if another header
## is present, ignore it.
target_include_directories (libarcstk_example_albumid
	PRIVATE ${libcue_INCLUDE_DIRS}
	PRIVATE $<BUILD_INTERFACE:${LIBARCSTK_BINARY_DIR}/include/arcstk> )

## Always link to the libarcstk binary in the build/ dir - if another binary
## is present, ignore it.
target_link_libraries (libarcstk_example_albumid
	PRIVATE ${libcue_LIBRARIES}
	PRIVATE libarcstk )

add_dependencies ( libarcstk_examples libarcstk_example_albumid )


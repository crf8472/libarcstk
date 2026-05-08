## Build example application 'albumid'
## vim: fdm=marker

add_executable ( albumid albumid.cpp )

set_target_properties ( albumid PROPERTIES
	CXX_STANDARD          17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS        OFF
	BUILD_RPATH           "${LIBARCSTK_BINARY_DIR}"
)

target_compile_options ( albumid PRIVATE -Wall -Wextra -pedantic )

add_dependencies ( albumid libarcstk )


## build with libsndfile for audio reading

find_package (SndFile 1.0.17 REQUIRED )

if (TARGET SndFile::sndfile )
	## found by libsndfile's cmake config
	target_link_libraries (albumid PRIVATE SndFile::sndfile )
else()
	## found by cmake/Modules/Findlibsndfile.cmake

	if (NOT libsndfile_LIBRARIES )
		find_package (libsndfile 1.0.17 REQUIRED )
	endif()

	target_include_directories ( albumid PRIVATE ${libsndfile_INCLUDE_DIRS} )
	target_link_libraries      ( albumid PUBLIC  ${libsndfile_LIBRARIES} )
endif()


## build with libcue for metadata parsing

find_path    (LIBCUE_INCLUDE_DIR NAMES "libcue/libcue.h" REQUIRED )
find_library (LIBCUE_LIBRARY     NAMES libcue.so         REQUIRED )


## build

## Always link to the libarcstk headers in the build/ dir - if another header
## is present, ignore it.
target_include_directories ( albumid
	PRIVATE ${LIBCUE_INCLUDE_DIR}
	PRIVATE $<BUILD_INTERFACE:${LIBARCSTK_BINARY_DIR}/include/arcstk> )

## Always link to the libarcstk binary in the build/ dir - if another binary
## is present, ignore it.
target_link_libraries ( albumid PRIVATE ${LIBCUE_LIBRARY} PRIVATE libarcstk )


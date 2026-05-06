## Build example application 'albumcalc'
## vim: fdm=marker

add_executable ( albumcalc albumcalc.cpp )

set_target_properties ( albumcalc
	PROPERTIES
	CXX_STANDARD          17
	CXX_STANDARD_REQUIRED ON
	CXX_EXTENSIONS        OFF )

target_compile_options ( albumcalc PRIVATE -Wall -Wextra -pedantic )

add_dependencies ( albumcalc libarcstk )


## build with libsndfile for audio reading

find_package (SndFile 1.0.17 REQUIRED )

if (TARGET SndFile::sndfile )
	## found by libsndfile's cmake config
	target_link_libraries (albumcalc PRIVATE SndFile::sndfile )
else()
	## found by cmake/Modules/Findlibsndfile.cmake

	if (NOT libsndfile_LIBRARIES )
		find_package (libsndfile 1.0.17 REQUIRED )
	endif()

	target_include_directories ( albumcalc PRIVATE ${libsndfile_INCLUDE_DIRS} )
	target_link_libraries      ( albumcalc PUBLIC  ${libsndfile_LIBRARIES} )
endif()


## build with libcue for metadata parsing

find_path    (LIBCUE_INCLUDE_DIR NAMES "libcue/libcue.h" REQUIRED )
find_library (LIBCUE_LIBRARY     NAMES libcue.so         REQUIRED )


## build

set (PROJECT_BUILD_DIR "${PROJECT_ROOT_DIR}/build" )

## Always link to the libarcstk headers in the build/ dir - if another header
## is present, ignore it.
target_include_directories ( albumcalc
	PRIVATE ${LIBCUE_INCLUDE_DIR}
	PRIVATE $<BUILD_INTERFACE:${PROJECT_BUILD_DIR}/arcstk> )

## Always link to the libarcstk binary in the build/ dir - if another binary
## is present, ignore it.
target_link_libraries ( albumcalc
	PRIVATE ${LIBCUE_LIBRARY}
	PRIVATE "-Wl,--disable-new-dtags,-rpath,${PROJECT_BUILD_DIR}/" libarcstk )


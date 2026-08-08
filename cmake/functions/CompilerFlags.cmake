## libarcstk: Compiler-specific warning and optimization flags
## vim:fdm=marker

cmake_minimum_required (VERSION 3.10 )

## Prerequisites: CMAKE_RELEASE_TYPE

## --- Option: WITH_NATIVE (default ON for Release only) {{{1

option (WITH_NATIVE "Use platform specific optimization on compiling" OFF )

## For 'Release', Set Default ON
if (CMAKE_BUILD_TYPE STREQUAL "Release")

	set (WITH_NATIVE ON CACHE BOOL
			"Release: Force platform specific optimization" FORCE )
endif ()
# }}}1

## --- g++ Flags: Warnings {{{1

function (_GNU_flags_warning OUT_VAR )
	set (${OUT_VAR}
		-Wall -Wextra -Wpedantic -Weffc++
		-Wold-style-cast
		-Wsuggest-override
		-Wcast-align
		-Wcast-qual
		-Wctor-dtor-privacy
		-Wdisabled-optimization
		-Wformat=2
		-Wlogical-op
		-Wmissing-declarations
		-Wmissing-include-dirs
		-Wnoexcept
		-Woverloaded-virtual
		-Wredundant-decls
		-Wshadow
		-Wsign-conversion
		-Wsign-promo
		-Wstrict-null-sentinel
		-Wstrict-overflow=5
		-Wswitch-default
		-Wundef
		-Wuseless-cast
		-Wzero-as-null-pointer-constant
	PARENT_SCOPE )
endfunction ()
## 1}}}
## --- g++ Flags: Optimize {{{1

function (_GNU_flags_optimization OUT_VAR )

	cmake_parse_arguments (CFLAGS "" "" "FLAGS" ${ARGN} )

	set (_result "${CFLAGS_FLAGS}" )

	list (APPEND _result -march=native  )
	list (APPEND _result -mtune=generic )

	set (${OUT_VAR} "${_result}" PARENT_SCOPE )
endfunction ()
## 1}}}
## --- g++ Flags: Tests {{{1
function (_GNU_modify_flags_for_tests OUT_VAR )

	cmake_parse_arguments (CFLAGS "" "" "FLAGS" ${ARGN} )

	set (_result "${CFLAGS_FLAGS}" )

	## --- g++: Remove flags that pollute the output with false positives
	if (CMAKE_COMPILER_IS_GNUCXX )

		list (REMOVE_ITEM _result -Weffc++ )
		list (REMOVE_ITEM _result -Wctor-dtor-privacy )
		list (REMOVE_ITEM _result -Wuseless-cast )
	endif()

	set (${OUT_VAR} "${_result}" PARENT_SCOPE )
endfunction ()
## 1}}}

## --- clang++ Flags: Warnings {{{1

## Clang warnings are quite aggressive by default. Some of the g++ warnings are
## either compatibility flags that do nothing or are activated by other meta
## flags. Some are just on by default. The following list is intended to contain
## only those flags that are off by default and no compatibility flags.

function (_CLANG_flags_warning OUT_VAR )
	set (${OUT_VAR}
		## -- same as GNU
		-Wall -Wextra -Wpedantic -Weffc++
		-Wold-style-cast
		-Wcast-align
		-Wcast-qual
		-Wformat=2
		-Wshadow
		-Wsign-conversion
		-Wundef
		-Wunused
		-Wzero-as-null-pointer-constant
		## -- clang++ specific
		-Winconsistent-missing-destructor-override
	PARENT_SCOPE )
endfunction ()
## 1}}}
## --- clang++ Flags: Optimize {{{1

function (_CLANG_flags_optimization OUT_VAR )

	cmake_parse_arguments (CFLAGS "" "" "FLAGS" ${ARGN} )

	set (_result "${CFLAGS_FLAGS}" )

	list (APPEND _result -march=native  )
	list (APPEND _result -mtune=generic )

	set (${OUT_VAR} "${_result}" PARENT_SCOPE )
endfunction ()
## 1}}}
## --- clang++ Flags: Tests {{{1
function (_CLANG_modify_flags_for_tests OUT_VAR )
	## nothing to modify
	#set (${OUT_VAR} "${OUT_VAR}" PARENT_SCOPE )
endfunction ()
## 1}}}

## --- Set compile flags for COMPILER and TARGET
function (_compile_flags OUT_VAR ) # {{{1

	set (${OUT_VAR} "" PARENT_SCOPE )

	set (_options MAIN TEST )

	cmake_parse_arguments (CXX_FLAGS "${_options}" "COMPILER" "" ${ARGN} )

	##

	message (STATUS "Build with flags for compiler ${CXX_FLAGS_COMPILER}" )

	set (_cxx_flags )

	if (CXX_FLAGS_COMPILER STREQUAL "GNU" )


		_GNU_flags_warning (_cxx_flags )

		if (WITH_NATIVE )

			message (STATUS "Build with platform specific optimization")
			_GNU_flags_optimization (_cxx_flags FLAGS "${_cxx_flags}" )
		else()

			message (STATUS "Build without platform specific optimization" )
		endif()

		if (CXX_FLAGS_TEST) ## set flags for test targets

			_GNU_modify_flags_for_tests (_cxx_flags FLAGS "${_cxx_flags}" )
		endif()

	elseif  (CXX_FLAGS_COMPILER STREQUAL "CLANG" )


		_CLANG_flags_warning (_cxx_flags )

		if (WITH_NATIVE )

			message (STATUS "Build with platform specific optimization")
			_CLANG_flags_optimization (_cxx_flags FLAGS "${_cxx_flags}"  )
		else()

			message (STATUS "Build without platform specific optimization" )
		endif()

		if (CXX_FLAGS_TEST) ## set flags for test targets

			_CLANG_modify_flags_for_tests (_cxx_flags FLAGS "${_cxx_flags}" )
		endif()

	else ()

		## TODO
	endif ()

	set (${OUT_VAR} "${_cxx_flags}" PARENT_SCOPE )

endfunction() # 1}}}

## --- Select actual Compiler
function (_select_compile_flags OUT_VAR ) # {{{1

	set (${OUT_VAR} "" PARENT_SCOPE )

	set (_options MAIN TEST )
	cmake_parse_arguments (BUILD "${_options}" "" "" ${ARGN} )

	##

	set (_compiler_type  )

	if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" )

		message(STATUS "Compiler: g++ (select appropriate flags)" )
		set (_compiler_type "GNU" )

	elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*Clang" )

		message (STATUS "Compiler: clang++ (select appropriate flags)" )
		set (_compiler_type "CLANG" )

	elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC" )

		message (WARNING "Compiler: msvc is currently not supported." )
		return()
	else()

		message (WARNING "Unknown compiler: no specific flags will be applied" )
		return()
	endif()

	if (BUILD_TEST )

		_compile_flags (_cxx_flags COMPILER "${_compiler_type}" TEST )

	else()

		_compile_flags (_cxx_flags COMPILER "${_compiler_type}" MAIN )

	endif()

	set (${OUT_VAR} "${_cxx_flags}" PARENT_SCOPE )
endfunction ()
## 1}}}

## --- Apply compiler flags to targets
function (libarcstk_apply_compiler_flags ) # {{{1

	set (_options MAIN TEST )
	set (_one_value_args TARGET )

	cmake_parse_arguments (CXX
		"${_options}" "${_one_value_args}" "" ${ARGN} )

	if (NOT CXX_TARGET )
		message (WARNING "No target specified" )
		return()
	endif()

	## Select + apply flags

	set (_actual_cxx_flags )

	if (CXX_TEST )

		_select_compile_flags (_actual_cxx_flags TEST )

	else ()

		_select_compile_flags (_actual_cxx_flags MAIN )

	endif ()

	## Any actual flags passed directly?

	if (CMAKE_CXX_FLAGS )
		list (APPEND _actual_cxx_flags ${CMAKE_CXX_FLAGS} )
	endif()

	string (TOUPPER ${CMAKE_BUILD_TYPE} _build_type_name )

	if (CMAKE_CXX_FLAGS_${_build_type_name} )
		list (APPEND _actual_cxx_flags ${CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}} )
	endif()

	## Put it all together

	target_compile_options(${CXX_TARGET} PRIVATE ${_actual_cxx_flags})

	message(STATUS "Applied libarcstk's C++ compile flags"
		" to target: ${CXX_TARGET}" )

endfunction()
# }}}1


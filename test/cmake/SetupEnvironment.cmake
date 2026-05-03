## Common configuration for all test executables

## Provide compileflags for all tests
function (setup_test_compileflags )

	## --- Remove flags from g++ that pollute the output
	if (CMAKE_COMPILER_IS_GNUCXX )
		set (GNUCXX_IGNORE_FLAGS
			-Weffc++
			-Wctor-dtor-privacy
			-Wuseless-cast
		)

		foreach (_flag ${GNUCXX_IGNORE_FLAGS} )
			message(STATUS
				"Ignore compile flag ${_flag} when compiling Catch2 tests")
			list(REMOVE_ITEM PROJECT_CXX_FLAGS_WARNINGS ${_flag} )
		endforeach()
	endif()

	set (TEST_CXX_FLAGS_WARNINGS ${PROJECT_CXX_FLAGS_WARNINGS} ) #PARENT_SCOPE )
endfunction()

#set (TEST_CXX_FLAGS ${TEST_CXX_FLAGS_WARNINGS}
#		"--param" "max-gcse-memory=142720" )

function (configure_test_environment )
	setup_test_compileflags()
endfunction()


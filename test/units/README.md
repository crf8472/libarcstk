# Unit tests

## Files and naming convention

  - Every class in a public header gets a unit test file whose name is the
    name of the class. Example: class "Settings" will be unit-tested by file
	Settings.cpp. Class "ARCSAlgorithm<ARCSv1>" will be unit-tested by file
	ARCSAlgorithm_ARCS_v1.cpp.
  - Every testable entity in a public header that is not a class gets a TESTCASE
    in the unit test for the header. Example: public functions and constants of
	header metadata.hpp will be unit-tested by file header-metadata.cpp.


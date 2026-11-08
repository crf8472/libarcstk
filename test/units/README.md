# Unit tests

## Files and naming convention

  - Every class in a public header gets a unit test file whose name is the
    name of the class. Example: class "Settings" will be unit-tested by file
	Settings.cpp. Class "ARCSAlgorithm<ARCSv1>" will be unit-tested by file
	ARCSAlgorithm_ARCS_v1.cpp.
  - Every testable entity in a public header that is not a class gets a TESTCASE
    in the unit test for the header. Example: public functions and constants of
	header metadata.hpp will be unit-tested by file header-metadata.cpp.

## Tests

### TEST_CASE 1: construction and special members

  - Correctness of default and parametized Construction
  - Correctness of copy construction (if any) + copy assignment (if any)
  - Correctness of move construction (if any) + move assignment (if any)

### TEST_CASE 2: guarantees after default construction

  - size() == 0, empty()
  - converts to true/false via bool()

### TEST_CASE 3: special members

  Correctness of special functions like:

  - empty()
  - size()
  - clone()
  - operator bool()
  - to_string()
  - member swap()
  - member equals()
  - operator <<

### TEST_CASE 4:

  Correctness of public member functions.


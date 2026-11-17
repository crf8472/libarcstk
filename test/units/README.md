# Unit tests

Verify special members, member functions and dedicated friend functions of a
class and testable non-member functions.

## Files and naming convention

  - Every class in a public header gets a unit test file whose name is the
    name of the class. Example: class "Settings" will be unit-tested by file
	Settings.cpp. Class "ARCSAlgorithm<ARCSv1>" will be unit-tested by file
	ARCSAlgorithm_ARCS_v1.cpp.
  - Every testable entity in a public header that is not a class gets a TESTCASE
    in the unit test for the header. Example: public functions and constants of
	header metadata.hpp will be unit-tested by file header-metadata.cpp.

## Tests

There are up to four ``TEST_CASE``s for each unit. They are numbered in the
order of appearance in the file in case all of them are present.

### TEST_CASE 1: construction and special members

This ``TEST_CASE`` has the same name as the class it tests.

  - Correctness of default and parametized Construction
  - Correctness of copy construction (if any) + copy assignment (if any)
  - Correctness of move construction (if any) + move assignment (if any)

### TEST_CASE 2: guarantees after parameterless construction

This ``TEST_CASE`` has the same name as the class it tests, followed by the word
'default'. It verifies the guarantees of a parameterless constructed instance of
the tested class.

  - size() == 0, empty()
  - converts to true/false via bool()

### TEST_CASE 3: behavior as a value

This ``TEST_CASE`` has the same name as the class it tests, followed by the word
'value'. It verifies the correctness of the value behavior of the class, which
boils down to special functions like:

  - empty()
  - size()
  - clone()
  - operator bool()
  - to_string()
  - member swap()
  - member equals()
  - operator <<

### TEST_CASE 4: specific behavior

This ``TEST_CASE`` has the same name as the class it tests, followed by the word
'behavior'. It verifies the correctness of the specific behavior of the class.
This means, every specific public member function.


# Testing


## Test types

  - Test strategies are defined for the instantiated [unit](units/README.md),
    the [type of the unit](type_traits/README.md) if the unit is a class, struct
	or template with arguments, and [functional tests](functional/README.md) for
	any testable entity.

## Tested entities

  - Units are classes and non-member functions.
	Structs and templates with arguments are considered to be classes.
  - Public non-member functions are unit-tested in a test together with the
	other entities of the header where they are defined.
  - Tests for enum classes, non-class enums and public constants are optional.

## Building and running tests

Tests are requested and built by CMake configure switch ``-DWITH_TESTS=ON``.

To execute tests do

  $ ctest --test-dir build

when in project root directory. Note that a build with tests takes considerably
longer than without tests.

## Test framework

All tests are implemented in [Catch2][1] as ``TEST_CASE``s or, in case of
functional tests, optionally as ``SCENARIO``s (see [here][4] for documentation).

Catch2 is downloaded and build when testing is requested to CMake. Catch2 is
completely managed by CMake and is not required to be available as os package.

## Test organization

### Source files

  - Classes are unit-tested in a file with the name of the class that may
	contain one or more ``TEST_CASE``s or ``SCENARIO``s about that class.
  - For all entities declared in the same public header that are not
    classes a catchall test file for the header exists named
	``header-<name>.cpp``.
  - For all testable entities declared in a non-public header that are not
    classes a catchall test file for the module exists named
	``module-<name>.cpp``.

### Labels (CMake)

  - There are currently 3 labels for tests corresponding to the three test
	types: ``type_traits``, ``units`` and ``functional`` tests. Labels are on
	CMake-Level (and have nothing to do with Catch2).
  - The test labels correspond to source subdirectories in
	``${PROJECT_ROOT}/test/``. For documentation, see [here][2].
  - For each of those labels there exists one binary that contains all the tests
	with the respective label. For instance, for the unit tests there exists
	``libarcstk_unit_tests`` which resides in ``/build/test/units`` after
	building.

### Tags (Catch2)

On Catch2-Level, each ``TEST_CASE`` is augmented with tags, that have the form
"[testtag]" (lbracket-tag-rbracket).

In libarcstk, each test has at most the following 4 tags:

  - Class name or function name in lowercase letters. This is mandatory.
  - Module name. This is one of 'calc', 'verify', 'id', 'dbar', 'meta',
    'logging', or 'version'.
  - Public header name (optional): 'calculate', 'verify', 'dbar', 'identifier',
    'metadata', 'checksum', 'samples', 'logging', 'loglevel', ...
    (Module names with the exceptions of 'calc', 'meta' and 'id' are also header
	names.)
  - Tag ``[details]`` for implementation details regardless of the concrete
    namespace of the tested unit.

The Catch2-binary can be called with option ``--list-tags`` to list all tags or
with option ``--list-tests`` to get a list of all tests. Tags are passed as a
string like ``[verify]``, so ``./libarcstk_unit_tests --list-tests "[calc]"``
lists all tests tagged as 'calc' while ``./libarcstk_functional_tests -s
"[perform_update]"`` executes all tests tagged with ``[perform_update]``. Option
``--help`` is implemented and the entire documentation is found [here][3].

## TODO

  - Compile each public header separately

[1]: https://github.com/catchorg/catch2
[2]: https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html
[3]: https://github.com/catchorg/Catch2/blob/devel/docs/command-line.md
[4]: https://github.com/catchorg/Catch2/blob/devel/docs/test-cases-and-sections.md
[5]: https://github.com/catchorg/Catch2/blob/devel/docs/assertions.md


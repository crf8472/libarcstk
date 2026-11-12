# Testing


## Test framework

All tests are implemented in Catch2 as TEST_CASEs. Catch2 is downloaded and
managed by CMake and is not required to be available as os package.

Tests are built by CMake configure switch ``-DWITH_TESTS=ON``.


## Tested entities

Classes are tested in a file with the name of the class that may contain one or
more ``TEST_CASE``s or scenarios. For all testable entities declared in
the same header that are not classes a catchall test file for the header
exists named ``header-<name>.cpp``.


## Test Organization

### Labels (CMake)

There are currently 3 labels for tests: ``type_traits``, ``units`` and
``functional`` tests. The test labels correspond to source subdirectories in
``${PROJECT_ROOT}/test/``. Labels are on CMake-Level. For documentation, see
[1][here].

For each of those labels there exists one binary that contains all the tests
with the respective label. For instance, for the unit tests there exists
``libarcstk_unit_tests`` which resides in ``/build/test/units`` after building.

#### type_traits

Verify design properties of any class in public or non-public headers:

  - Constructing methods and destructibility of the type.
  - Verify whether class is abstract, final or without members.
  - Verify whether class is swappable, comparable or supports to_string
	functionality.

#### units

Units are classes. Verify special members, member functions and dedicated friend
functions of a class.

#### functional

Integrational non-unit tests for key functionality, e.g. whether ARCSs are
correctly calculated or dBAR-files are correctly parsed.


### Tags (Catch2)

On Catch2-Level, each ``TEST_CASE`` is augmented with tags, that have the form
"[name]".

In libarcstk, each test has at most the following 4 tags:

  - Class name or function name in lowercase letters.
  - Module name: calc, verify, id, dbar, metadata, checksum, samples, logging.
  - Public header name (optional): calculate, verify, dbar, metadata, samples.
    (Module names are also header names with the exceptions of 'calc' and 'id'.)
  - Tag ``[details]`` for implementation details regardless of the concrete
    namespace of the tested unit.

This binary can be called with option ``--list-tags`` to list all tags or with
option ``--list-tests`` to get a list of all tests. Tags are passed as a string
like ``[verify]``, so ``./libarcstk_unit_tests --list-tests "[calc]"`` lists all
tests tagged as ``calc`` while
``./libarcstk_functional_tests -s "[perform_update]"`` executes all tests
tagged with ``[perform_update]``. Option ``--help`` is implemented and the
entire documentation is found [2][here].


[1]: https://cmake.org/cmake/help/book/mastering-cmake/chapter/Testing%20With%20CMake%20and%20CTest.html
[2]: https://github.com/catchorg/Catch2/blob/devel/docs/command-line.md


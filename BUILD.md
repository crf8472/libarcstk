# How to Build



## Quickstart

We presuppose you have downloaded and unpacked or git-cloned libarcstk to a
local folder named ``libarcstk``. Thereafter do:

	$ cd libarcstk       # your libarcstk root folder where README.md resides
	$ mkdir build && cd build  # create build folder for out-of-source-build
	$ cmake ..           # configure for Release
	$ cmake --build .    # perform the actual build
	$ sudo make install  # install to /usr/local

This will just build and install libarcstk with all local optimizations and
without debug-symbols, tests and documentation. You will be able to use it in
your project.



## Building libarcstk on Linux and \*BSD

Libarcstk is compiled as C++17 since release 0.3. It was developed mainly (but
not exclusively) for Linux and has no runtime dependencies other than the C++
standard library. It was not tested whether libarcstk builds out-of-the-box on
BSDs but don't expect major issues.


### Mandatory Buildtime Dependencies

- C++-17-compliant-compiler with C++ standard library (e.g. g++ or clang++)
- ``cmake`` >= 3.10
- ``make``, ``ninja`` or some other build tool compatible to cmake


### Optional Buildtime Dependencies

If you intend to run the tests or build the documentation, there are some
more dependencies required.

|Tool                  |Task                   | Description                   |
|----------------------|-----------------------|-------------------------------|
|**Git**               |Testing, Documentation |Clone test framework [Catch2][2] and site generator [m.css][3] |
|**Doxygen**           |Documentation          |Build documentation in HTML (graphviz/dot is not required) |
|**virtualenv**/Python |Documentation          |Build documentation in HTML styled with [m.css][3] |
|**LaTeX**             |Documentation          |Build documentation manual     |
|**libsndfile**, **libcue** |Examples          |When building the examples     |

### Installed files

The following 20 files (not including softlinks) will be installed to your
system:

- The shared object ``libarcstk.so.x.y.z`` (along with a symbolic link
  ``libarcstk.so``) in the standard library location (e.g. ``/usr/local/lib``).
- The 16 public header files ``accuraterip.hpp``, ``algorithm.hpp``,
  ``algorithms.hpp``, ``bytes.hpp``, ``calculate.hpp``, ``checksum.hpp``,
  ``dbar.hpp``, ``identifier.hpp``, ``libarcstk.hpp``, ``logging.hpp``,
  ``loglevel.hpp``, ``metadata.hpp``, ``mixins.hpp``, ``samples.hpp``,
  ``verify.hpp``, and ``version.hpp`` in the subfolder ``arcstk`` in the default
  include location (e.g. ``/usr/local/include``).
- The 4 cmake packaging files ``libarcstk-config.cmake``,
  ``libarcstk-config-version.cmake``, ``libarcstk-targets.cmake`` and
  ``libarcstk-targets-release.cmake`` in directory ``libarcstk`` beneath the
  default cmake location (e.g. ``/usr/local/lib/cmake``). Those files allow
  other projects to simply import libarcstk's exported cmake targets.
- The pkg-config configuration file ``libarcstk.pc`` in the default pkgconfig
  location (e.g. ``/usr/local/lib/pkgconfig``).

The default installation prefix can be changed by passing the actual prefix to
cmake. This is achieved by using the switch
``-DCMAKE_INSTALL_PREFIX=/path/to/install/dir`` in the configure step. See
[Configure switches](#configure-switches) for more configuration options.

We describe the build configuration for the following profiles:
- [User](#users) (read: a developer who uses libarcstk in her project)
- [Contributing developer](#contributors) (who intends to debug and test
  libarcstk and maybe contribute to the documentation)
- [Package maintainer](#package-maintainers) (who intends to package libarcstk
  for some target system).


### Users

You intend to install libarcstk on your system, say, as a dependency for your
own project. You just need libarcstk to be available along with its headers and
not getting in your way:

	$ cmake -DCMAKE_BUILD_TYPE=Release ..
	$ cmake --build .
	$ sudo make install


### Contributors

You want to debug into the libarcstk code, hence you need to build libarcstk
*with* debugging symbols and *without* aggressive optimization:

	$ cmake -DCMAKE_BUILD_TYPE=Debug ..

For also building and running the tests, just use the corresponding switch:

	$ cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON ..

Thereafter just start the build and run the tests:

	$ cmake --build .
	$ ctest

Note: This build will take *significantly longer* than the build without
tests.


### Package maintainers

You want to build libarcstk with a release profile but without any architecture
specific optimization (e.g. without ``-march=native`` and ``-mtune=generic`` for
g++ or clang++).

Furthermore, you would like to adjust the install prefix path such that
libarcstk is configured for being installed in the real system prefix (such as
``/usr``) instead of some default prefix (such as ``/usr/local``).

You may also want to specify a staging directory as an intermediate install
target.

When using clang++ or g++, all of these can be achieved as follows:

	$ cmake -DCMAKE_BUILD_TYPE=Release -DWITH_NATIVE=OFF -DCMAKE_INSTALL_PREFIX=/usr ..
	$ cmake --build .
	$ make DESTDIR=/my/staging/dir install

**Note** that ``-DWITH_NATIVE=OFF`` currently only works for clang++ and g++.
The build process is *untested* and *broken* on other compilers.

If you use another compiler than clang++ or g++, CMake will not apply any
project specific modifications to the compiler default settings. Therefore, you
have to carefully inspect the build process (e.g. by using ``$ make VERBOSE=1``
instead of ``cmake --build .``) to verify which compiler settings are actually
used.


### Configure switches

|Switch              |Description                                     |Default|
|--------------------|------------------------------------------------|-------|
|CMAKE_BUILD_TYPE    |Build type for release or debug             |``Release``|
|CMAKE_INSTALL_PREFIX|Top-level install location prefix   |*plattform defined*|
|CMAKE_CXX_COMPILER  |Choose compiler                                  |g++   |
|CMAKE_EXPORT_COMPILE_COMMANDS|Rebuild compilation database when configuring |ON    |
|USE_DOC_TOOL        |Set 'MCSS' to [use m.css](#website-mcss-with-html5-and-css3-via-doxygens-xml) to build the documentation. Set 'LUALATEX' to build the manual (experimental). | *none* |
|WITH_DOCS           |Configure for [documentation](#building-the-api-documentation)                                     |OFF    |
|WITH_NATIVE         |Use platform [specific optimization](#turn-optimizing-onoff) on compiling                          |       |
|                    |CMAKE_BUILD_TYPE=Debug                                                                             |OFF    |
|                    |CMAKE_BUILD_TYPE=Release                                                                           |ON     |
|WITH_TESTS          |Compile [tests](#run-unit-tests) (but don't run them)                                              |OFF    |
|WITH_EXAMPLES       |Compile [examples](#examples)                                                                      |OFF    |

Note that ``USE_DOC_TOOL`` can be passed multiple values. For example, building
the HTML version as well as the manual in one build run is achieved by:

	$ cmake -DUSE_DOC_TOOL=MCSS\;LUALATEX ..


### Switch between clang++ and g++

Libarcstk is tested to compile with clang++ as well as with g++, where the
platform default is used. (Compilation is known to work with mingw but this path
is not yet fully supported.)

Delete your directory ``build`` since it contains metadata from the previous
compiler. Start off cleanly.

	$ cd ..
	$ rm -rf build

CMake-reconfigure the project to have the change take effect:

	$ mkdir build && cd build
	$ cmake -DCMAKE_CXX_COMPILER=clang++ ..

You may combine this with more of the above compile switches according to your
needs.

To check whether your setting took effect, observe the CMake output. During the
configure step, CMake informs about the actual C++-compiler like:

	-- The CXX compiler identification is Clang 19.1.7
	...
	-- Check for working CXX compiler: /usr/bin/clang++ - works


### Building as a git-submodule

Libarcstk supports being build as a submodule. However, there is a usual caveat:
If the parent uses any of the switches that affect build on its own, e.g.
``WITH_TESTS`` or ``WITH_EXAMPLES``, then the parent is responsible for whether
the libarcstk submodule should see (and therefore react on) those switches. For
example, if the parent should be tested without also testing the libarcstk
submodule, the switch must be handled appropriately by the parent.


### Turn optimizing on/off

You may or may not want the ``-march=native`` and ``-mtune=generic`` switches on
compilation. For Debug-builds, they are ``OFF`` by default, but can be added by
using ``-DWITH_NATIVE=ON``. For now, this switch has only influence when using
g++ or clang++. For other compilers, default settings apply.


### Run tests

Note that ``-DWITH_TESTS=ON`` will try to git-clone the testing framework
[Catch2][2] within your ``build`` directory and fail if this does not work.

Running the unit tests is *not* part of the build process. To run the tests,
invoke ``ctest`` manually in the ``build`` directory after ``cmake --build .``
is completed.

Note that ctest will write report files in the folder ``build/reports``. Their
name pattern is ``report.<testcase>.xml`` where ``<testcase>`` corresponds to a
Catch2 TEST_CASE name.

Many but not all parts of libarcstk are tested. There are three categories of
tests:

  - Some but still few functional tests that will fail when the calculation of
    checksums is not correct.
  - Unit tests for most of the API artefacts libarcstk offers.
  - For currently some of the classes, their type traits are tested (e.g.
	whether the special members work and they have the members they should
	have).


### Examples

For each of the four basic tasks - verifiying ARCSs, calculating ARCSs,
calculating ARIds, parsing DBAR files - there is a code example within the
directory ``examples``.

Note that the examples ``albumid`` and ``albumcalc`` rely on processing audio
and metadata input and therefore introduce dependencies to libcue and
libsndfile. Those are dependencies of these particular examples, not of the
libarcstk library itself - libarcstk has no external dependencies.

**Important:** the examples will *always* link against the libarcstk binary in
directory ``build``. It is therefore NOT supported to just use them independent
from the libarcstk build process.

#### Building during build phase

You may build those examples by configuring ``-DWITH_EXAMPLES=ON`` before
building. After building, you will find the binaries in
``build/examples/<NAME>``.

#### Building manually

Alternatively each example can be built separately by cd'ing into its directory
and just issuing ``make``. Note that the libarcstk binary has to be available in
the build directory, so build libarcstk before building the examples manually.

For example, to build the albumcalc example:

	$ cd examples/albumcalc
	$ make clean && make
	$ ./albumcalc

#### Cleaning the examples

If the examples were built by the regular build phase, it is sufficient to just
remove the corresponding subdirectory:

	$ rm -r build/examples

If the examples were built manually, the current example folder can be cleaned
by:

	$ make clean


### Cleaning the project

Clean only the shared library binaries (when in directory ``build``):

	$ cmake -P CMakeFiles/libarcstk.dir/cmake_clean.cmake

Clean the project entirely:

	$ cmake --build . --target clean

Note that this forces to recompile everything including Catch2 if
``-DWITH_TESTS`` is configured.

Completely wipe everything configured and built locally (when in top-level
directory):

	$ rm -rf build



## Building the API documentation

When you configure the project, switch ``-DWITH_DOCS=ON`` is required to prepare
building the documentation. Only this configuration option will create the
target ``doc`` that can build the documentation.

Doxygen is required for building the documentation in either case.

The documentation can be build as a set of static HTML pages (recommended) or as
a PDF manual using LaTeX (experimental, very alpha).

When building HTML, you may choose between the following options:
  - stock HTML output of doxygen (the default)
  - HTML output styled by [m.css][3]


### Website: Doxygen Stock HTML

The generation of the documentation sources must be requested at configuration
stage. The documentation sources will not be generated automatically during
build. It is required to call target ``doc`` manually.

	$ cd build
	$ cmake -DWITH_DOCS=ON ..
	$ cmake --build . --target doc

This will build the documentation sources for HTML in subdirectories of
``build/generated-docs/doxygen``. Open the file
``build/generated-docs/doxygen/html/index.html`` in your browser to see the
entry page.


### Website: m.css with HTML5 and CSS3 via doxygen's XML

Accompanying [m.css][3] comes a doxygen style. It takes the doxygen XML output
and generates a static site in plain HTML5 and CSS3 from it (nearly without
JavaScript).

The [public APIdoc of libarcstk is build with m.css][5].

This APIdoc can be built locally by the following steps (when in folder
``libarcstk``):

	$ cmake -B build -DWITH_DOCS=ON -DUSE_DOC_TOOL=MCSS
	$ cmake --build build --target doc

CMake then creates a local python sandbox in directory ``build`` using
``virtualenv``, installs jinja2 and Pygments in it, then clones [m.css][3], and
then runs m.css which internally runs doxygen. Maybe this process needs
finetuning for some environments. (It is completely untested on Windows and will
not work.)

Documentation is generated in ``build/generated-docs/mcss`` and you can
load ``build/generated-docs/mcss/html/index.html`` in your browser.

Gratitude for m.css is owed to [mozra][3].


### Manual: PDF by LaTeX (smoke-tested, more or less)

Libarcstk is supposed to provide support for a PDF manual using LaTeX at some
point in the future. Currently, it is possible to create a manual for just
working on the process. Do not expect satisfying results.

An actual LaTeX installation (containing ``lualatex`` and ``epstopdf``) is
required for creating the manual.

Building the PDF manual can be requested by using ``-DUSE_DOC_TOOL=LUALATEX``.
It will thereby be typeset by building the ``doc`` target.

The entire process:

	$ cmake -B -DWITH_DOCS=ON -DUSE_DOC_TOOL=LUALATEX
	$ cmake --build build --target doc

This will create the document
``build/generated-docs/doxygen/lualatex/refman.pdf`` (while issuing loads of
warnings, which is perfectly normal).

Note that I did never give any love to the manual. It will build. Not more.
However, it will not be convenient to read or look good at its current stage.


### Cleaning the documentation

Distclean the documentation by removing the generated markdown pages and
generated documentation output:

	$ rm -rf build/doc/texts/ build/generated-docs/ && cmake --build build --target doc


## Compilation database

The generation of a compilation database is ON in the default CMake
configuration. The compilation database is in file
``build/compile_commands.json`` that is generated by CMake during the
configuration phase.

It may or may not be required on your system to link the compilation database
file to the project root directory to make specific use cases work:

	$ ln -s build/compile_commands.json . # May or may not be required

The compilation database is not only required for LSP servers (and therefore
deep language support by clang++) but also for some of the targets supporting
development, like clang-tidy and iwyu.

However, if you do not require the compilation database, say, because you do not
intend to do any development, it is strongly advised to just ignore it and leave
it as it is. Do not turn it off except you know what you are doing and for good
reasons.


## Targets to support development

### Coverage report

For analyzing and improving the coverage of the tests, use

	$ cmake --build build --target libarcstk_coverage

to generate code coverage reports. Either gcovr or lcov is required to generate
coverage reports. If both tools are available, gcovr is used. In either case
after building the target the coverage report is generated in
``build/coverage-report/index.html``.

### Clang-tidy analysis report

For static analysis of the entire codebase with clang-tidy, use

	$ cmake --build build --target libarcstk_clang-tidy

to generate a report by clang-tidy. The binary ``clang-tidy`` is required to be
available in the ``PATH``. After building the target, the report is generated
in ``build/libarcstk_clang-tidy_report.txt``. It contains ASCII SGR codes,
view it with ``less`` to have them rendered.

### Dependency graph for header ``#include``-relationships

To visualize the dependencies between C++ headers, use

	$ cmake --build build --target libarcstk_include-graph

to generate a dependency graph in PNG format that shows the dependencies between
C++ headers. [ClangIncludeGraph][6] as well as GraphViz with dot are required to
generate the dependency graph for C++ headers. After building the target, the
dependency graph is generated as
``build/clang_include_graph/libarcstk_includes.png``.

### Dependency graph for CMake targets

To visualize the dependencies between CMake custom targets, use

	$ cmake --build build --target libarcstk_target-deps

to generate a dependency graph in PNG format that shows the dependencies between
custom targets. GraphViz with dot is required to generate the dependency graph
for CMake custom targets. After building the target, the dependency graph is
generated as ``build/graphviz/libarcstk_targets.png``.


## Build on Windows

The default release-building process on Windows with mingw is possible if the
mandatory buildtime dependencies are available. The tests run successful
thereafter. No further work has been done on that.


[1]: https://include-what-you-use.org/
[2]: https://github.com/catchorg/Catch2
[3]: https://mcss.mosra.cz/doxygen/
[4]: https://jothepro.github.io/doxygen-awesome-css/
[5]: https://crf8472.github.io/libarcstk/current/
[6]: https://github.com/bkryza/clang-include-graph


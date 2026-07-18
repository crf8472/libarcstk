# How to Build



## Quickstart

We presuppose you have downloaded and unpacked or git-cloned libarcstk to a
local folder named ``libarcstk``. Thereafter do:

	$ cd libarcstk       # your libarcstk root directory where README.md resides
	$ cmake -B build     # generate directory 'build', configure a release build
	$ cmake --build build
	$ cd build
	$ sudo make install  # installs to /usr/local

This will just build and install libarcstk with all local optimizations and
without debug-symbols, tests and documentation. You will be able to use it in
your project thereafter.



## Building libarcstk on Linux and \*BSD

Libarcstk is compiled as C++17 since release 0.3. It has no runtime dependencies
other than the C++ standard library. It was not tested whether libarcstk builds
out-of-the-box on BSDs but don't expect major issues. It builds correctly on
MacOS and Windows but is completely untested on these platforms for now.


### Mandatory Buildtime Dependencies

- C++-17-compliant-compiler with C++ standard library (e.g. g++ or clang++)
- ``cmake`` >= 3.10
- ``make``, ``ninja`` or some other build tool compatible to cmake


### Optional Buildtime Dependencies

If you intend to run the tests or build the documentation, there are some
more dependencies required.

|Component             |Task                   | Description                   |
|----------------------|-----------------------|-------------------------------|
|**Git**               |Testing, Documentation |Clone test framework [Catch2][2] and site generator [m.css][3] |
|**Doxygen**           |Documentation          |Build documentation in HTML (graphviz/dot is not required) |
|**virtualenv**/Python |Documentation          |Build documentation in HTML styled with [m.css][3] |
|**LaTeX**             |Documentation          |Build documentation manual     |
|**libsndfile**, **libcue** |Examples          |When building the examples     |

### Installed files

The following 22 files (not including softlinks) will be installed to your
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

Remember that the default installation prefix can be changed by passing the
actual prefix to cmake. This is achieved by using the switch
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

	$ cd libarcstk   # where README.md resides
	$ cmake -B build -DCMAKE_BUILD_TYPE=Release
	$ cmake --build build
	$ sudo cmake --install build   # install to /usr/local


### Contributors

You want to debug into the libarcstk code, hence you need to build libarcstk
*with* debugging symbols and *without* aggressive optimization:

	$ cd libarcstk   # where README.md resides
	$ cmake -B build -DCMAKE_BUILD_TYPE=Debug

For also building and running the tests, just use the corresponding switch:

	$ cmake -B build -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON

Thereafter just start the build and run the tests:

	$ cmake --build build
	$ ctest --test-dir build

Note: This build will take *significantly longer* than the build without tests.


### Package maintainers

You intend to build libarcstk with a release profile.

Furthermore, you would like to adjust the install prefix such that libarcstk is
configured for being installed in the real system path (such as ``/usr``)
instead of some default installation path (such as ``/usr/local``).

You may also want to specify a staging prefix as an intermediate install
target directory.

When using clang++ or g++ pass the installation prefix as follows:

	$ cd libarcstk   # where README.md resides
	$ cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/usr
	$ cmake --build build

Then choose the staging prefix while preserving the paths for the installation
prefix:

	$ cd build
	$ make DESTDIR=/my/staging/dir install

If for some reason you decide to ship libarcstk without optimization for the
target architecture (e.g. without ``-march=native`` and ``-mtune=generic`` for
g++ or clang++) add ``-DWITH_NATIVE=OFF`` when configuring.

**Note** that ``-DWITH_NATIVE=OFF`` currently only works for clang++ and g++.
The build process is *untested* and *broken* on other compilers.

If you use another compiler than clang++ or g++, CMake will not apply any
project specific modifications to the compiler default settings. Therefore, you
have to carefully inspect the build process (e.g. by using ``$ cmake --build
build --verbose`` or ``$ make VERBOSE=1`` instead of ``cmake --build build``) to
verify which compiler settings and build commands are actually used.


### Configure switches

|Switch              |Description                                     |Default|
|--------------------|------------------------------------------------|-------|
|CMAKE_BUILD_TYPE    |Build type for release or debug             |``Release``|
|CMAKE_INSTALL_PREFIX|Top-level install location prefix    |*platform defined*|
|CMAKE_CXX_COMPILER  |Choose compiler                      |*platform defined*|
|CMAKE_EXPORT_COMPILE_COMMANDS|Rebuild compilation database when configuring |ON    |
|USE_DOC_TOOL        |Set 'MCSS' to [use m.css](#website-mcss-with-html5-and-css3-via-doxygens-xml) to build the documentation. Set 'LUALATEX' to build the manual (experimental). | *none* |
|WITH_DOCS           |Configure for [documentation](#building-the-api-documentation)               |OFF    |
|WITH_NATIVE         |Use platform [specific optimization](#turn-optimizing-onoff) on compiling    |       |
|                    |CMAKE_BUILD_TYPE=Debug                                                       |OFF    |
|                    |CMAKE_BUILD_TYPE=Release                                                     |ON     |
|WITH_TESTS          |Compile [tests](#run-unit-tests) (but don't run them)                        |OFF    |
|WITH_EXAMPLES       |Compile [examples](#examples), requires libsndfile and libcue                |OFF    |

Note that ``USE_DOC_TOOL`` can be passed multiple values. For example, building
the HTML version as well as the manual in one build run is achieved by:

	$ cmake -B build -DWITH_DOCS=ON -DUSE_DOC_TOOL=MCSS\;LUALATEX

Specifying ``USE_DOC_TOOL`` without ``WITH_DOCS=ON`` results in a build error.

Note further that ``WITH_EXAMPLES`` pulls in the additional buildtime
dependencies libsndfile and libcue.


### Switch between clang++ and g++

Libarcstk is tested to compile with clang++ as well as with g++. (Compilation is
known to work with mingw but this path is untested and currently unsupported.)

Delete your directory ``build`` since it contains metadata from the previous
compiler. Start off cleanly.

	$ cd libarcstk   # where README.md resides
	$ rm -rf build

CMake-reconfigure the project to have the change take effect:

	$ cmake -B build -DCMAKE_CXX_COMPILER=clang++

You may combine this with more of the above compile switches according to your
needs.

To check whether your setting took effect, observe the CMake output. During the
configure step, CMake informs about the actual C++-compiler like:

	-- The CXX compiler identification is Clang 19.1.7
	...
	-- Check for working CXX compiler: /usr/bin/clang++ - works


### Building as a git-submodule

Libarcstk supports being build as a submodule. However, there is a usual caveat:
If the parent uses any of the switches that affect build, e.g. ``WITH_TESTS`` or
``WITH_EXAMPLES`` on its *own*, then the parent is also responsible for whether
the libarcstk submodule should see (and therefore react on) those switches.

For example, if the parent should be tested without also testing the libarcstk
submodule, the switch ``WITH_TESTS`` must be handled appropriately by the
parent.


### Turn optimizing on/off

You may or may not want the ``-march=native`` and ``-mtune=generic`` switches on
compilation. For Debug-builds, they are ``OFF`` by default, but can be added by
using ``-DWITH_NATIVE=ON``. For now, this switch has only influence when using
g++ or clang++. For other compilers, default settings apply.


### Run tests

Note that ``-DWITH_TESTS=ON`` will try to git-clone the testing framework
[Catch2][2] within your ``build`` directory and fail if this does not work.

Running the unit tests is *not* part of the build process. You have to run the
tests after ``cmake --build build`` is completed:

	$ ctest --test-dir build

Note that ctest will write report files in the folder ``build/reports``. Their
name pattern is ``report.<testcase>.xml`` where ``<testcase>`` corresponds to a
Catch2 ``TEST_CASE`` name.

Many but not yet all parts of libarcstk are tested. There are three categories
of tests:

  - Functional tests: Some but still few functional tests that will fail when
	the calculation of checksums is not correct.
  - Unit tests: for most of the API artefacts libarcstk offers.
  - Type traits tests: for currently some of the classes, their type traits are
	proven (e.g. whether the special members work, they have the members they
	should have, they are trivially constructible and so on).


### Cleaning the project

Clean the build artifacts but not the CMake auxiliary files:

	$ cd libarcstk   # where README.md resides
	$ cmake --build build --target clean

Do a "distclean", i.e. completely wipe everything configured and built locally:

	$ rm -rf build

Note that distcleaning forces to recompile everything including Catch2 if
``-DWITH_TESTS`` is configured.


### Examples

For each of the four basic tasks - verifiying ARCSs, calculating ARCSs,
calculating ARIds, parsing DBAR files - there is a code example within the
directory ``examples``.

You may build those examples by configuring ``-DWITH_EXAMPLES=ON`` before
building. After building, you will find the binaries in
``build/examples/<NAME>``.

Note that the examples ``albumid`` and ``albumcalc`` rely on processing audio
and metadata input and therefore introduce dependencies to libsndfile and
libcue. Those are dependencies of these particular examples, not of the
libarcstk library itself - libarcstk has no external dependencies.

**Important:** the examples will *always* link against the libarcstk binary in
directory ``build``. It is therefore NOT supported to just use the example
binaries independent from the libarcstk build process! Do not package them! Do
not expect them to work after a rebuild when new commits have arrived! Just
consider them as a part of a specific build process - like log files.

To clean the examples, just remove the corresponding subdirectory:

	$ rm -rf build/examples

#### Building and cleaning the examples manually (deprecated)

Alternatively each example can be built separately by cd'ing into its directory
and just issuing ``make``. Note that the libarcstk binary has to be available in
the build directory, so build libarcstk before building the examples manually.

For example, to build the albumcalc example:

	$ cd examples/albumcalc
	$ make clean && make
	$ ./albumcalc

The current example folder can be cleaned by:

	$ make clean

Building the examples manually is deprecated since it does not make use of
modern RPATH handling and is not build-tool independent. The old makefiles that
provide this mechanism may be removed any time soon.


## Building the API documentation

Documentation is requested when configuring by switch ``-DWITH_DOCS=ON``. This
configuration option will create target ``doc`` that builds the documentation
when requested.

The documentation sources will not be generated automatically during build. It
is required to build target ``doc`` explicitly:

	$ cd libarcstk   # where README.md resides
	$ cmake -B build -DWITH_DOCS=ON     # default: doxygen's stock HTML
	$ cmake --build build --target doc

The documentation can be build as a set of static HTML pages (recommended) or as
a PDF manual using LaTeX (experimental, very alpha).

When building HTML pages, you may choose one of the following options that are
mutually exclusive in the same build:
  - stock HTML output of doxygen (the default)
  - HTML output styled by [m.css][3]

These three alternative scenarios to build documentation can be selected by
using ``-DUSE_DOC_TOOL``:

|USE_DOC_TOOL      |Tools    |Output        |Status                 |
|------------------|---------|--------------|-----------------------|
|<empty> (default) |doxygen  |HTML pages    |supported              |
|MCSS              |m.css    |HTML pages    |experimental, tested   |
|LUALATEX          |lualatex |PDF manual    |experimental, untested |

Note that ``USE_DOC_TOOL`` can be passed multiple values to build multiple
scenarios like:

	$ cmake -B build -DWITH_DOCS=ON -DUSE_DOC_TOOL=MCSS\;LUALATEX

Only scenarios with different output can be combined in the same build.

Doxygen is required for building the documentation in either case.

For using m.css, a python installation with virtualenv is required. For using
lualatex, you need some LaTeX-distro installed and the lualatex binary must be
in the ``PATH``.


### HTML pages: Doxygen Stock HTML

The default. Just use:

	$ cd libarcstk   # where README.md resides
	$ cmake -B build -DWITH_DOCS=ON
	$ cmake --build build --target doc

This will build the documentation sources for HTML in subdirectories of
``build/generated-docs/doxygen``. Open the file
``build/generated-docs/doxygen/html/index.html`` in your browser to see the
entry page.


### HTML pages: m.css with HTML5 and CSS3 via doxygen's XML

Accompanying [m.css][3] comes a doxygen style. It takes the doxygen XML output
and generates a static site in plain HTML5 and CSS3 from it (nearly without
JavaScript).

For using m.css, a python installation with virtualenv is required. You also
need doxygen to create XML documents from the documentation sources.

The [public APIdoc of libarcstk is build with m.css][5].

This APIdoc can be built locally by the following steps:

	$ cd libarcstk   # where README.md resides
	$ cmake -B build -DWITH_DOCS=ON -DUSE_DOC_TOOL=MCSS
	$ cmake --build build --target doc

CMake then creates a local python sandbox in directory ``build`` using
``virtualenv``, installs jinja2 and Pygments in it, then clones [m.css][3], and
then runs m.css which internally runs doxygen. Maybe this process needs
finetuning for some environments. (It is completely untested on Windows and will
not work.)

Documentation is generated in ``build/generated-docs/mcss`` and you can
load ``build/generated-docs/mcss/html/index.html`` in your browser.

Libarcstk uses m.css for years now. Gratitude for m.css is owed to [mozra][3].


### Manual: PDF by LaTeX (smoke-tested, more or less)

Libarcstk is supposed to provide support for a PDF manual using LaTeX at some
point in the future. Currently, it is possible to create a manual for just
working on the process. Do not expect satisfying results.

An actual LaTeX installation (containing ``lualatex`` and ``epstopdf``) is
required for creating the manual.

Building the PDF manual can be requested by using ``-DUSE_DOC_TOOL=LUALATEX``.
It will thereby be typeset by building the ``doc`` target.

The entire process:

	$ cd libarcstk   # where README.md resides
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

	$ rm -rf build/doc/texts/ build/generated-docs/


## Compilation database

The generation of a compilation database is ON in the default CMake
configuration. The compilation database is in file
``build/compile_commands.json``.

It may or may not be required on your system to link the compilation database
file to the project root directory to make specific use cases work:

	$ cd libarcstk   # where README.md resides
	$ ln -s build/compile_commands.json . # May or may not be required

The compilation database is not only required for LSP servers (and therefore
deep language support by clang++) but also for some of the targets supporting
development, like clang-tidy and iwyu.

However, if you do not require the compilation database, say, because you do not
intend to do any development, it is strongly advised to just ignore it and leave
it as it is. Do not turn it off except you know what you are doing and for good
reasons.


## Targets for development

The following parts cover some special targets that are only interesting for
contributors. The targets in this section are always available for build and do
not depend on configuration switches.

### Coverage report

For analyzing and improving the coverage of the tests, use

	$ cmake --build build --target libarcstk_coverage

to generate code coverage reports.

Either gcovr or lcov is required to generate coverage reports. If both tools are
available, gcovr is preferred. In either case after building the target the
coverage report is generated in ``build/coverage-report/index.html``.

### Clang-tidy analysis report

For static analysis of the entire codebase with clang-tidy, use

	$ cmake --build build --target libarcstk_clang-tidy

to generate a report by clang-tidy. The report contains ASCII SGR codes. View it
with ``less`` to have them rendered:

	$ less < build/libarcstk_clang-tidy_report.txt

Target ``libarcstk_clang-tidy`` is only successful if no issue is reported. A
warning will be issued if the generated report is a non-empty file.

The binary ``clang-tidy`` is required to be available in the ``PATH``.

### Dependency graph for header ``#include``-relationships

To visualize the dependencies between C++ headers, use

	$ cmake --build build --target libarcstk_include-graph

to generate a dependency graph in PNG format that shows the dependencies between
C++ headers. After building the target, the dependency graph is generated as
``build/clang_include_graph/libarcstk_includes.png``.

[ClangIncludeGraph][6] as well as GraphViz with dot are required to generate the
include graph for C++ headers.

### Dependency graph for CMake targets

To visualize the dependencies between CMake custom targets, use

	$ cmake --build build --target libarcstk_target-deps

to generate a dependency graph in PNG format that shows the dependencies between
custom targets. After building the target, the dependency graph is generated as
``build/graphviz/libarcstk_targets.png``.

GraphViz with dot is required to generate the dependency graph for CMake custom
targets.


## Build on other platforms

### Windows

The default release-building process on Windows with mingw is possible if the
mandatory buildtime dependencies are available. The tests run successful
thereafter. No further work has been done on that.

### MacOS

The default release-building process on MacOS 15 (arm64) with clang++ 17 is
possible if the mandatory buildtime dependencies are available. The tests run
successful thereafter. No further work has been done on that.


[1]: https://include-what-you-use.org/
[2]: https://github.com/catchorg/Catch2
[3]: https://mcss.mosra.cz/doxygen/
[4]: https://jothepro.github.io/doxygen-awesome-css/
[5]: https://crf8472.github.io/libarcstk/current/
[6]: https://github.com/bkryza/clang-include-graph


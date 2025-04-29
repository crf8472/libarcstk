# How to Build



## Quickstart

We presuppose you have downloaded and unpacked or git-cloned libarcstk to a
folder named ``libarcstk``. Thereafter do:

	$ cd libarcstk       # your libarcstk root folder where README.md resides
	$ mkdir build && cd build  # create build folder for out-of-source-build
	$ cmake ..           # configure for Release
	$ cmake --build .    # perform the actual build
	$ sudo make install  # install to /usr/local

This will just build and install libarcstk with all local optimizations and
without debug-symbols, tests and documentation. You will be able to use it in
your project.



## Building libarcstk on Linux and \*BSD

Libarcstk >= 0.3 is compiled as C++17. It was developed mainly (but not
exclusively) for Linux and has no runtime dependencies other than the C++
standard library. It was not tested whether libarcstk builds out-of-the-box on
BSDs but don't expect major issues.


### Mandatory Buildtime Dependencies

- C++-17-compliant-compiler with C++ standard library
- ``cmake`` >= 3.10
- ``make`` or some other build tool compatible to cmake (the examples suppose
  ``make`` nonetheless)


### Optional Buildtime Dependencies

If you intend to run the tests or build the documentation, there are some
more dependencies required.

|Tool                  |Task                   | Description                   |
|----------------------|-----------------------|-------------------------------|
|**Git**               |Testing, Documentation |Clone test framework [Catch2][2] and site generator [m.css][3] |
|**Doxygen**           |Documentation          |Build documentation in HTML (graphviz/dot is not required) |
|**virtualenv**/Python |Documentation          |Build documentation in HTML styled with [m.css][3] |
|**LaTeX**             |Documentation          |Build documentation manual     |


### Installed files

The following 18 files will be installed to your system:

- The shared object libarcstk.so.x.y.z (along with a symbolic link
  ``libarcstk.so``) in the standard library location (e.g. ``/usr/local/lib``).
- The 12 public header files ``accuraterip.hpp``, ``algorithms.hpp``,
  ``calculate.hpp``, ``checksum.hpp``, ``dbar.hpp``, ``identifier.hpp``,
  ``logging.hpp``, ``metadata.hpp``, ``policies.hpp``, ``samples.hpp``,
  ``verify.hpp``, and ``version.hpp`` in the the subfolder ``arcstk`` in the
  default include location (e.g. ``/usr/local/include``).
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
|CMAKE_EXPORT_COMPILE_COMMANDS|Rebuild a [compilation database](#deep-language-support-in-your-editor) when configuring  |ON     |
|USE_DOC_TOOL        |Set 'MCSS' to use [m.css](#doxygen-by-m-css-with-html5-and-css3-tested-but-still-experimental) to build the documentation. Set 'LUALATEX' to build the manual. | *none* |
|WITH_DOCS           |Configure for [documentation](#building-the-api-documentation)                                     |OFF    |
|WITH_NATIVE         |Use platform [specific optimization](#turn-optimizing-on-off) on compiling                         |       |
|                    |CMAKE_BUILD_TYPE=Debug                                                                             |OFF    |
|                    |CMAKE_BUILD_TYPE=Release                                                                           |ON     |
|WITH_TESTS          |Compile [tests](#run-unit-tests) (but don't run them)                                              |OFF    |

Note that ``USE_DOC_TOOL`` can be passed multiple values. For example, building
the HTML version as well as the manual in one build run is achieved by:

	$ cmake -DUSE_DOC_TOOL=MCSS\;LUALATEX ..


### Switch between clang++ and g++

Libarcstk is tested to compile with clang++ as well as with g++.

If you want to switch the compiler, you should just hint CMake what compiler to
use. On unixoid systems you can usually do this via the environment variables
``CC`` and ``CXX``.

If your current compiler is not clang++ and you want to use your installed
clang++:

	$ export CC=$(type -p clang)
	$ export CXX=$(type -p clang++)

If your current compiler is not g++ and you want to use your installed g++:

	$ export CC=$(type -p gcc)
	$ export CXX=$(type -p g++)

Delete your directory ``build`` since it contains metadata from the previous
compiler. Start off cleanly.

	$ cd ..
	$ rm -rf build

CMake-reconfigure the project to have the change take effect:

	$ mkdir build && cd build
	$ cmake ..

To check whether your setting took effect, observe the CMake output. During the
configure step, CMake informs about the actual C++-compiler like:

	-- The CXX compiler identification is Clang 19.1.7
	...
	-- Check for working CXX compiler: /usr/bin/clang++ - works


### Turn optimizing on/off

You may or may not want the ``-march=native`` and ``-mtune=generic`` switches on
compilation. For Debug-builds, they are ``OFF`` by default, but can be added by
using ``-DWITH_NATIVE=ON``. For now, this switch has only influence when using
g++ or clang++. For other compilers, default settings apply.


### Run unit tests

Note that ``-DWITH_TESTS=ON`` will try to git-clone the testing framework
[Catch2][2] within your ``build`` directory and fail if this does not work.

Running the unit tests is *not* part of the build process. To run the tests,
invoke ``ctest`` manually in the ``build`` directory after ``cmake --build .``
is completed.

Note that ctest will write report files in the ``build`` folder, their name
pattern is ``report.<testcase>.xml`` where ``<testcase>`` corresponds to a
``.cpp``-file in ``test/src``.


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

When building HTML, you may choose either the stock HTML output of doxygen or
the HTML output styled by m.css. Doxygen's stock HTML output is stable but looks
outdated. The m.css-styled seems by far user-friendlier, cleaner and more
adapted for documentation of modern C++. On the other hand it is more cutting
edge and therefore not as stable as doxygen's stock HTML output. Credits for
m.css go to [mozra][3].


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

The [public APIdoc of libarcstk is build with m.css][4].

This APIdoc can be built locally by the following steps:

	$ cd build
	$ cmake -DWITH_DOCS=ON -DUSE_DOC_TOOL=MCSS ..
	$ cmake --build . --target doc

CMake then creates a local python sandbox in directory ``build`` using
``virtualenv``, installs jinja2 and Pygments in it, then clones [m.css][3], and
then runs m.css which internally runs doxygen. Maybe this process needs
finetuning for some environments. (It is completely untested on Windows and will
not work.)

Documentation is generated in ``build/generated-docs/mcss`` and you can
load ``build/generated-docs/mcss/html/index.html`` in your browser.


### Manual: PDF by LaTeX (smoke-tested, more or less)

Libarcstk provides also support for a PDF manual using LaTeX. An actual LaTeX
installation (containing ``lualatex`` and ``epstopdf``) is required for creating
the manual.

Building the PDF manual can be requested by using ``-DUSE_DOC_TOOL=LUALATEX``.
It will therefore be typeset by building the ``doc`` target.

The entire process:

	$ cd build
	$ cmake -DWITH_DOCS=ON -DUSE_DOC_TOOL=LUALATEX ..
	$ cmake --build . --target doc

This will create the manual ``refman.pdf`` in folder
``build/generated-docs/doxygen/lualatex`` (while issuing loads of warnings,
which is perfectly normal).

Note that I did never give any love to the manual. It will build. Not more.
However, it will not be convenient to read or look good at its current stage.


## Using a compilation database

A compilation database provides the dependencies and paths used for building the
project. CDBs are used for deep language support in the ``$EDITOR`` or IDE.

If you intend to use an LSP server (e.g. the one from clang++), the use of the
CDB is encouraged since otherwise the LSP server may not find required paths and
augment your display with artifacts that suggest errors which in fact don't
exist.

You may have noticed that libarcsdec comes with a top-level ``.clang`` file that
already points to ``compile_commands.json`` in the same directory. This prepares
the support for clang-based DLS for libarcsdec. However, the compilation
database is OFF in the default configuration and must be re-built locally for
the local compiler and the local settings:

	$ cd build
	$ cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
	$ cd ..
	$ ln -s build/compile_commands.json . # May or may not be required

The compilation database is now recreated whenever configuring the project.
Its recreation can be forced by just doing:

	$ cd build
	$ cmake ..

If you do not intend to use an LSP server, it is completely safe to skip this
paragraph, ignore the ``.clang`` file and just feel good. It will not get in
your way.


## Build on Windows ... duh!

No Windows port yet :-(

In fact, as a lack of requirement, libarcstk has not yet even been tried to be
built on Windows.

To avoid any show-stoppers for porting libarcstk to Windows or other platforms,
libarcstk relies completely on pure C++ and the C++ standard library. It does
not require any other dependencies. In fact, it is intended to not use platform
specific operations at all. Code that breaks platform independence will be
considered being a bug. The porting is expected not to be difficult, but is
just not done. Help will be appreciated.


[1]: https://include-what-you-use.org/
[2]: https://github.com/catchorg/Catch2
[3]: https://mcss.mosra.cz/doxygen/
[4]: https://crf8472.github.io/libarcstk/current/


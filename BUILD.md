# How to Build



## Building libarcstk on Linux and \*BSD

Libarcstk is compiled as C++17, originally written in C++14. It was developed
mainly (but not exclusively) for Linux and has no runtime dependencies other
than the C++ standard library. It was not tested whether libarcstk builds
out-of-the-box on BSDs but don't expect major issues.



## Buildtime dependencies


### Mandatory Buildtime Dependencies

- C++-17-compliant-compiler with C++ standard library
- ``cmake`` >= 3.10
- ``make`` or some other build tool compatible to cmake (the examples suppose
  ``make`` nonetheless)

### Optional Buildtime Dependencies

- git - for testing: to clone test framework [Catch2][2] as an external project
  when running the unit tests. For building the documentation with
  [m.css][3] (instead of stock doxygen) to clone m.css.
- Doxygen - for documentation: to build the API documentation in HTML
  (graphviz/dot is not required)
- Python (with virtualenv) - for documentation: to build the documentation in
  HTML styled with [m.css][3]
- LaTeX (TeXLive for instance) - for documentation: to build the documentation
  in LaTeX


## Building the library

We presuppose you have downloaded and unpacked or git-cloned libarcstk to a
folder named ``libarcstk``. Then do:

	$ cd libarcstk         # your libarcstk root folder where README.md resides
	$ mkdir build && cd build  # create build folder for out-of-source-build
	$ cmake -DCMAKE_BUILD_TYPE=Release ..   # choose 'Release' or 'Debug'
	$ cmake --build .    # perform the actual build
	$ sudo make install  # installs to /usr/local

This will just install libarcstk with all optimizations and without
debug-symbols and tests.

We describe the build configuration for the following profiles:
- [User](#users) (read: a developer who uses libarcstk in her project)
- [Contributing developer](#contributors) (who wants to debug and test
  libarcstk)
- [Package maintainer](#package-maintainers) (who intends to package libarcstk
  for some target system).


### Using a different compiler

Libarcstk is tested to compile with clang++ as well as with g++.

If you have both and want to switch the compiler, you should just hint CMake
what compiler to use. On many unixoid systems you can do this via the
environment variables ``CC`` and ``CXX``.

If your actual compiler is not clang and you want to use your installed clang:

	$ export CC=$(type -p clang)
	$ export CXX=$(type -p clang++)

If your actual compiler is not g++ and you want to use your installed g++:

	$ export CC=$(type -p gcc)
	$ export CXX=$(type -p g++)

Then, delete your directory ``build`` (which contains metadata from
the previous compiler) to start off cleanly.

	$ cd ..
	$ rm -rf build

CMake-reconfigure the project to have the change take effect:

	$ mkdir build && cd build
	$ cmake ..

During the configure step, CMake informs about the actual C++-compiler like:

	-- The CXX compiler identification is Clang 10.0.0
	...
	-- Check for working CXX compiler: /usr/bin/clang++ - works


### Users

You intend to install libarcstk on your system, say, as a dependency for your
own project. You just want libarcstk to be available along with its headers and
not getting in your way:

	$ cmake -DCMAKE_BUILD_TYPE=Release ..
	$ cmake --build .
	$ sudo make install

This will install the following files to your system:

- the shared object libarcstk.so.x.y.z (along with a symbolic link
  ``libarcstk.so``) in the standard library location (e.g. ``/usr/local/lib``)
- the 8 public header files ``calculate.hpp``, ``identifier.hpp``,
  ``logging.hpp``, ``match.hpp``, ``parse.hpp``, ``policies.hpp``,
  ``samples.hpp`` and ``version.hpp`` in the standard include location
  (e.g. ``/usr/local/include``).
- the 2 exported non-API template headers ``builder.tpp`` and ``validate.tpp``
  in folder ``details`` below the location where the public headers are
- the 4 cmake packaging files ``libarcstk-config.cmake``,
  ``libarcstk-config-version.cmake``, ``libarcstk-targets.cmake`` and
  ``libarcstk-targets-release.cmake`` that allow other projects to simply import
  libarcstk's exported cmake targets
- the pkg-config configuration file ``libarcstk.pc``

You can change the install location by calling cmake with the
``-DCMAKE_INSTALL_PREFIX=/path/to/install/dir`` switch.



### Contributors

You want to debug into the libarcstk code, hence you need to build libarcstk
with debugging symbols and without aggressive optimization:

	$ cmake -DCMAKE_BUILD_TYPE=Debug ..
	$ cmake --build .

For also building and running the tests, just use the corresponding switch:

	$ cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON ..
	$ cmake --build .
	$ ctest

Note: This build will take *significantly* *longer* than the build without
tests.

#### Turn optimizing on/off

You may or may not want the ``-march=native`` and ``-mtune=generic`` switches on
compilation. For Debug-builds, they are ``OFF`` by default, but can be added by
using ``-DWITH_NATIVE=ON``. For now, this switch has only influence when using
gcc or clang. For other compilers, default settings apply.

#### Run unit tests

Note that ``-DWITH_TESTS=ON`` will try to git-clone the testing framework
[Catch2][2] within your ``build`` directory and fail if this does not work.

Running the unit tests is *not* part of the build process. To run the tests,
invoke ``ctest`` manually in the ``build`` directory after ``cmake --build .``
is completed.

Note that ctest will write report files in the ``build`` folder, their name
pattern is ``report.<testcase>.xml`` where ``<testcase>`` corresponds to a
``.cpp``-file in ``test/src``.

#### Find unused header includes

From time to time, I tend to mess up the includes and like to check for unused
includes that can be removed.

CMake brings some support for Google's tool [include-what-you-use][1]. If it is
available in the PATH, CMake can use it on the project:

	$ cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=$(type -p include-what-you-use) ..
	$ cmake --build . 2>iwuy.txt

This runs every source file through inlcude-what-you-use instead of the actual
compiler and writes the resulting analysis to file ``iwyu.txt``. If a real
compiler is to be used again, the project has to be reconfigured.

The tool may log some warnings about unknown compile switches when you have
selected g++ as your actual compiler. This is just because there are some
switches configured for your actual compiler that are unknown to the tool. The
warnings can be ignored. To avoid them
[switch to clang++](#using-a-different-compiler), then configure the project
with ``-DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=...`` pointing to the include...-binary
and run the build again. If include-what-you-use is not in the PATH it is
recommended to specify its location as an absolute path.


### Package maintainers

You want to build libarcstk with a release profile but without any architecture
specific optimization (e.g. without ``-march=native`` and ``-mtune=generic`` for
gcc or clang).

Furthermore, you would like to adjust the install prefix path such that
libarcstk is configured for being installed in the real system prefix (such as
``/usr``) instead of some default prefix (such as ``/usr/local``).

You may also want to specify a staging directory as an intermediate install
target.

When using clang or gcc, all of these can be achieved as follows:

	$ cmake -DCMAKE_BUILD_TYPE=Release -DWITH_NATIVE=OFF -DCMAKE_INSTALL_PREFIX=/usr ..
	$ cmake --build .
	$ make DESTDIR=/my/staging/dir install

*Note* that ``-DWITH_NATIVE=OFF`` currently only works for clang and gcc.

If you use another compiler than clang or gcc, CMake will not apply any project
specific modifications to the compiler default settings. Therefore, you have to
carefully inspect the build process (e.g. by using ``$ make VERBOSE=1``) to
verify which compiler settings are actually used.

By default, the release-build of libarcstk uses -O3. If you intend to change
that, locate the paragraph ``Compiler Specific Settings`` in
[CMakeLists.txt](./CMakeLists.txt) in the root directory and adjust the
settings to your requirements.


## Configure switches

|Switch              |Description                                     |Default|
|--------------------|------------------------------------------------|-------|
|CMAKE_BUILD_TYPE    |Build type for release or debug             |``Release``|
|CMAKE_INSTALL_PREFIX|Top-level install location prefix     |plattform defined|
|CMAKE_EXPORT_COMPILE_COMMANDS|Rebuilds a [compilation database](#deep-language-support-in-your-editor) when configuring |OFF    |
|WITH_DOCS           |Configure for [documentation](#building-the-api-documentation)                     |OFF    |
|WITH_INTERNAL_DOCS  |Configure for [documentation](#building-the-api-documentation) for internal APIs   |OFF    |
|WITH_NATIVE         |Use platform [specific optimization](#turn-optimizing-on-off) on compiling |       |
|                    |CMAKE_BUILD_TYPE=Debug                          |OFF    |
|                    |CMAKE_BUILD_TYPE=Release                        |ON     |
|WITH_TESTS          |Compile [tests](#run-unit-tests) (but don't run them)              |OFF    |
|USE_MCSS            |[Use m.css](#doxygen-by-m-css-with-html5-and-css3-tested-but-still-experimental) when building the documentation. Implies WITH_DOCS=ON  |OFF    |



## Building the API documentation

When you configure the project, switch ``-DWITH_DOCS=ON`` is required to prepare
building the documentation. Only this configuration option will create the
target ``doc`` that can be used to build the documentation.

Doxygen is required for building the documentation.

The documentation can be build as a set of static HTML pages (recommended) or as
a PDF manual using LaTeX (experimental, very alpha).

If you decide to build HTML, you may choose either the stock HTML output of
doxygen or the HTML output styled by m.css. Doxygen's stock HTML output is
stable but looks outdated. The m.css-styled output is much, much more
user-friendly, clean and fit for documentation of modern C++. On the other hand
it is more cutting edge and therefore not as stable as doxygen's stock HTML
output. Credits for the amazing m.css tool go to [mozra][3].


### Quickstart: Doxygen Stock HTML

The generation of the documentation sources must be requested at configuration
stage. The documentation sources will not be generated automatically during
build. It is required to call target ``doc`` manually.

	$ cd build
	$ cmake -DWITH_DOCS=ON ..
	$ cmake --build . --target doc

This will build the documentation sources for HTML as well as LaTeX in
subdirectories of ``build/generated-docs/``. Open the file
``build/generated-docs/html/index.html`` in your browser to see the entry page.


### Doxygen by m.css with HTML5 and CSS3 (tested, but still experimental)

Accompanying [m.css][3] comes a doxygen style. It takes the doxygen XML output
and generates a static site in plain HTML5 and CSS3 from it (nearly without
JavaScript).

The resulting site presents the documentation content very clean and
well structured, using a more contemporary design than the stock doxygen HTML
output. (Which, on the other hand, gives us this warm nostalgic memory of the
Nineties... we loved the Nineties, didn't we?)

The [public APIdoc of libarcstk is build with m.css][4].

This APIdoc can be built locally by the following steps:

	$ cd build
	$ cmake -DWITH_DOCS=ON -DUSE_MCSS=ON ..
	$ cmake --build . --target doc

CMake then creates a local python sandbox in ``build`` with ``virtualenv``,
installs jinja2 and Pygments in it, then clones [m.css][3], and then runs m.css
which internally runs doxygen. Maybe this process needs finetuning for some
environments I did not foresee. (It is completely untested on Windows and may
not work.)

Documentation is generated in ``build/generated-docs/mcss`` and you can
load ``build/generated-docs/mcss/html/index.html`` in your browser.

Note that ``-DUSE_MCSS=ON`` turns off the LaTeX output! You cannot generate
m.css and LaTeX output in the same build.


### Manual: PDF by LaTeX (smoke-tested, more or less)

Libarcstk provides also support for a PDF manual using LaTeX. An actual LaTeX
installation (along with ``pdflatex``) is required for creating the manual.

Building the PDF manual is only available when ``USE_MCSS`` is ``OFF``. Using
``-DUSE_MCSS=ON`` will effectively turn off LaTeX source generation! If you have
previously configured ``USE_MCSS``, just reconfigure your build:

	$ cmake -DWITH_DOCS=ON -DUSE_MCSS=OFF ..

Building the ``doc`` target like in the examples above will create the LaTeX
sources for the PDF manual but will not automatically typeset the actual PDF
document. This requires to change directory to ``build/generated-docs/latex``
and issue ``make``.

The entire process:

	$ cd build
	$ cmake -DWITH_DOCS=ON ..  # Do not use -DUSE_MCSS=ON!
	$ cmake --build . --target doc
	$ cd generated-docs/latex
	$ make

This will create the manual ``refman.pdf`` in folder
``build/generated-docs/latex`` (while issueing loads of ``Underfull \hbox``
warnings, which is perfectly normal).

Note that I did never give any love to the manual. It will build but it will not
be convenient or look good at its current stage!


## Deep language support in your ``$EDITOR``

The project provides a workflow to create a compilation database as a basic
support for what is usually called "deep language support" (DLS).

You may have noticed that libarcstk comes with a top-level ``.clang`` file that
already points to ``compile_commands.json`` in the same directory. This prepares
the support for clang-based DSL for libarcstk, but you have to create the
compilation database on your own, for your compiler and your settings:

	$ cd build
	$ cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
	$ cd ..
	$ ln -s build/compile_commands.json . # May or may not be required

Whenever the compilation process changes - say, a source file is added or
removed or the code changes significantly - you should recreate the compilation
database.

If this all sounds odd for you, it is completely safe to skip this paragraph,
ignore the ``.clang`` file and just feel good. It will not get in your way.


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

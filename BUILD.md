# How to Build



## Building libarcs on Linux and \*BSD

Libarcs is written in C++14. It was developed on Linux with a strong focus on
C++ standard compliance and has no runtime dependencies other than the C++
standard library. It was not tested whether libarcs builds out-of-the-box on
BSDs but don't expect major issues.



## Buildtime dependencies

### Mandatory Buildtime Dependencies

- C++-14-compliant-compiler with C++ standard library
- ``cmake`` >= 3.9.6
- ``make`` or some other build tool compatible to cmake (the examples suppose
  ``make`` nonetheless)

### Optional Buildtime Dependencies

- git - for testing: to clone test framework [Catch2][2] as an external project
  when running the unit tests
- Doxygen - for documentation: to build the API documentation in HTML or LaTeX
- LaTeX (TeXLive for instance) - for documentation: to build the documentation
  in LaTeX
- include-what-you-use - for development: to control ``include``-relationships
  when working on the code


## How to build

The build is quite standard. We presuppose you have downloaded and unpacked or
git-clonde libarcs to a folder ``libarcs``.

The steps for building are then as follows (for Linux and \*nix):

	$ cd libarcs         # your libarcs root folder where README.md resides
	$ mkdir build && cd build  # create build folder for out-of-source-build
	$ cmake -DCMAKE_BUILD_TYPE=Release ..   # configure build type
	$ cmake --build .    # perform the actual build
	$ sudo make install  # installs to /usr/local

This will install libarcs with all optimizations and without debug-symbols and
tests.

The latter three steps may be different for you depending whether you act as
- a user (read: a developer who uses libarcs in her project)
- a contributing developer (who wants to debug and test) or
- as a package maintainer (who intends to package libarcs for some target
  system).

We therefore describe the steps separately for users, contributors and package
maintainers.


### Users

You intend to install libarcs on your system, say, as a dependency for your own
project. You just want libarcs to be available along with its headers, running
fast, remaining small and not getting in your way:

	$ cmake -DCMAKE_BUILD_TYPE=Release ..
	$ cmake --build .
	$ sudo make install

Better not use cmake via sudo, stick to the first-order build tool. If your user
has write permissions in the install directory, you can stick to the cmake-only
version instead:

	$ cmake -DCMAKE_BUILD_TYPE=Release ..
	$ cmake --build . --target install

This will install the following files to your system:

- the shared object libarcs.so.x.y.z (along with a symbolic link ``libarcs.so``)
  in the standard library location (e.g. /usr/local/lib)
- the six public header files calculate.hpp, checksum.hpp, identifier.hpp,
  logging.hpp, match.hpp and parse.hpp in the standard include location
  (e.g. /usr/local/include).
- the pkg-config configuration file libarcs.pc

On \*nix environments, as you guessed, the default install prefix is
``/usr/local`` but you can change this by calling cmake with the
``-DCMAKE_INSTALL_PREFIX=/path/to/install/dir`` switch.


### Contributors

You want to debug into the libarcs code, which may be helpful when working on
the code. You need to build libarcs with debugging symbols and without
aggressive optimization:

	$ cmake -DCMAKE_BUILD_TYPE=Debug ..
	$ cmake --build .
	$ sudo make install

For also building and running the tests, just use the corresponding switch:

	$ cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON ..
	$ cmake --build .
	$ ctest
	$ sudo make install

Note that ``-DWITH_TESTS=ON`` will try to git-clone the testing framework Catch2
within your directory ``build`` and fail if this does not work.

Running the unit tests is *not* part of the build process. The tests have to be
executed manually after the build is completed, just by invoking ``ctest``.

Note that ctest will write report files in the ``build`` folder.

You may or may not want the ``-march=native`` and ``-mtune=native`` switches on
compilation. For Debug-builds, they are not used by default, but can be added by
using ``-DWITH_NATIVE=ON``. For now, this switch has only influence when using
gcc or clang. For other compilers, default settings apply.

For controlling the ``include``-relationships within the code, CMake knows
about Google's tool [include-what-you-use][1]. If you have installed this tool,
you can instruct CMake to create a buildfile for calling include-what-you-use:

	$ cmake -DCMAKE_BUILD_TYPE=Debug -DIWYU=ON ..
	$ cmake --build . 2>iwuy.txt

This runs every source file through inlcude-what-you-use instead of the actual
compiler and writes the resulting analysis to file ``iwyu.txt``.


### Package maintainers

You want to build libarcs with a release profile but without having the compiler
performing any architecture specific optimization (e.g. without
``-march=native`` for gcc or clang).

Furthermore, you would like to adjust the install prefix path such that libarcs
is configured to expect being installed in the real system prefix (such as
``/usr``) instead of some default prefix (such as ``/usr/local``).

Thus, when executing the install target after build, you choose the staging
directory as the intermediate target but you tell libarcs that it may expect to
be installed in another target.

When using clang or gcc, this can be achieved as follows:

	$ cmake -DCMAKE_BUILD_TYPE=Release -DWITH_NATIVE=OFF -DCMAKE_INSTALL_PREFIX=/usr ..
	$ cmake --build .
	$ make DESTDIR=/my/staging/dir install

*Note* that ``WITH_NATIVE=OFF`` currently only works for clang and gcc.

If you use another compiler than clang or gcc, CMake will not apply any project
specific modifications to the compiler default settings. Therefore, you have to
carefully inspect the build process (e.g. by using ``$ make VERBOSE=1``) to
verify which compiler settings are actually used.



## Buildtime switches

|Switch         |Description                                     |Default|
|---------------|------------------------------------------------|-------|
|AS_STATIC      |Build static library instead of shared library  |OFF    |
|WITH_NATIVE    |Use platform specific optimization on compiling |OFF for CMAKE_BUILD_TYPE=Debug, ON for CMAKE_BUILD_TYPE=Release|
|WITH_TESTS     |Compile tests (but don't run them)              |OFF    |
|IWYU           |Use include-what-you-use on compiling           |OFF    |



## Building the documentation

Libarcs provides support for a HTML documentation and a PDF manual.

Doxygen is required for creating the documentation in HTML and as latex sources.
An actual LaTeX installation (along with ``pdflatex``) is required for creating
the manual.

The documentation will not be built automatically during build but is built
manually by calling target ``doc``.

	$ cd build
	$ cmake --build . --target doc

This will create the HTML documentation in ``build/html``. Open the file
``build/html/index.html`` in your browser to see the entry page.

Building the ``doc`` target will also create the LaTeX sources for the PDF
manual but will not automatically typeset the actual PDF document. Compile the
manual as follows (while still in directory ``build``):

	$ cd latex
	$ make

This will create the manual ``refman.pdf`` in folder ``build/latex`` (while
issueing loads of ``Underfull \hbox`` warnings, which is perfectly normal).


## No Windows port yet :-(

Libarcs has not yet been tried to be built on Windows.

To avoid any show-stoppers for porting libarcs to Windows or other platforms,
libarcs relies completely on pure C++14 and the C++ standard library. It
requires no other dependencies and does not use platform specific operations
within the source code without asserting the platform. (At least it is not
intended to do so, and the author will consider this as a bug). The porting is
expected not to be difficult, but is just not done.


[1]: https://include-what-you-use.org/
[2]: https://github.com/catchorg/Catch2


# How to Build															{#build}


# Building libarcs on Linux and \*BSD

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

- git - testing: to clone test framework [Catch2][2] as an external project when running the unit tests
- Doxygen - documentation: to build the API documentation in HTML or LaTeX
- LaTeX (TeXLive for instance) - documentation: to build the documentation in LaTeX
- include-what-you-use - development: to control ``include``-relationships when working on the code



## Buildtime switches

The desired buildtime configuration for libarcs may differ for users, developers
and packagers of libarcs, which is pointed out below.

Libarcs' CMake configuration knows the following switches:

|Switch         |Description                                     |Default|
|---------------|------------------------------------------------|-------|
|AS_STATIC      |Build static library instead of shared library  |OFF    |
|WITH_NATIVE    |Use platform specific optimization on compiling |OFF for CMAKE_BUILD_TYPE=Debug, ON for CMAKE_BUILD_TYPE=Release|
|WITH_TESTS     |Compile tests for runing ctest                  |OFF    |
|IWYU           |Use include-what-you-use on compiling           |OFF    |

If you are experienced in compiling libraries and know what you are doing, you
may skip the following part of this paragraph and proceed to the paragraph
``Building the documentation``.


## How to build

The build is quite standard. The steps for building are as follows:

-# Download and unpack libarcs to directory ``libarcs``
-# Change to directory ``libarcs``
-# Beneath directory ``libarcs``, create a directory named ``build`` and change
   to it
-# Configure the project by calling ``cmake ..`` with the desired options from
   the table above
-# Build the project by calling ``cmake --build .``
-# Install the provided files to your system by calling the ``install`` target

The first step may vary with the method you acquired a copy of libarcs. If you
have just cloned the repository to directory ``libarcs`` you already completed
the first step.

Second step (for Linux and \*nix):

	$ cd libarcs

Third step (for Linux and \*nix):

	$ mkdir build
	$ cd build

The last three steps may be different for you depending whether you act as a
user, a developer (maybe who intends to contribute to libarcs) or as a package
maintainer (who intends to package libarcs for some target system).

We therefore describe the steps separately for users, developers and package
maintainers.

### Users

You want to install libarcs on your system, say, for using ``arcs_tools`` or for
writing your own tools that use the libarcs API. You just want libarcs to be
available along with its headers, running fast and remaining small:

	$ cmake -DCMAKE_BUILD_TYPE=Release ..
	$ cmake --build .
	$ sudo make install

If your user has write permissions in the install directory, you should stick
to the cmake-only version instead:

	$ cmake -DCMAKE_BUILD_TYPE=Release ..
	$ cmake --build . --target install

This will install the following files to your system:

- the shared object libarcs.so.x.y.z (along with a symbolic link ``libarcs.so``) in the standard library location
- the six public header files \ref calculate.hpp, \ref checksum.hpp, \ref identifier.hpp, \ref logging.hpp, \ref match.hpp and \ref parse.hpp in the standard include location.
- the pkg-config configuration file libarcs.pc

On \*nix environments, as you guessed, the default install prefix is
``/usr/local`` but you can change this by calling cmake with the
``-DCMAKE_INSTALL_PREFIX=/path/to/install/dir`` switch.

### Developers

You want to debug into the libarcs code, which may be helpful when working on
the code. You need to build libarcs with debugging symbols and without
aggressive optimization:

	$ cmake -DCMAKE_BUILD_TYPE=Debug ..
	$ cmake --build .
	$ sudo make install

For also building the tests, just use the corresponding switch:

	$ cmake -DCMAKE_BUILD_TYPE=Debug -DWITH_TESTS=ON ..
	$ cmake --build .
	$ ctest
	$ sudo make install

Note that ``-DWITH_TESTS=ON`` will try to git-clone the testing framework Catch2
within your directory ``build`` and fail if this does not work.

Running the unit tests is not part of the build process. The tests have to be
executed manually after the build is completed, just by invoking ``ctest``.

NOTE that currently there are tests that read test data by hardcoded relative
paths. These tests will only pass if ``ctest`` is called from directory
``build``. This behaviour is considered a bug and will be fixed soon.

For controlling the ``include``-relationships within the code, CMake knows
about Google's tool [include-what-you-use][1]. If you have installed this tool,
you can instruct CMake to create a buildfile for calling include-what-you-use
(in this example, we use ``make``):

	$ cmake -DCMAKE_BUILD_TYPE=Debug -DIWYU=ON ..
	$ cmake --build . 2>iwuy.txt

This runs every source file through inlcude-what-you-use and writes the
resulting analysis to file ``iwyu.txt``.


### Package maintainers

You want to build libarcs with a release profile but without having the compiler
performing any architecture specific optimization (e.g. without -march=native
for gcc). Furthermore, you would like to adjust the install prefix path such
that libarcs is configured to expect being installed in the real system prefix
(such as ``/usr``) instead of some default prefix (such as ``/usr/local``). (So
when executing the install target after build, you choose the staging directory
as the intermediate target.)

When using clang or gcc, this can be achieved as follows:

	$ cmake -DCMAKE_BUILD_TYPE=Release -DWITH_NATIVE=OFF -DCMAKE_INSTALL_PREFIX=/usr ..
	$ cmake --build .
	$ make DESTDIR=/my/staging/dir install

*Note* that ``WITH_NATIVE=OFF`` currently only works for clang and gcc.

If you use another compiler than clang or gcc, CMake will not apply any project
specific modifications to the compiler default settings. Therefore, you have to
carefully inspect the build process (e.g. by using ``$ make VERBOSE=1``) to
verify which compiler settings are actually used.


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
``build/html/index.html`` in your browser to see *this* page.

Building the ``doc`` target will also create the LaTeX sources for the PDF
manual. Creating the actual PDF file from those sources requires manual
compilation. Compile the manual as follows (while still in directory ``build``):

	$ cd latex
	$ make

The manual [refman.pdf](../latex/refman.pdf) will be created in
``build/latex`` (while issueing loads of ``Underfull \hbox`` warnings, which is
quite normal).


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


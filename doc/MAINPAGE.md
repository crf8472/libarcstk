\mainpage libarcs - A library to calculate and verify AccurateRip checksums and ids


\section features Features

The libarcs function library provides offline service functions for working with
AccurateRip checksums ("ARCSs"). Although AccurateRip checksums are often
referred to as "CRCs", in a mathematical sense, they are not CRCs. Therefore, we
call them "ARCS"s (not "CRC"s).

Libarcs is written in C++14. It exposes a conservative API that is intended to
be open for use by pre-C++11 applications. (Untested at the moment.)

Libarcs supports the following tasks for working with ARCSs:

- Compute the AccurateRip checksums of a succession of uint32_t represented
  sample blocks (from a CD TOC and the actual audio samples)
- Verify ARCSs against reference sums from AccurateRip
- Compute the AccurateRip ID of a CD from its TOC information
- Parse the response of an request to the AccurateRip database



\section nonfeatures What libarcs does not

Libarcs has no network facilities and is not supposed to get any. The actual
HTTP request for fetching the reference values is better performed by the
HTTP networking client of your choice.



\section restrictions Current Limitations

- Not considered stable before 1.0.0, the API may change any time in any way
- Supports only little endian plattforms. Release-build on big endian plattforms
  is therefore ruled out by cmake. Will just assume a little endian plattform
  and fail silently on big endian plattforms without reporting.



\section bugs Bugs

- Data tracks are just processed like audio tracks. What happens is completely
  untested.



\section howtobuild How to Build

Build and install to just use the API:

	$ cd libarcs          # your libarcs root directory where README.md resides
	$ mkdir build && cd build
	$ cmake -DCMAKE_BUILD_TYPE=Release ..
	$ cmake --build .
	$ sudo make install   # installs to /usr/local

A detailed HowTo explaining different build scenarios and all build switches see
BUILD.md.



\section howtouse How to Use

For the API reference see API.md. An introductory tutorial is to follow.


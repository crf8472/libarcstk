# A library to calculate and verify AccurateRip checksums and ids



## What libarcs is

- Function library for AccurateRip checksums ("ARCS"s)
- Calculates checksums and AccurateRip ids
- Verifies checksums against reference sums from AccurateRip
- Written in C++14
- Conservative API, intended to be open for use by pre-C++11 applications
  (yet untested).
- Seems pretty fast



## Features

Libarcs supports the following tasks for working with ARCSs:

- Compute the AccurateRip checksums of a succession of uint32_t represented
  sample blocks (from a CD TOC and the actual audio samples)
- Verify ARCSs against reference sums from AccurateRip
- Compute the AccurateRip ID of a CD from its TOC information
- Parse the response from AccurateRip database to plain text

Although AccurateRip checksums are often referred to as "CRCs", in a
mathematical sense, they are not CRCs. Therefore, we call them "AccurateRip
Checksums" or "ARCS"s for short.



## What libarcs does not

Libarcs offers no network facilities and is not supposed to do so. The actual
HTTP request for fetching the reference values is better performed by the
HTTP networking client of your choice.



## Current Limitations

- No production release yet - will be 1.0.0
- API is not considered stable before 1.0.0 - For now, API may change any time
  in any way
- Supports only little endian plattforms. Release-build on big endian plattforms
  is therefore ruled out by cmake.



## Bugs

- Never tested calculation any images containing data tracks.



## How to Build

Build and install to just use the API:

	$ cd libarcs         # your libarcs root directory where README.md resides
	$ mkdir build && cd build
	$ cmake -DCMAKE_BUILD_TYPE=Release ..
	$ cmake --build .
	$ sudo make install  # installs to /usr/local

See a [detailed HowTo](BUILD.md) explaining different build scenarios and all
build switches.



## How to Use

An introductory tutorial will be found in the Wiki. You can find the API
documentation here.


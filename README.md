# A library to calculate and verify AccurateRip checksums and ids



## What libarcs is

- Function library for AccurateRip checksums ("ARCS"s)
- Calculates checksums and ids on CD images
- Verifies checksums against reference sums from AccurateRip
- Written in C++14
- Conservative API, also for pre-C++11 (untested) applications
- Seems pretty fast



## Features

Libarcs supports the following tasks:

- Compute the ARCSs of a succession of uint32_t sample blocks
  (from a CD TOC and the actual audio samples)
- Verify local ARCSs against the checksums provided by AccurateRip
- Calculate the AccurateRip ID of a CD image from TOC information
- Parse the response from AccurateRip database to plain text

Although AccurateRip checksums are often referred to as "CRCs", they are not
CRCs in a mathematical sense. Therefore, we call them just "AccurateRip
Checksums" or "ARCS"s for short.



## What libarcs does not

- Libarcs does not offer to read, buffer or decode any audio data. You have to
  provide the samples on your own. (Solution is underway.)
- Libarcs offers no network facilities and is not supposed to do so. The actual
  HTTP request for fetching the reference values is better performed by the HTTP
  networking client of your choice.



## Current Limitations

- No production release yet - will be 1.0.0
- API is not considered stable before 1.0.0 - For now, API may change any time
  in any way
- Supports only little endian plattforms. Release-build on big endian plattforms
  is therefore ruled out by cmake.



## Bugs

- Never tested with CD images containing data tracks.



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

A quickstart tutorial will be found (soon) in the Wiki. You can [build the API
documentation](BUILD.md#building-the-api-documentation) and view it in a
browser.


# Calculate and Verify AccurateRip Checksums and Ids

[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)
[![C++17](https://img.shields.io/badge/C++-17-darkblue.svg)](./DESIGN.md)
[![Release](https://img.shields.io/github/v/release/crf8472/libarcstk?display_name=tag&include_prereleases)](https://github.com/crf8472/libarcstk/releases)
[![Build & Test](https://github.com/crf8472/libarcstk/actions/workflows/build-and-test.yml/badge.svg)](https://github.com/crf8472/libarcstk/actions/workflows/build-and-test.yml)
[![Sanitizers](https://github.com/crf8472/libarcstk/actions/workflows/sanitizers.yml/badge.svg)](https://github.com/crf8472/libarcstk/actions/workflows/sanitizers.yml)
[![clang-tidy](https://github.com/crf8472/libarcstk/actions/workflows/clang-tidy.yml/badge.svg)](https://github.com/crf8472/libarcstk/actions/workflows/clang-tidy.yml)


## What libarcstk does

Libarcstk supports the following tasks:

- Calculate the AccurateRip checksums of a sequence of decoded samples.
- Verify local AccurateRip checksums against their reference values provided by
  AccurateRip.
- Calculate the AccurateRip ID of a CD image from TOC information.
- Parse the response from AccurateRip database to plain text.

Libarcstk provides:

- An API for working with AccurateRip checksums and ids
- Tested to build on Linux, MacOS and Windows
- Reasonably fast

Although AccurateRip checksums are often referred to as "CRCs", they are not
CRCs in a mathematical sense. Therefore, we call them just "AccurateRip
Checksums" or "ARCS"s for short. And this is libarcstk, a toolkit library for
mostly ARCSs.



## What libarcstk does not

- Libarcstk does not rip CDs
- Libarcstk does not offer to read or decode any audio data. You have to
  provide the samples on your own. (Note that there is [libarcsdec][2] that
  can possibly do that for you. If you need the functions of libarcstk in an
  executable for the command line check whether [arcs-tools][3] fits your
  needs.)
- Libarcstk offers no network facilities and is not supposed to do so. The
  actual HTTP request for fetching the reference values from AccurateRip is
  better performed by the HTTP client of your choice.



## How to Build

Build and install to just use the libarcstk API:

	$ cd libarcstk       # your libarcstk root directory where README.md resides
	$ mkdir build && cd build
	$ cmake ..           # configure default 'Release' build
	$ cmake --build .
	$ sudo make install  # installs to /usr/local

See a [detailed HowTo](BUILD.md) explaining different build scenarios and all
build switches.



## How to Use

- Provide the ToC data, pass the samples in a sequence of portions and get the
  checksums or the id. Verify the checksums by matching them against reference
  checksums you provide.
- Each supported usecase is illustrated by a corresponding [example
  application](./examples/) in the ``examples/`` folder. Building the examples
  is requested by adding ``-DWITH_EXAMPLES=ON`` to the cmake configure step.
  Note that this requires the external dependencies libsndfile and libcue which
  are not required in a default build!
- [Build the API
  documentation](BUILD.md#user-content-building-the-api-documentation) and view
  it in a browser or [read it online][1].
- For local ARCS calculation, also check whether [libarcsdec][2] is useful. Its
  target is to read virtually any lossless audio format by a uniform API that is
  really simple to use. The functionality of both, libarcsdec and libarcstk is
  made available for the command line by the [arcs-tools][3].



## Current Limitations

- No production release yet - will be 1.0.0.
- API is not considered stable before 1.0.0 (may change any time in any way
  while major release number is 0).
- Supports only little endian plattforms. Release-build on big endian plattforms
  is therefore ruled out by cmake for the moment.
- Builds on Linux, MacOS and Windows. However, it is not well tested on MacOS
  and Windows and the support of those platforms should be considered
  experimental (at best) in current releases.



## Bugs

- Checksumming CD images containing data tracks is untested and broken.

[1]: https://crf8472.github.io/libarcstk/current/
[2]: https://github.com/crf8472/libarcsdec/
[3]: https://github.com/crf8472/arcs-tools


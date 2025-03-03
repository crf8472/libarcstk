# Toolkit to calculate and verify AccurateRip checksums and ids

[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)
[![C++14](https://img.shields.io/badge/C++-14-darkred.svg)](./API.md)
[![Release](https://img.shields.io/github/v/release/crf8472/libarcstk?display_name=tag&include_prereleases)](https://github.com/crf8472/libarcstk/releases)

**This branch contains version 0.2 which is not developed any more**


## Version 0.2

This version branch is outdated and will not see any development any more.
The last release with this API was 0.2.1-alpha.2. You will find the API
documentation for this version [here][1].



## What libarcstk does

Libarcstk supports the following tasks:

- Compute the AccurateRip checksums of a sequence of decoded samples
- Verify local AccurateRip checksums against their reference values provided by
  AccurateRip
- Calculate the AccurateRip ID of a CD image from TOC information
- Parse the response from AccurateRip database to plain text

Libarcstk provides:

- An API for working with AccurateRip checksums and ids
- Builds on Linux (and presumably/untested on *BSD)
- Reasonably fast

Although AccurateRip checksums are often referred to as "CRCs", they are not
CRCs in a mathematical sense. Therefore, we call them just "AccurateRip
Checksums" or "ARCS"s for short.



## What libarcstk does not

- Libarcstk does not rip CDs
- Libarcstk does not offer to read or decode any audio data. You have to
  provide the samples on your own. (Note that there is [libarcsdec][2] that
  can possibly do that for you.)
- Libarcstk offers no network facilities and is not supposed to do so. The
  actual HTTP request for fetching the reference values from AccurateRip is
  better performed by the HTTP client of your choice.



## How to Build

Build and install to just use the libarcstk API:

	$ cd libarcstk       # your libarcstk root directory where README.md resides
	$ mkdir build && cd build
	$ cmake ..           # defaults to 'Release' build
	$ cmake --build .
	$ sudo make install  # installs to /usr/local

See a [detailed HowTo](BUILD.md) explaining different build scenarios and all
build switches.



## How to Use

- Each supported usecase is illustrated by a corresponding [example
  application](./examples/) in the ``examples/`` folder.
- [Build the API
  documentation](BUILD.md#user-content-building-the-api-documentation) and view
  it in a browser or [read it online][1].
- For local ARCS calculation, also check whether [libarcsdec][2] is useful. Its
  target is to read virtually any lossless audio format by a uniform API that is
  really simple to use.



## Current Limitations

- No production release yet - will be 1.0.0.
- API is not considered stable before 1.0.0 (may change any time in any way
  while major release number is 0).
- Supports only little endian plattforms. Release-build on big endian plattforms
  is therefore ruled out by cmake.
- Never built, installed or tested on Windows or Mac OS X, but it is intended to
  support these platforms in the future.



## Bugs

- Checksumming CD images containing data tracks is untested and broken.

[1]: https://crf8472.github.io/libarcstk/0.2.1/
[2]: https://github.com/crf8472/libarcsdec/

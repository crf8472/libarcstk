# A library to calculate and verify AccurateRip checksums and ids

[![MIT licensed](https://img.shields.io/badge/license-MIT-blue.svg)](./LICENSE)
[![C++14](https://img.shields.io/badge/C++-14-darkred.svg)](./API.md)
[![Latest](https://img.shields.io/badge/dynamic/json.svg?url=https://codeberg.org/api/v1/repos/tristero/libarcstk/releases&label=latest&query=$.0.name&color=yellowgreen)](https://codeberg.org/tristero/libarcstk/releases)


## What libarcstk is

Libarcstk supports the following tasks:

- Compute the AccurateRip checksums of a sequence of decoded samples
- Verify local AccurateRip checksums against their reference values provided by
  AccurateRip
- Calculate the AccurateRip ID of a CD image from TOC information
- Parse the response from AccurateRip database to plain text

Libarcstk provides:

- An API for working with AccurateRip checksums and ids
- Conservative OO API (intended to be usable from other languages)
- Builds on Linux (and presumably/untested on *BSD)
- Reasonably fast

Although AccurateRip checksums are often referred to as "CRCs", they are not
CRCs in a mathematical sense. Therefore, we call them just "AccurateRip
Checksums" or "ARCS"s for short.



## What libarcstk does not

- Libarcstk does not rip CDs
- Libarcstk does not offer to read, buffer or decode any audio data. You have to
  provide the samples on your own.
- Libarcstk offers no network facilities and is not supposed to do so. The
  actual HTTP request for fetching the reference values from AccurateRip is
  better performed by the HTTP networking client of your choice.



## How to Build

Build and install to just use the libarcstk API:

	$ cd libarcstk       # your libarcstk root directory where README.md resides
	$ mkdir build && cd build
	$ cmake -DCMAKE_BUILD_TYPE=Release ..
	$ cmake --build .
	$ sudo make install  # installs to /usr/local

See a [detailed HowTo](BUILD.md) explaining different build scenarios and all
build switches.



## How to Use

- Consult the [example applications](./examples/README.md) in the examples
  folder. They illustrate the supported usecases.
- [Build the API documentation](BUILD.md#building-the-api-documentation) and
  view it in a browser.
- For local ARCS calculation, also check whether
  [libarcsdec](https://codeberg.org/tristero/libarcsdec) is helpful. Its
  target is to read virtually any lossless audio format by a uniform
  API that is really simple to use.
- As soon as I manage to write a quickstart tutorial, it will be found in the
  Wiki.



## Current Limitations

- No production release yet - will be 1.0.0
- API is not considered stable before 1.0.0 (may change any time in any way
  until then)
- Supports only little endian plattforms. Release-build on big endian plattforms
  is therefore ruled out by cmake.
- Never built, installed or tested on Windows or Mac OS X, but it is intended to
  support these platforms in the future



## Bugs

- Never tested with CD images containing data tracks.


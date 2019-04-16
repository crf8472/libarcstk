# API

The public libarcstk API is represented by the following seven public headers.

Note that every symbol of libarcstk resides in namespace ``arcs`` or one of its
inner namespaces.

Read [how to build the comprehensive reference documentation of the API]
(./BUILD.md#building-the-api-documentation).

## [identifier.hpp](./src/identifier.hpp)

Parse TOC information and calculate the AccurateRip id of your medium. This API
computes the AccurateRip ID of your CD when you provide the offsets and lengths
for all tracks to it. Depending on which TOC data you have it may be required to
also count the actual samples to know the leadout.

## [calculate.hpp](./src/calculate.hpp)

Throw the samples to a Calculation instance and get the AccurateRip checksums as
a result. Combine this with the sample reader implementation of your choice
(ffmpeg, libsndfile, any decoder lib, you name it).

### [checksum.hpp](./src/checksum.hpp)

Represent track-related Checksums as provided by the ``calculate`` API.

### [samples.hpp](./src/samples.hpp)

Representing normalized samples as a SampleSequence. This is the input to the
``calculate`` API.

This header offers only templates and has no compiled parts.

## [match.hpp](./src/match.hpp)

Match a calculation result against an AccurateRip response. This functionality
is used to actually verify the calculated checksums.

## [parse.hpp](./src/parse.hpp)

Parsing the binary payload of AccurateRip responses to plaintext. It is quite
easy to throw together a parser that just returns the parsed content as
structured data you can print (in a style you have to define and implement). Of
course you can write your own parser handlers that act on the input bytes as you
define.

## [logging.hpp](./src/logging.hpp)

Provides access to the libarcstk logging facility, i.e. provides the
``ARCS_LOG\*`` macros. You will not need this unless you intend to write a tool
that uses the logging part of libarcstk. However, this might be of some interest
if you intend to write some wrapper library or a tool.


# API

# Public API

The public libarcs API is represented by the following publich headers:

- \ref calculate.hpp - Throw the samples in a calculating class and get the
  AccurateRip checksums as a result. Combine this with the sample reader
  implementation of your choice (ffmpeg, libsndfile, any decoder lib, you name
  it).
- \ref checksum.hpp - An API for representing Checksums and calculation results
- \ref identifier.hpp - Parse TOC information and calculate AccurateRip id of
  your medium. This API computes the AccurateRip ID of your CD when you
  provide the offsets and lengths for all tracks to it. Depending on which TOC
  data you have it may be required to also count the actual samples to know the
  leadout.
- \ref match.hpp - Match a calculation result against and AccurateRip response.
- \ref parse.hpp - Parsing the binary payload of AccurateRip responses to
  plaintext. It is quite easy to throw together a parser that just returns the
  parsed content as structured data you can print (in a style you have to define
  and implement). Of course you can write your own parser handlers that act on
  the input bytes as you define.
- \ref logging.hpp - Provides access to the libarcs logging facility, i.e.
  provides the ARCS_LOG\* macros. You will not need this unless you intend to
  write a tool that uses the logging part of libarcs. However, this might be of
  some interest if you intend to write some wrapper.

Note that every symbol of libarcs resides in namespace ``arcs`` or one of its
inner namespaces.


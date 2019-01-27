# API																	{#api}

# Public API

The public libarcs API is split in four different parts:

- \ref identifier.hpp - If you intend to construct the AccurateRip ID from the
  TOC data you already have, this API computes the AccurateRip ID of your CD
  when you provide the offsets and lengths for all tracks to it. Depending on
  which TOC data you have it may be required to also count the actual samples to
  know the length of the last track.
- \ref calculate.hpp - If you intend to write your own audio sample readers,
  there is also a low-level interface in \ref calculate.hpp that gives you an
  API for throwing samples in a calculating class and get the checksums as a
  result. Combine this with your own reader implementation.
- \ref checksum.hpp - An API for representing Checksums and calculation results
- \ref match.hpp - An API for matching a calculation result against and
  AccurateRip response
- \ref parse.hpp - An API for parsing AccurateRip responses. It is quite easy
  to throw together a parser that just returns the parsed content as structured
  data that you can print (in a style you have to define and implement). Of
  course you can write your own parser handlers that act on the input bytes as
  you define.
- \ref logging.hpp - Provides access to the libarcs logging facility, i.e.
  provides the ARCS_LOG\* macros. You will not need this unless you intend to
  write a tool that uses the logging part of libarcs.

Note that every symbol of libarcs resides in namespace ``arcs``.

For a list of all modules, see [here](modules.html)


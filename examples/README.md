# Example applications

libarcstk comes with 4 optional executables. Those are mini-examples for what
can be done with libarcstk and how this is to be done. Each of the examples
refers to one of the main modules of libarcstk (calculate, id, dbar, verify).

To build the examples set the switch ``-DWITH_EXAMPLES=ON`` when configuring the
project. In this case, find the compiled binaries after build in your directory
``build/examples/`` where a build directory for each example is located.

The example applications are linked against the compiled libarcstk binary in the
build tree! They are not suited to work in a regular system install. Do not
install them to another platform and do not rely on them in your production!

The motivation of the examples is demonstration, not productive use. It is
necessary to carefully backup any file before using it as input for the example
applications!

Note: The example applications do exclusively target situations where an entire
album as a single audio file along with its metadata (as a Cuesheet) is to be
processed. This restriction keeps the examples reasonably small. Of course,
libarcstk can also process single audio files representing specific tracks or a
set of audio files that represent an album or parts of an album. For this use
cases, there is currently no example provided but given the examples and the API
documentation it should not be that hard to figure out how to do it.

- [albumverify](./albumverify/README.md) - Demonstrates how locally known
  checksums can be verified against the checksums AccurateRip provides.
- [albumcalc](./albumcalc/README.md) - Demonstrates how AccurateRip checksums
  can be calculated locally on a ripped audio image and a Cuesheet. The actual
  decoding is performed by libsndfile, hence checksums can be calculated on any
  combination of container format and lossless audio codec that libsndfile
  supports.
- [albumid](./albumid/README.md) - Demonstrates how to derive the
  AccurateRip-ID of a ripped album. With this id, libarcstk also provides the
  Query-Request-URL specific to that album as well as the canonical filename for
  the response file. This part of the API can be used to send a query request to
  AccurateRip and receive the checksums for the actual album ripped locally.
- [dbarparse](./dbarparse/README.md) - Demonstrates how the response that
  AccurateRip sends can be parsed to get the actual checksums in a plaintext
  format.

## Disclaimer

Note that despite the example applications may provide some useful functions,
they are rudimentary implemented and in no way suitable for production use. They
are mere local demonstrations. Do not use them as tools in your production
workflows!


# Example applications

libarcs comes with 4 mini examples for what can be done with libarcs and how
this is to be done. Each of the examples refers to one of the main modules of
libarcs.

Each of the example application lets the user input its own data and is intended
to provide useful results for a specific task.

Of course the examples can be plugged together on a shell command line to
demonstrate how they complement each other.

- [albumid](./albumid/README.md) - Demonstrates how the ``identifier`` header
  can be used to derive the AccurateRip-ID of a ripped album. With this id,
  libarcs also provides the specific AccurateRip-URL that can be used to send a
  query request to AccurateRip and receive the checksums for this id.
- [arparse](./arparse/README.md) - Demonstrates how the response that
  AccurateRip sends can be parsed to get the actual checksums in a plaintext
  format.
- [albumcalc](./albumcalc/README.md) - Demonstrates how AccurateRip checksums
  can be locally calculated on a ripped audio image and a CUEsheet. The actual
  decoding is performed with libsndfile, so checksums can be calculated on any
  combination of container format and lossless audio codec that libsndfile can
  handle.
- [albumverify](./albumverify/README.md) - Demonstrates how locally known
  checksums can be verified against the checksums AccurateRip provides.

The example applications do only target situations where an entire album (in a
single audio file) along with its metadata (as a CUESheet) is to be processed.
Of course, libarcs can also process single audio files representing tracks or a
set of audio files that represent an album.

The aim of the examples is demonstration. If you are interested in using
libarcs, the author feels you should have some code to start with. This may make
the current lack of a tutorial less hurting.

Disclaimer: Note that despite the example applications may provide some useful
functions, they are _not_ hardened for production use. They should be considered
as local demonstrations, _not_ as tools!

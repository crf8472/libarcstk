# Example application: arparse

This example application demonstrates how libarcstk is used to parse a binary
response to an AccurateRip database request into plaintext. The response is
provided as a file or as piped command line input.


## Requirements

- libarcstk


## Build

The Makefile uses g++ to compile the sources.

Build application with just

	$ make

For removing all compiled and temporary files, just use

	$ make clean


## Usage

### How to get the input file

The ripper sends a request to AccurateRip and sometimes saves the response to a
file. EAC for example will produce a file with a name starting with "dBAR" and
a ".bin" suffix, like "dBAR-015-001b9178-014be24e-b40d2d0f.bin". EAC puts those
files in ``${USER}/Application Data/AccurateRip/AccurateRipCache``. Any of them
can be used as input to this example.

If you cannot get a response file from your ripper, you may produce a file on
your own by actually send a query to AccurateRip. You can use ``curl`` and the
example application [albumid](../albumid/README.md), for example try:

	$ curl -s "$(../albumid/albumid /path/to/album/album.{cue,flac} | tail -1 | awk '{print $2}'))" > response_file.bin


### How to use the example application

arparse expects the filename as parameter, just like:

	$ ./arparse response_file.bin

For more information, read the comments in [arparse.cpp](./arparse.cpp).

Note: the 'parse' application of arcs-tools contains
[a parser subclass that can parse from stdin](https://codeberg.org/tristero/arcs-tools/src/branch/master/src/tools-parse.hpp).



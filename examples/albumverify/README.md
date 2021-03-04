# Example application: albumverify

This example application demonstrates how libarcstk is used to match the
checksums you have from your rip with the AccurateRip checksums. This example
can only verify entire albums.


## Requirements

Dependencies for building and running:

- libarcstk

Install a http-client like ``curl`` if you want to send actual queries to
AccurateRip.


## Build

The Makefile uses g++ to compile the sources.

Build application with just

	$ make

For removing all compiled and temporary files, just use

	$ make clean


## Usage

Use it like:

	$ ./albumverify --id=<ARId of the album> --arcs2=0x1,0x2,0x3,... <name_of_response_file>

albumverify expects two mandatory parameters:

- ``--id=...`` The AccurateRip id of the album in the form
  AAA-BBBBBBBB-CCCCCCCC-DDDDDDDD
- ``--arcs1=...`` or ``--arcs2=...`` A comma-separated list of hexadecimal
  numbers, representing the actual local checksums that are to be verified.
  You have to declare ARCSv1 values by using ``--arcs1`` and ARCSv2 values by
  using ``--arcs2``. The values may or may not be prefixed by "0x".
- The ``<name_of_the_response_file>`` is interpreted as a file holding the
  binary response of an AccurateRip request. It is optional since you can also
  pipe the binary input directly to ``albumverify``. If you are not sure how to
  provide this input, you may consult the
  [arparse example](../arparse/README.md).


## Example data

For convenience, ARCSv2 values of a real album along with its ARId are provided
as test input. The correct values reside in ``example_sums_ok.txt`` while a
version with deviating values for tracks 6, 12 and 15 resides in
``example_sums_fail.txt``. The ARId along with the specific Query URL resides in
``example_arid.txt``.

On Linux these input data can be verified against the values from AccurateRip as
follows:

	$ curl -s "$(tail -1 < example_arid.txt)" > response.txt
	$./albumverify --id=$(head -1 < example_arid.txt) --arcs2="$(xargs < example_sums_ok.txt | tr ' ' ',')" response.txt

To see the output for a partial failing verify, replace ``example_sums_ok.txt``
by ``example_sums_fail.txt`` in the example above.

For more information, read the comments in [albumverify.cpp](./albumverify.cpp).


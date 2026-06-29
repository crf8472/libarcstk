# Example application: albumid

This example application demonstrates how libarcstk is used to calculate the
AccurateRip request id of a already ripped album.


## Requirements

- libcue >= 2.0.0 (For parsing the Cuesheet)
- libsndfile >= 1.0.17 (For decoding the audio data)
- The compiled libarcstk binary object in the ``build/`` folder (e.g.
  ``libarcstk.so`` under Linux)


## Build

Do a regular build after setting switch ``-DWITH_EXAMPLES=ON``. Find executable
``albumid`` in ``build/examples/albumid``.


## Libcue bug with EOF

Note that libcue 2.2.1 and before has a bug that shows a syntax error for
some Cuesheets that end with whitespace. Use libcue 2.3+ to avoid this.


## Usage

albumid expects two filenames as parameters, the first being a Cuesheet and
the second an audio file in a lossless audio format that can be read by
libsndfile, e.g. RIFFWAV/PCM or FLAC/FLAC.

If the CUE sheet contains any information about files, this information will be
ignored.

	$ ./albumid <name_of_cuesheet.cue> <name_of_audio_file>

For more information, read the comments in [albumid.cpp](./albumid.cpp).


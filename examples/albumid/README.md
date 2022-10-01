# Example application: albumid

This example application demonstrates how libarcstk is used to calculate the
AccurateRip request id of a already ripped album.


## Requirements

- libcue >= 2.0.0 (For parsing the Cuesheet)
- libsndfile >= 1.0.17 (For decoding the audio data)
- The compiled libarcstk shared object ``libarcstk.so`` in the ``build/`` folder


## Build

The Makefile uses g++ to compile the sources.

Build application with just

	$ make

For removing all compiled and temporary files, just use

	$ make clean


## Usage

albumid expects two filenames as parameters, the first being a CUEsheet and
the second an audio file in a lossless audio format that can be read by
libsndfile, e.g. RIFFWAV/PCM or FLAC/FLAC.

If the CUE sheet contains any information about files, this information will be
ignored.

	$ ./albumid <name_of_cuesheet.cue> <name_of_audio_file>

For more information, read the comments in [albumid.cpp](./albumid.cpp).


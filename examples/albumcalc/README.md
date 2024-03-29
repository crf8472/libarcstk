# Example application: albumcalc

This example application demonstrates how libarcstk is used to calculate
AccurateRip checksums for each track of an album. The album is provided as a
single audio file accompanied by a Cuesheet. The parsing of the Cuesheet is
done using libcue, the decoding of the audio samples is done using libsndfile.


## Requirements

Dependencies for building and running:

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

albumcalc expects two filenames as parameters, the first being a CUEsheet and
the second an audio file in a lossless audio format that can be read by
libsndfile, e.g. RIFFWAV/PCM or FLAC/FLAC.

	$ ./albumcalc <name_of_cuesheet.cue> <name_of_audio_file>

For more information, read the comments in [albumcalc.cpp](./albumcalc.cpp).


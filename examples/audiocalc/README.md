# Example application: audiocalc

This example application demonstrates how libarcs is used to calculate
AccurateRip checksums for each track of an album. The album is provided as a
single audio file accompanied by a CUESheet.

## Requirements

Dependencies for building and running:

- libcue >= 2.0.0 (For parsing the CUESheet)
- libsndfile >= 1.0.17 (For decoding the audio data)

## Build

Build application with

	$ make

and use

	$ make clean

to remove the binaries.

## Usage

audiocalc expects two filenames as parameters, the first being a CUE sheet and
the second an audio file in a lossless audio format that can be read by
libsndfile, e.g. WAV/PCM or FLAC.

	$ ./audiocalc <name_of_chuesheet.cue> <name_of_audio_file>

For more information, read the comments in audiofile.cpp.

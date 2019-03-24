# Example application: albumid

This example application demonstrates how libarcs is used to calculate the
AccurateRip request id of a already ripped album.

## Requirements

- libarcs
- libcue >= 2.0.0 (For parsing the CUESheet)
- libsndfile >= 1.0.17 (For decoding the audio data)


## Build

Build application with just

	$ make

For removing all compiled and temporary files, just use

	$ make clean


## Usage


albumid expects two filenames as parameters, the first being a CUE sheet and
the second an audio file in a lossless audio format that can be read by
libsndfile, e.g. RIFFWAV/PCM or FLAC/FLAC.

If the CUE sheet contains any information about files, this information will be
ignored.

	$ ./albumid <name_of_chuesheet.cue> <name_of_audio_file>

For more information, read the comments in [albumid.cpp](./albumid.cpp).


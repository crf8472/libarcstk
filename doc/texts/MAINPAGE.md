\mainpage libarcstk - A library to calculate and verify AccurateRip checksums and ids


\section Home

See the project home [on codeberg.org](https://codeberg.org/tristero/libarcstk).


\section features Features

Libarcstk supports the following tasks for working with AccurateRip checksums:

- Calculate the AccurateRip checksums of a succession of audio samples, may it
  be a single track or an album
- Verify AccurateRip checksums against their reference sums from AccurateRip
- Calculate the AccurateRip ID of a CD from its TOC information (along with the
  request URL and the canonical response filename)
- Parse the response of an request to the AccurateRip database to plaintext



\section nonfeatures Non-features

- Libarcstk does not rip CDs
- Libarcstk does not read, decode or buffer audio data
- Libarcstk will not send any requests to AccurateRip. Libarcstk has no network
  facilities and is not supposed to get any. The actual HTTP request for
  fetching the reference values is better performed by the HTTP client of your
  choice.



\section howtobuild How to Build

Build and install to just use the API:

	$ git clone https://codeberg.org/tristero/libarcstk
	$ cd libarcstk       # your libarcstk root directory where README.md resides
	$ mkdir build && cd build
	$ cmake -DCMAKE_BUILD_TYPE=Release ..
	$ cmake --build .
	$ sudo make install  # installs to /usr/local


\section Version

This documentation was generated from commit
[@PROJECT_GIT_DESCRIBE@]
(https://codeberg.org/tristero/libarcstk/commit/@PROJECT_GIT_COMMIT@).


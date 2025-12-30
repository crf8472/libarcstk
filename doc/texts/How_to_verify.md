# How to Verify AccurateRip Checksums                            {#howto_verify}


## Have a look at the code example

Locate the example ``albumverify`` in the top-level folder ``examples``. You may
find a compileable code example for verifying AccurateRip checksums on a
concrete album image.


### Verifying

The motivation is to check whether the audio input you want to verify is exactly
the audio data from the compact disc. Verifying checksums means to locally
recalculate the checksums and then match it against the reference checksums
provided by AccurateRip.

For verifying audio data in general you need
	- The audio input you wish to verify
	- Reference Checksums provided by AccurateRip for this audio input

### Prerequisites

Consult the module @ref verify for the part of the API you want to use.

Insert

@code{.cpp}
	#include <arcstk/dbar.hpp>
	#include <arcstk/identifier.hpp>
	#include <arcstk/calculate.hpp>
	#include <arcstk/verify.hpp>
@endcode

in the file where you intend to place your code.

#### Verifying an Album

The common use case is to verify an album. To get this done you need even a
little bit more:
	- The audio image you wish to verify (may it be one or more files)
	- If you have exactly one file, the track offsets for the image are required
	- The AccurateRip id of the album to verify
	- Reference Checksums provided by AccurateRip for this particular id, which
	  typically come as a parseable DBAR file

This may require that you accomplish the following tasks:
	- Reading the audio data possibly along with its metadata & calculating the
	  checksums of this input (as explained in @ref howto_calculate).
	- Calculate the AccurateRip id if you not already know it (as explained in
	  @ref howot_get_ids).
	- Use the id to acquire and parse the reference values (as explained in
	  @ref howto_calculate).
	- Instantiate and configure a Verifier to run it on the aforementioned input
	  data (as explained below, just proceed).

@code{.cpp}
	using arcstk::AlbumVerifier;

	// Those three are discussed in the other HowTo-parts:
	using arcstk::ARId;
	using arcstk::Checksums;
	using arcstk::DBAR;
@endcode

The basic approach is to construct a Verifier on your local data and then call
''perform()'' on it thereby passing the reference values to match against.
This is the easy part.

@code{.cpp}

	// Let's assume that you already have the following:

	ARId      my_id        { /* calculated from your local audio or known */ };
	Checksums my_checksums { /* calculated from your local audio */ };
	DBAR      dbar         { /* from AccurateRips http response to my_id */ };

	// You use the AlbumVerifier since you have a single file and metadata:

	AlbumVerifier verifier { my_checksums, my_id };

	// This instance is now configured to match your local data agains the
	// reference values from the DBAR instance:

	const auto result { verifier.perform(dbar) };
@endcode

Now, how to interpret the result?

The most coarse-grained way would be a call of result->all_tracks_verified()
that will immediately tell you whether your input data matches the reference
values completely. It is a boolean value that just says "it's accurate" or "it's
not".

The most fine-grained way is to query the result for any triple of <block,
track, type> by just calling result->track(i, j, flag). This will tell you,
whether track j in block i matches for algorithm flag. (It will also tell you,
whether your reference checksums are v1 or v2 if you did not already know it.)

If your verification returned "not accurate", you may be interested in some
mid-level kind of analysis, that can be done by looking at the best matching
block first. This may be the most interesting part of the reference data, since
it is the closest match you got.

A call of result->best_block() tells you the index of the reference checksum
block within the DBAR instance that has the most matches. If there are multiple
blocks that all match, best_block() will return the first.

@code{.cpp}
	const auto best = result->best_block();
@endcode

The resulting object is a std::tuple that can be inspected by 0 (index), 1
(algorithm) or 2 (number of not matching tracks).

Get the best block from the DBAR instance and print it along with your
calculated data. The code example shows you how to do that in a very basic way.

#### Verifying a set of files

If your album consists of several files instead of one, the verification process
maybe a little bit easier since you do not need the track offsets. Without the
offsets, there is also no need for the id.

@code{.cpp}
	TracksetVerifier verifier { my_checksums };
@endcode

The result can be used in the same way as for AlbumVerifier.

For the API consult the page for module @ref verify.


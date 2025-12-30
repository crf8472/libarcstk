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
	  @ref howto_get_ids).
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
	DBAR   their_checksums { /* from AccurateRip's http response for my_id */ };

	// Let's use the AlbumVerifier since we have a single audio image file
	// and its metadata:

	AlbumVerifier verifier { my_checksums, my_id };

	// This instance is now set up to match your local data. Perform the match
	// by passing the reference values from the DBAR instance:

	const auto result { verifier.perform(their_checksums) };
@endcode

Note that function perform() accepts not only a DBAR object but a subclass of
ChecksumSource. Abstract class ChecksumSource provides an interface to access
checksum containers for verification. For any checksum container X that is to be
wrapped for input to function perform(), create a subclass of
ChecksumSourceOf<X>. Have a look at the code of class DBARSource for an example
how to do it.

So, having performed the verification, how to interpret the result?

The most coarse-grained way would be to simply check if verification was a total
success and the audio data was verified to be accurate.

Function all_tracks_verified() will just tell you whether your input data
matches the reference values completely. It is a boolean value that just says
"it's accurate" or "it's not".

@code{.cpp}
	if (result->all_tracks_verified())
	{
		// Success! Take the relevant actions
		// ...
	} else
	{
		// Not a complete success at least.
		// ...
	}
@endcode

If your verification returned "not accurate", you may be interested in some
mid-level kind of analysis, that can be done by inspecting the best matching
block of the reference checksums.

This block may be the most interesting part of the reference data, since it is
the closest match you got. The best block is that block within the DBAR instance
that has the most matching tracks. If there are multiple blocks that all match
equally well, best_block() will return the last matching ARCSv2 block of them. A
best block can only be v1 if there is no at least equally good block that is v2.

@code{.cpp}
	const auto best { result->best_block() };
@endcode

A call of result->best_block() provides you with 3 values:
  * the index of the reference checksum block within the DBAR instance,
  * the type of algorithm of the checksums within this respective block, and
  * the total number of mismatches in this block.

Currently, there is no API to access those values. The resulting object is a
std::tuple. Use std::get with the index values 0 (int for index), 1 (bool
whether it is v2 or not) or 2 (int for total number of mismatching tracks in
this block).

Get the best block from the DBAR instance and print it along with your
calculated data. The code example ``albumverify`` shows you how to do that in a
very basic way.

The most fine-grained way of inspection is to query the result object for any
triple of <b, t, flag> of a block index, a track index, and an algorithm type
(ARCSv1 or ARCSv2). This is achieved by result->track(b, t, flag).

This will tell you, whether track t in block b matches for algorithm type flag.

@code{.cpp}

	auto b { 0 };
	auto t { 1 };
	auto is_v2 { true };

	if (result->track(b, t, is_v2)
	{
		// The v2 value for track j in block i matches!
	}
@endcode

#### Verifying a set of files

If your album consists of several files instead of one, the verification process
maybe a little bit easier since you do not need the track offsets. Without the
offsets, there is also no need for the id.

@code{.cpp}
	TracksetVerifier verifier { my_checksums };
@endcode

The result can be used in the same way as for AlbumVerifier.

For the API consult the page for module @ref verify.


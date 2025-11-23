# How to calculate AccurateRip checksums


## Have a look at the code example

Locate the example ``albumcalc`` in the top-level folder ``examples``. You may
find a compileable libsndfile-based code example for calculating the ARCSs for
each track of an album.


## Calculating Checksums for the Tracks of an Album

### Prerequisites

Consult the module @ref calc for the part of the API you want to use.

Insert

@code{.cpp}
	#include <arcstk/metadata.hpp>
	#include <arcstk/samples.hpp>
	#include <arcstk/algorithms.hpp>
	#include <arcstk/calculate.hpp>
@endcode

in the file where you intend to place your code.

To decode the audio input into PCM samples, you may probably need a thirdparty
API to read samples from input files. For this task consider libsndfile, ffmpeg
or an abstraction API like libarcsdec.

For formats and codecs that do not require complex decoding (like WAV/PCM), you
are good to use the file reading capabilities of the bare C++ API and a
SampleSequence.

Start with some convenience:

@code{.cpp}
	using arcstk::make_toc;
	using arcstk::make_calculation;
	using arcstk::AccurateRip;
@endcode

In this example, we use interleaved samples represented as 16 bit wide integers.
That means that for calculation, each sample has to be normalized to a 32 bit
integer of a certain format required by the AccurateRip algorithm. To achieve
this, choose the InterleavedSamples interface.

@code{.cpp}
	using arcstk::InterleavedSamples;
@endcode

In case you intend to provide planar samples instead of interleaved ones, use
PlanarSamples.


### Create the Calculation Instance

If you intend to calculate the ARCSs for all tracks of an album, the start and
end positions of those tracks must be made available to libarcstk.

We therefore need the index (represented as total number of LBA frames) of each
track's first sample. Those indices are called offsets. To get also the last
track's end correct we furthermore need the total number of input frames, which
is identical with the leadout of the original compact disc. The leadout is as
well represented as total number of LBA frames.

You will have to acquire these informations from your image or your disc on your
own. We presuppose you have this values and show how to proceed from there.

With the leadout and the complete set of offsets, a ToC instance can be created:

@code{.cpp}
	const auto toc   { make_toc(total_number_of_frames, offsets) };
@endcode

We choose to calculate two sets of AccurateRip checksums for the entire input,
the checksums by the legacy algorithm (v1) and by the improved algorithm (v2).

@code{.cpp}
	auto algorithm   { std::make_unique<AccurateRip::V1and2>() };
@endcode

With the toc and the algorithm provided, we prepare the calculation process by
choosing an algorithm and instantiating the concrete calculation object.

@code{.cpp}
	auto calculation { make_calculation(std::move(algorithm), *toc) };
@endcode

The Calculation object is now ready to receive its input.


### Provide the Audio Samples

We define
	- an input buffer of the sample format previously determined
	- a SampleSequence object that converts the samples in the input buffer to
	  PCM 32bit.

@code{.cpp}
	const auto buffer_len { 16777216 * 2 };                     // e.g. 64 MB for 32 bit samples
	auto buffer           { std::vector<int16_t>(buffer_len) }; // sample buffer
@endcode

We have to wrap the bytes read from the file in a normalizing SampleSequence
that can be provided to the Calculation object.

We remember that we have chosen an interleaved 16bit input, so let's define our
SampleSequence accordingly.

@code{.cpp}
	auto sequence         { InterleavedSamples<int16_t>() };    // normalizing wrapper
@endcode

Once the samples can be decoded from the input file, the calculation task can be
performed. We will fill the buffer and then use the SampleSequence to provide
the samples to the Calculation.


### The Read/Update Loop

The actual samples can be read in blocks of size <code>buffer_len</code> to the
input buffer. As soon as the <code>buffer</code> is filled with integers from
your input stream or file, pass its content to the Calculation. For this
purpose, wrap the input samples in the normalizing wrapper and update the
calculation object.

Since the last block may be smaller than the other blocks, check for this case
and handle it according to your needs.

@code{.cpp}
	auto ints_in_block { int32_t { 0 } }; // know when to end the loop

	while ((ints_in_block = read_or_decode_ints_from_file(buffer, buffer_len)))
	{
		if (buffer_len != ints_in_block) // Read less samples than expected?
		{
			// ... Handle case where last block that may be smaller.
			// Usually you will have to adjust the size declaration according
			// to the actual smaller size or you resize physically e.g.
			// buffer.resize(ints_in_block);
		}

		// Use normalizing layer on the input for updating
		sequence.wrap_int_buffer(&buffer[0], buffer.size());

		// Update calculation by current sequence
		calculation->update(std::cbegin(sequence), std::cend(sequence));
	}

	if (!calculation->complete())
	{
		// ... Do error handling
	}

	// Get result
	auto checksums { calculation->result() };
@endcode


### Accessing the Checksums

Now the resulting checksums are represented as a list of ChecksumSet instances,
one per track.

They can be printed as a table by doing something like:

@code{.cpp}
	std::cout << "Track  ARCSv1    ARCSv2" << '\n';

	auto trk_no { 1 };
	for (const auto& track_values : checksums)
	{
		std::cout << std::dec << " " << std::setw(2) << std::setfill(' ')
			<< trk_no << "   " << std::hex << std::uppercase
			<< std::setw(8) << std::setfill('0')
			<< track_values.get(arcstk::checksum::type::ARCS1).value()
			<< "  "
			<< std::setw(8) << std::setfill('0')
			<< track_values.get(arcstk::checksum::type::ARCS2).value()
			<< '\n';

		++trk_no;
	}
@endcode

For the API consult the page for module @ref calc.


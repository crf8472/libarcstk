# How to calculate AccurateRip checksums

First you need an API to read samples from input files. For formats and codecs
that do not require complex decoding (like WAV/PCM), this can be achieved by
the file reading capabilities of the C++ API together with wrapping the bytes
read in a normalizing SampleSequence. You can also use a thirdparty lib like
libsndfile, ffmpeg or an abstraction API like libarcsdec.

Once the samples can be decoded from the input file, the calculation task can be
performed. Start with some convenience:

<code>
using arcstk::make_toc;
using arcstk::make_calculation;
using arcstk::AccurateRip;
</code>

The concrete code depends on the type of input that is decoded from the file.

In this example, we use interleaved samples represented as 16 bit wide integers.
For calculation, each sample has to be normalized to a 32 bit integer of a
certain format required by the AccurateRip algorithm. To achieve this, choose
the InterleavedSamples interface.

<code>
using arcstk::InterleavedSamples;
</code>

If you intend to calculate the ARCSs for all tracks in an album, the start and
end samples of those tracks must be made available to libarcstk. We therefore
need the index (represented as total number of LBA frames) of each track's first
sample. Those indices are called offsets. To get the last track's end correct we
also need the total number of input samples, which is identical with the leadout
of the original compact disc. The leadout is also represented as total number of
LBA frames. With those two informations, a ToC instance can be created:

<code>
const auto toc   { make_toc(total_samples, offsets) };
</code>

With the toc and the sample format provided, we prepare the calculation process
by choosing an algorithm and instantiating the concrete calculation object.

<code>
auto algorithm   { std::make_unique<AccurateRip::V1and2>() };
auto calculation { make_calculation(std::move(algorithm), *toc) };
</code>

We define an input buffer of the sample format previously determined.

<code>
const auto buffer_len { 16777216 * 2 };             // 64 MB for 32 bit samples
auto buffer   { std::vector<int16_t>(buffer_len) }; // sample buffer
auto sequence { InterleavedSamples<int16_t>() };    // normalizing wrapper
</code>

The actual samples can be read in blocks of size buffer_len to the input buffer.
Since the last block may be smaller than the other blocks, check for this case.
Wrap the input samples in the normalizing wrapper and update the calculation
object.

<code>
auto ints_in_block { int32_t { 0 } }; // know when to end the loop

while ((ints_in_block = read_or_decode_from_file(buffer, buffer_len)))
{
	if (buffer_len != ints_in_block) // Read less samples than expected?
	{
		// ... Handle last block that may be smaller, e.g. buffer.resize(...)
	}

	// Normalize input for updating
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
</code>

Now the resulting checksums are represented as a list of ChecksumSet instances,
one per track. They can be printed as a table by doing something like:

<code>
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
</code>


For the API consult the page for module @ref calc.


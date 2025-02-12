//
// Example for calculating AccurateRip checksums from each track of an album,
// represented by a Cuesheet and a single losslessly encoded audio file.
//

#ifndef __LIBARCSTK_ALGORITHMS_HPP__ // libarcstk: AccurateRip algorithms
#include "algorithms.hpp"
#endif
#ifndef __LIBARCSTK_CALCULATE_HPP__  // libarcstk: calculate ARCSs
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_METADATA_HPP__   // libarcstk: Compact Disc metadata
#include "metadata.hpp"
#endif
#ifndef __LIBARCSTK_SAMPLES_HPP__    // libarcstk: normalize input samples
#include "samples.hpp"
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__    // libarcstk: log what you do
#include "logging.hpp"
#endif

extern "C" {
#include <libcue/libcue.h>        // libcue for parsing the CUEsheet
}
#include <sndfile.hh>             // libsndfile for reading the audio file

#include <cstdint>   // for uint32_t etc.
#include <cstdio>    // for fopen, fclose, FILE
#include <cstdlib>   // for EXIT_SUCCESS
#include <iomanip>   // for setw, setfill, hex
#include <iostream>  // for cerr, cout
#include <stdexcept> // for runtime_error
#include <string>    // for string


// ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !
// NOTE! THIS IS EXAMPLE CODE! IT IS INTENDED TO DEMONSTRATE HOW LIBARCSTK COULD
// BE USED. IT IS NOT INTENDED TO BE USED IN REAL LIFE PRODUCTION. IT IS IN NO
// WAY TESTED FOR PRODUCTION. TAKE THIS AS A STARTING POINT TO YOUR OWN
// SOLUTION, NOT AS A TOOL.
// ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !


/**
 * \brief Parse a CUEsheet and return offsets and implicitly the track count.
 *
 * This method is implemented without any use of libarcstk. It just has to be
 * available for parsing the Cuesheet.
 *
 * @param[in] cuefilename Name of the CUEsheet file to parse
 *
 * @return STL-like container with a size_type holding offsets
 */
auto get_offsets(const std::string &cuefilename) -> std::vector<int32_t>
{
	FILE* f = std::fopen(cuefilename.c_str(), "r");

	if (!f)
	{
		std::cerr << "Failed to open CUEsheet: " << cuefilename << '\n';
		throw std::runtime_error("Failed to open CUEsheet");
	}

	::Cd* cdinfo = ::cue_parse_file(f);

	if (std::fclose(f))
	{
		std::cerr << "Failed to close CUEsheet: " << cuefilename << '\n';
	}
	f = nullptr;

	if (!cdinfo)
	{
		std::cerr << "Failed to parse CUEsheet: " << cuefilename << '\n';
		throw std::runtime_error("Failed to parse CUEsheet");
	}

	const auto track_count = ::cd_get_ntrack(cdinfo);

	auto offsets = std::vector<int32_t>();
	offsets.reserve(static_cast<decltype(offsets)::size_type>(track_count));

	::Track* track = nullptr;
	auto offset    = long { 0 };

	for (auto i = int { 1 }; i <= track_count; ++i)
	{
		track  = ::cd_get_track(cdinfo, i);

		if (!track)
		{
			offsets.emplace_back(0);
			continue;
		}

		offset = ::track_get_start(track);

		if (offset <= std::numeric_limits<int32_t>::max())
		{
			offsets.emplace_back(static_cast<int32_t>(offset));
		} else
		{
			std::cerr << "Offset too big: " << offset << " frames, abort\n";
			::cd_delete(cdinfo);
			return {};
		}
	}

	::cd_delete(cdinfo);

	return offsets;
}


int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: albumcalc <cuesheet> <audiofile>" << '\n';
		return EXIT_SUCCESS;
	}

	// Of course you would validate your input parameters in production code.
	const auto cuefilename   { std::string { argv[1] }};
	const auto audiofilename { std::string { argv[2] }};

	// If you like, you can activate the internal logging of libarcstk to
	// see what's going on behind the scenes. We provide an appender for stdout.
	arcstk::Logging::instance().add_appender(
			std::make_unique<arcstk::Appender>("stdout", stdout));

	// 'INFO' means you should probably not see anything unless you give
	// libarcstk unexpected input.  Try 'DEBUG' or 'DEBUG1' if you want to
	// see more about what libarcstk is doing with your input.
	arcstk::Logging::instance().set_level(arcstk::LOGLEVEL::INFO);

	// Define input block size in number of samples, where 'sample' means a
	// 32 bit unsigned integer holding a pair of PCM 16 bit stereo samples.
	const auto samples_per_block { 16777216 }; // == 64 MB block size

	// Calculation will have to distinguish the tracks in the audiofile.
	// To identify the track bounds, we need the TOC, precisely:
	// 1. the leadout frame
	// 2. the number of tracks
	// 3. the track offset for each track

	// Since the CUEsheet usually does not know the length of the last track, we
	// have to derive the leadout frame from the audio data.  We could do this
	// quite convenient by using libarcstk's AudioReader::acquire_size() method.
	// But thanks to libsndfile, this is not even necessary: the information is
	// conveniently provided by the audiofile handle:

	// libsndfile API provides the file handle for the audio file
	SndfileHandle audiofile(audiofilename, SFM_READ);
	// Skip any sanity checks you would do in production code...

	const auto total_samples { audiofile.frames() /* It's 'samples'! */ };
	// Since this is of type 'long', we have to ensure that it fits for
	// conversion.
	if (total_samples * arcstk::CDDA::BYTES_PER_SAMPLE
			> std::numeric_limits<int32_t>::max())
	{
		std::cerr << "File too big ("
			<< total_samples * arcstk::CDDA::BYTES_PER_SAMPLE
			<< " bytes), abort\n";
	}

	const auto audiosize { arcstk::AudioSize {
		static_cast<int32_t>(total_samples), arcstk::UNIT::SAMPLES } };
	// Remark: what libsndfile calls "frames" is what libarcstk calls
	// "PCM 32 bit samples" or just "samples". Our "sample" represents a pair of
	// 16 bit stereo samples as a single 32 bit unsigned int (left/right).
	// Libsndfile's frame encodes the same information as 2 signed 16 bit
	// integers, one per channel.

	// We derive 2. total number of tracks and 3. actual track offsets from
	// parsing the CUEsheet.  We skip the details here for libarcstk does not
	// provide this functionality and the author just did a quick hack with
	// libcue.  (Just consult the implementation of function parse_cuesheet()
	// if you are interested in the details, but this is libcue, not libarcstk.)
	const auto offsets { get_offsets(cuefilename) };
	// Skip santiy checks and everything you could do with try/catch ...

	// We now have derived all relevant metadata from our input files.
	// Let's print it one last time before starting with the real business:
	for (decltype(offsets)::size_type i = 1; i < offsets.size(); ++i)
	{
		std::cout << "Track " << std::setw(2) << std::setfill(' ') << i
			<< " offset: "
			<< std::setw(6)  << std::setfill(' ') << offsets[i-1]
			<< '\n';
	}
	std::cout << "Track count: " << offsets.size()     << '\n';
	std::cout << "Leadout: "     << audiosize.frames() << '\n';

	// Step 1: Use libarcstk to construct the TOC
	// (cf. example 'albumid' for a different method to construct a ToC).
	const auto toc { arcstk::make_toc(audiosize.frames(), offsets) };

	// Step 2: Choose an Algorithm. For this occasion we choose AccurateRipV1V2
	// to get checksums for ARCSv2 as well as for ARCSv1.
	auto algorithm { std::make_unique<arcstk::AccurateRipV1V2>() };

	// Step 3: Create a Calculation and provide it with the context.
	// We do not specify a checksum type, thus the Calculation will provide
	// ARCSv1 as well as ARCSv2 values as default result.
	auto calculation { arcstk::make_calculation(std::move(algorithm), *toc) };

	// Let's enumerate the blocks in the output. This is just to give some
	// informative logging.
	const auto total_blocks
		{ 1 + (audiosize.samples() - 1) / samples_per_block };

	std::cout << "Expect " << total_blocks << " blocks" << '\n';

	// Provide simple input buffer for libsndfile's genuine sample/frame format.
	// We decide to want 16 bit signed integers.
	const auto buffer_len { samples_per_block * 2 };
	auto buffer { std::vector<int16_t>(buffer_len) };

	auto ints_in_block { int32_t { 0 }}; // Count ints read in single operation
	auto samples_read  { int64_t { 0 }}; // Count total samples actually read

	// The input buffer 'buffer' holds each 16 bit sample in a single integer.
	// Since we have stereo audio, there are two channels, which makes one
	// 16 bit integer per sample for each channel in interleaved (== not planar)
	// order, where the 16 bit sample for the left channel makes the start.
	// Libarcstk is not interested in those details, so we provide the samples
	// via a SampleSequence that abstracts the concrete format away:
	arcstk::InterleavedSamples<int16_t> sequence;
	// NOTE: These prerequisites are just provided by libsndfile at this
	// site in the code. In production code, you would of course verify
	// things... If the channel order is switched, the sample format is
	// changed or the sequence is planar, the example code will screw up!

	using std::cbegin;
	using std::cend;

	// Main loop: let libsndfile read the sample in its own format, normalize it
	// and update the prepared Calculation with the samples read in the current
	// loop run.
	while ((ints_in_block = audiofile.read(&buffer[0], buffer_len)))
	{
		// Check whether we have read the expected amount of samples in this run
		if (buffer_len != ints_in_block)
		{
			// Ok, no!  So, this must be the last block.  Check!

			const auto samples_in_block {
					ints_in_block / arcstk::CDDA::NUMBER_OF_CHANNELS };
			const auto samples_expected { audiosize.samples() - samples_read };

			if (samples_in_block != samples_expected)
			{
				// Unexpected number of samples for the last block.
				// This is an unrecoverable error, act accordingly here.
				std::cerr << "Expected " << buffer_len << " integers but got "
					<< ints_in_block << ". Bail out." << '\n';
				return EXIT_FAILURE;
			}

			// Adjust buffer size of the read buffer
			buffer.resize(
					static_cast<decltype(buffer)::size_type>(ints_in_block));
		}

		std::cout << "Read block " << (1 + samples_read / samples_per_block)
			<< "/" << total_blocks
			<< " (" << (buffer.size() / 2) << " samples)" << '\n';

		// Wrap buffer in a reusable SampleSequence
		sequence.wrap_int_buffer(&buffer[0], buffer.size());

		// Count PCM 32 bit stereo samples processed.
		samples_read += sequence.size();
		// We could also compute the number of samples ourselves:
		// buffer.size() / static_cast<unsigned int>(CDDA::NUMBER_OF_CHANNELS)

		// Note: since libsndfile has told us the total sample count, we were
		// able to configure the context with the correct leadout.
		// Otherwise, we would not yet know the leadout frame number. If that
		// were the case we would have to provide our Calculation with this
		// information manually by doing:
		//
		// calculation.update_audiosize(samples_read);
		//
		// _before_ we send the last block of samples to it. This is absolutely
		// essential since otherwise the Calculation will not know when to stop
		// and eventually fail. It is sufficient to update the audio size
		// just before the last block of samples is passed to Calculation. Since
		// we can recognize the last block as demonstrated above, we can also
		// count the total number of samples read before the last update.

		// Update calculation with next portion of normalized samples.
		calculation->update(cbegin(sequence), cend(sequence));
	}

	// Ok, no more samples. We demonstrate that the Calculation is complete:
	if (calculation->complete())
	{
		std::cout << "Calculation complete" << '\n';
	} else
	{
		std::cerr << "Error, calculation incomplete" << '\n';
	}
	std::cout << "Read " << samples_read << " samples" << '\n';

	// Let's finally get us the result!
	auto checksums { calculation->result() };

	// And now, the time has come: print the actual checksums.
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

	return EXIT_SUCCESS;
}


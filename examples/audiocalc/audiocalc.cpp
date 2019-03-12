#include <cstdint>   // for uint32_t etc.
#include <cstdio>    // for fopen, fclose, FILE
#include <iomanip>   // for setw, setfill, hex
#include <iostream>  // for cerr, cout
#include <stdexcept> // for runtime_error
#include <string>    // for string

extern "C" {
#include <libcue/libcue.h>  // libcue for parsing the CUEsheet
}

#include <sndfile.hh>       // libsndfile for reading the audio file

#ifndef __LIBARCS_CALCULATE_HPP__ // libarcs: calculate ARCSs
#include <arcs/calculate.hpp>
#endif
#ifndef __LIBARCS_SAMPLES_HPP__   // libarcs: normalize input samples
#include <arcs/samples.hpp>
#endif

#ifndef __LIBARCS_LOGGING_HPP__   // libarcs: log what you do
#include <arcs/logging.hpp>
#endif


// ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !
// NOTE! THIS IS EXAMPLE CODE! IT IS INTENDED TO DEMONSTRATE HOW LIBARCS COULD
// BE USED. IT IS NOT INTENDED TO BE USED IN REAL LIFE PRODUCTION. IT IS IN NO
// WAY TESTED FOR PRODUCTION. TAKE THIS AS A STARTING POINT TO YOUR OWN
// SOLUTION, NOT AS A TOOL.
// ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !


/**
 * Parse a CUEsheet and return offsets and implicitly the track count.
 *
 * @param[in] cuefilename Name of the CUEsheet file to parse
 *
 * @return Offsets
 */
auto parse_cuesheet(const std::string &cuefilename)
{
	FILE* f = std::fopen(cuefilename.c_str(), "r");

	if (!f)
	{
		std::cerr << "Failed to open CUEsheet: " << cuefilename << std::endl;
		throw std::runtime_error("Failed to open CUEsheet");
	}

	::Cd* cdinfo = ::cue_parse_file(f);

	if (std::fclose(f))
	{
		std::cerr << "Failed to close CUEsheet: " << cuefilename << std::endl;
	}

	if (!cdinfo)
	{
		std::cerr << "Failed to parse CUEsheet: " << cuefilename << std::endl;
		throw std::runtime_error("Failed to parse CUEsheet");
	}

	Track* track = nullptr;
	std::vector<int> offsets;

	auto track_count = ::cd_get_ntrack(cdinfo);
	offsets.reserve(track_count);

	for (int i = 1; i <= track_count; ++i)
	{
		track = ::cd_get_track(cdinfo, i);

		if (!track)
		{
			offsets.emplace_back(0);
			continue;
		}

		offsets.emplace_back(::track_get_start(track));
	}

	::cd_delete(cdinfo);

	return offsets;
}


int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: audiocalc <cuesheet> <audiofile.wav>" << std::endl;
		return EXIT_SUCCESS;
	}

	// Of course you would validate your input parameters in production code.
	const std::string cuefilename   {argv[1]};
	const std::string audiofilename {argv[2]};

	// If you like, you can activate the internal logging of libarcs to
	// see what's going on behind the scenes. We provide an appender for stdout
	// and set the loglevel to 'INFO', which means you should probably not see
	// anything unless you give libarcs unexpected input.
	arcs::Logging::instance().add_appender(
			std::make_unique<arcs::Appender>("stdout", stdout));

	// Set this to LOG_DEBUG or LOG_DEBUG1 if you want to see what libarcs is
	// doing with your input
	arcs::Logging::instance().set_level(arcs::LOG_INFO);

	// Define input block size (in number of samples)
	const int samples_per_block { 16777216 }; // == 64 MB block size

	// libsndfile provides file handle
	SndfileHandle audiofile(audiofilename, SFM_READ);
	// Skip any santiy checks you would do in production code...

	// Calculation will have to distinguish the tracks in the audiofile, so we
	// need the TOC, precisely:
	// 1. the number of tracks
	// 2. the track offset for each track
	// 3. the leadout frame

	// Since the CUEsheet usually does not know the length of the last track, we
	// have to derive the leadout frame from the audio data. The leadout frame
	// can easily be deduced from the total number of samples. Thanks to
	// libsndfile, this information is conveniently provided by the audiofile
	// handle:
	arcs::AudioSize total_samples;
	total_samples.set_sample_count(audiofile.frames());
	// Remark: what libsndfile calls "frames" is what libarcs calls
	// "PCM 32 samples". Our "sample" represents a single stereo sample as
	// a single 32 bit unsigned int (left/right), libsndfile's frame encodes the
	// same information as 2 signed 16 bit integers, one per channel.

	// One completed, two to go. We derive track number and offsets from parsing
	// the CUEsheet. We skip the details here. (Just consult the implementation
	// of function parse_cuesheet if you are interested in the details.)
	auto offsets { parse_cuesheet(cuefilename) };
	// Skip santiy checks ...

	// We now have derived all relevant metadata from our input files.
	// Let's print it one last time before starting with the real business:
	for (unsigned int i = 1; i < offsets.size(); ++i)
	{
		std::cout << "Track " << std::setw(2) << std::setfill(' ') << i
			<< " offset: "
			<< std::setw(6)  << std::setfill(' ') << offsets[i-1]
			<< std::endl;
	}
	std::cout << "Track count: " << offsets.size()                << std::endl;
	std::cout << "Leadout: "     << total_samples.leadout_frame() << std::endl;

	// Use libarcs to construct the TOC. This validates the parsed toc data
	// and will fail if the parsed data is inconsistent.
	auto toc { arcs::make_toc(offsets.size(), offsets,
			total_samples.leadout_frame()) };

	// Create a context from the TOC along with the name of the audiofile.
	auto context { arcs::make_context(audiofilename, *toc) };

	// From this context, create a Calculation.
	// Since we did not specify a checksum type, the Calculation will provide
	// ARCSv1 as well as ARCSv2 value.
	arcs::Calculation calculation { std::move(context) };

	// Since libsndfile has told us the sample count, our TOC object
	// already contains the leadout frame number. If that were not the case we
	// would have to provide our Calculation with this information manually
	// by doing:
	//
	// calculation.update_audiosize(total_samples);
	//
	// This is absolutely essential since otherwise the Calculation will fail.
	// It is sufficient to do this before the last block of samples is passed to
	// Calculation.

	// Let's enumerate the blocks in the output. This is just to give some
	// informative logging.
	auto total_blocks
		{ 1 + (total_samples.sample_count() - 1) / samples_per_block };

	std::cout << "Expect " <<  total_blocks << " blocks" << std::endl;

	// Provide simple input buffer for libsndfile's genuine sample/frame format.
	// We decide to want 16 bit signed integers.
	uint32_t buffer_len { samples_per_block * 2 };
	std::vector<int16_t> buffer(buffer_len);

	uint32_t ints_in_block {0}; // Count integers read in single operation
	uint64_t sample_count  {0}; // Count total samples that were actually read

	// Main loop: let libsndfile read the sample in its own format, normalize it
	// and update the prepared Calculation with the samples read in the current
	// loop run
	while ((ints_in_block = audiofile.read(&buffer[0], buffer_len)))
	{
		// Check whether we have read the expected amount of samples in this run
		if (buffer_len != ints_in_block)
		{
			// Did we expect this?
			auto expected_total
			{calculation.context().audio_size().sample_count() - sample_count};

			if (ints_in_block / arcs::CDDA.NUMBER_OF_CHANNELS != expected_total)
			{
				// Wrong number of samples
				// This is an unrecoverable error, act accordingly here.
				std::cerr << "Expected " << buffer_len << " integers but got "
					<< ints_in_block << ". Bail out." << std::endl;
				return EXIT_FAILURE;
			}

			// Last block is smaller, adjust buffer sizes of the read buffer as
			// well as of the conversion buffer
			buffer.resize(ints_in_block);
		}

		std::cout << "Read block " << (1 + sample_count / samples_per_block)
			<< "/" << total_blocks
			<< " (" << (buffer.size() / 2) << " samples)" << std::endl;

		// The input buffer holds each 16 bit sample in a single integer.
		// Since we have stereo audio, there are two channels, which makes one
		// 16 bit integer per sample for each channel in interleaved order,
		// where the 16 bit sample for the left channel makes the start.
		// Libarcs is not interested in those details, so we provide the samples
		// via a SampleSequence that abstracts the concrete format away:
		arcs::SampleSequence<int16_t, false> sequence;
		// Each sample is a signed 16 bit integer, the sequence is interleaved
		// and the channel ordering is left,right.
		// NOTE: These prerequisites are just provided by libsndfile at this
		// site in the code. In production code, you would of course verify
		// things.. If the channel order is switched, the sample format is
		// changed or the sequence is planar, the example code will screw up!

		// Wrap buffer in SampleSequence
		sequence.reset(&buffer[0], buffer.size());

		// Update calculation with next portion of normalized samples
		calculation.update(sequence.begin(), sequence.end());

		// Count PCM stereo samples processed
		sample_count += (buffer.size() / 2);
	}

	// Ok, no more samples. We demonstrate that the Calculation is complete:
	if (calculation.complete())
	{
		std::cout << "Calculation complete" << std::endl;
	} else
	{
		std::cerr << "Error, calculation incomplete" << std::endl;
	}

	// Let's finally get us the result
	auto checksums { calculation.result() };

	// And now, the time has come: print the checksums
	std::cout << "Track  ARCSv1    ARCSv2" << std::endl;
	int trk_no = 1;
	using type = arcs::checksum::type;
	for (const auto& values : checksums)
	{
		auto arcs1 = values.get(type::ARCS1);
		auto arcs2 = values.get(type::ARCS2);

		std::cout << std::dec << " " << std::setw(2) << std::setfill(' ')
			<< trk_no << "   " << std::hex << std::uppercase
			<< std::setw(8) << std::setfill('0') << arcs1.value()
			<< "  "
			<< std::setw(8) << std::setfill('0') << arcs2.value()
			<< std::endl;
		++trk_no;
	}
}

#include <cstdint>   // for uint32_t etc.
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
#ifndef __LIBARCS_IDENTIFIER_HPP__ // libarcs: parse AccurateRip responses
#include <arcs/identifier.hpp>
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


/**
 * Analyze the audiofile and return the total number of samples (if decoded).
 *
 * @param[in] audiofilename Name of the audio file to analyze
 *
 * @return Total number of samples
 */
auto total_samples(const std::string &audiofilename)
{
	// libsndfile provides file handle
	SndfileHandle audiofile(audiofilename, SFM_READ);
	// Skip any santiy checks you would do in production code...

	return audiofile.frames();
}


int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: albumid <cuesheet> <audiofile.wav>" << std::endl;
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

	// To calculate the AccurateRip id of the CD, we need its TOC, precisely:
	// 1. the number of tracks
	// 2. the track offset for each track
	// 3. the leadout frame

	// Since the CUEsheet usually does not know the length of the last track, we
	// have to derive the leadout frame from the audio data. The leadout frame
	// can easily be deduced from the total number of samples. Thanks to
	// libsndfile, this information is conveniently provided by the audiofile
	// handle:
	arcs::AudioSize audio_size;
	audio_size.set_sample_count(total_samples(audiofilename));
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
	std::cout << "Track count: " << offsets.size()             << std::endl;
	std::cout << "Leadout: "     << audio_size.leadout_frame() << std::endl;

	// Use libarcs to construct the TOC. This validates the parsed toc data
	// and will fail if the parsed data is inconsistent.
	auto toc { arcs::make_toc(offsets.size(), offsets,
			audio_size.leadout_frame()) };

	// Since the TOC is guaranteed to be complete, i.e. yield a non-zero
	// leadout, we can now construct the ID directly from the TOC.
	auto id { arcs::make_arid(*toc) };

	std::cout << "ID: " << id->to_string() << std::endl;
	std::cout << "Filename: " << id->filename() << std::endl;
	std::cout << "Request-URL: " << id->url() << std::endl;

	return EXIT_SUCCESS;
}


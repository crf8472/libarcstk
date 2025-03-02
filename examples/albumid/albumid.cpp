//
// Example for calculating AccurateRip id and specific query URL of an album,
// represented by a CueSheet and a single losslessly encoded audio file.
//


#include <limits>
#ifndef __LIBARCSTK_CALCULATE_HPP__  // libarcstk: calculate ARCSs
#include "calculate.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__ // libarcstk: calculate AccurateRip ids
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_METADATA_HPP__   // libarcstk: Compact Disc metadata
#include "metadata.hpp"
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__    // libarcstk: log what you do
#include "logging.hpp"
#endif

extern "C" {
#include <libcue/libcue.h>        // libcue for parsing the Cuesheet
}
#include <sndfile.hh>             // libsndfile for reading the audio file

#include <cstdint>   // for uint32_t etc.
#include <cstdio>    // for fopen, fclose, FILE
#include <cstdlib>   // for EXIT_SUCCESS
#include <iomanip>   // for setw, setfill, hex
#include <iostream>  // for cerr, cout
#include <stdexcept> // for runtime_error
#include <string>    // for string
#include <vector>    // for vector


// ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !
// NOTE! THIS IS EXAMPLE CODE! IT IS INTENDED TO DEMONSTRATE HOW LIBARCSTK COULD
// BE USED. IT IS NOT INTENDED TO BE USED IN REAL LIFE PRODUCTION. IT IS IN NO
// WAY TESTED FOR PRODUCTION. TAKE THIS AS A STARTING POINT TO YOUR OWN
// SOLUTION, NOT AS A TOOL.
// ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !


/**
 * \brief Parse a Cuesheet and return offsets and implicitly the track count.
 *
 * This method is implemented without any use of libarcstk. It just has to be
 * available for parsing the CueSheet.
 *
 * @param[in] cuefilename Name of the Cuesheet file to parse
 *
 * @return STL-like container with a size_type holding offsets
 */
auto get_offsets(const std::string &cuefilename) -> std::vector<int32_t>
{
	FILE* f { std::fopen(cuefilename.c_str(), "r") };

	if (!f)
	{
		std::cerr << "Failed to open Cuesheet: " << cuefilename << '\n';
		throw std::runtime_error("Failed to open Cuesheet");
	}

	::Cd* cdinfo = ::cue_parse_file(f);

	if (std::fclose(f))
	{
		std::cerr << "Failed to close Cuesheet: " << cuefilename << '\n';
	}
	f = nullptr;

	if (!cdinfo)
	{
		std::cerr << "Failed to parse Cuesheet: " << cuefilename << '\n';
		throw std::runtime_error("Failed to parse Cuesheet");
	}

	const auto track_count { ::cd_get_ntrack(cdinfo) };

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


/**
 * Analyze the audiofile and return the total number of frames (if decoded).
 *
 * This method is implemented without any use of libarcstk. It just has to be
 * available for abstracting away how to get the amount of total frames. You
 * _could_ use libarcstk for this, but in this situation, libsndfile provides a
 * very convenient way.
 *
 * @param[in] audiofilename Name of the audio file to analyze
 *
 * @return Total number of frames
 */
auto get_total_frames(const std::string &audiofilename) -> int32_t
{
	// libsndfile provides file handle
	SndfileHandle audiofile(audiofilename, SFM_READ);
	// Skip any santiy checks you would do in production code...

	// Remark: what libsndfile calls "frames" is what libarcstk calls
	// "PCM 32 bit samples" or just "samples". Our "sample" represents a pair of
	// 16 bit stereo samples as a single 32 bit unsigned int (left/right).
	// Libsndfile's frame encodes the same information as 2 signed 16 bit
	// integers, one per channel. However, we have to convert it.
	const auto frames { audiofile.frames() / arcstk::CDDA::SAMPLES_PER_FRAME };

	if (frames <= std::numeric_limits<int32_t>::max())
	{
		return static_cast<int32_t>(frames);
	}

	std::cerr << "File too big (" << frames * 2352 << " bytes), abort\n";

	return 0;
}


int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		std::cout << "Usage: albumid <cuesheet> <audiofile>" << '\n';
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

	// Calculation will have to distinguish the tracks in the audiofile.
	// To identify the track bounds, we need the TOC, precisely:
	// 1. the number of tracks
	// 2. the track offset for each track
	// 3. the leadout frame

	// We derive 1. total number of tracks and 2. actual track offsets from
	// parsing the Cuesheet.  We skip the details here for libarcstk does not
	// provide this functionality and the author just did a quick hack with
	// libcue.  (Just consult the implementation of function parse_cuesheet()
	// if you are interested in the details, but this is libcue, not libarcstk.)
	const auto offsets { get_offsets(cuefilename) };
	// Skip santiy checks and everything you could do with try/catch ...

	// Two completed, one to go.  Since the Cuesheet usually does not know the
	// length of the last track, we have to derive the leadout frame from the
	// audio data.  We could do this quite convenient by using libarcstk's
	// AudioReader::acquire_size() method.  But thanks to libsndfile, this is
	// not even necessary: the information is conveniently provided by the
	// audiofile handle:
	const auto leadout { get_total_frames(audiofilename) };

	// We now have derived all relevant metadata from our input files.
	// Let's print it one last time before starting with the real business:
	for (decltype(offsets)::size_type i = 1; i < offsets.size(); ++i)
	{
		std::cout << "Track " << std::setw(2) << std::setfill(' ') << i
			<< " offset: "
			<< std::setw(6)  << std::setfill(' ') << offsets[i-1]
			<< '\n';
	}
	std::cout << "Leadout:         " << leadout        << '\n';
	std::cout << "Track count:     " << offsets.size() << '\n';

	// Step 1: Use libarcstk to construct the ToC.

	// There are several methods to achieve this. Here we construct a ToCData
	// object which is accepted as argument by the ToC Constructor.
	// You could also use make_toc(), which returns a unique_ptr and omits
	// validation.
	const auto toc_data { arcstk::toc::construct(leadout, offsets) };

	// The method demonstrated here has the advantage that the data can be
	// validated before constructing the ToC. You could do something like:
	try
	{
		// This validates the parsed toc data and will throw if the parsed data
		// is inconsistent.
		arcstk::toc::validate(toc_data);

	} catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	// Step 2: Since the TOC is guaranteed to be complete, i.e. yield a non-zero
	// leadout, we can now construct the AccurateRip ID directly from the TOC.
	const auto id { arcstk::make_arid(arcstk::ToC(toc_data)) };

	// Print the ARId.
	using std::string;
	std::cout << "ID:          " << to_string(*id) << '\n';
	std::cout << "Filename:    " << id->filename() << '\n';
	std::cout << "Request-URL: " << id->url()      << '\n';

	return EXIT_SUCCESS;
}


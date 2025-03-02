//
// Example for matching local AccurateRip checksums against the checksums
// provided by AccurateRip.
//

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"             // for Checksums
#endif
#ifndef __LIBARCSTK_DBAR_HPP__
#include "dbar.hpp"                  // for DBAR
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"            // for ARId
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__    // libarcstk: log what you do
#include "logging.hpp"
#endif
#ifndef __LIBARCSTK_VERIFY_HPP__     // libarcstk: match Checksums and DBAR
#include "verify.hpp"
#endif

#include <algorithm> // for count
#include <cstdint>   // for uint32_t etc.
#include <cstdlib>   // for EXIT_SUCCESS
#include <cstring>   // for strtok
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


// NOTE 2: This example is rather long, much longer than I would prefer it to
// be. The reason for this longishness is that for using the match interface,
// you have to add code that prepares and provides the following input:
//
// - your own sums, i.e. the sums you have from your rip
// - the reference sums from AccurateRip and
// - the AccurateRip id of the album.
//
// This requires some boilerplate code. Although that boilerplate does not carry
// any didactic evidence concerning libarcstk, I wanted to provide an example
// that at least demonstrates the added value on your own input data.
// In fact, the functions
//
// parse_arid(),
// parse_input_arcs() and
// parse_match_arcs()
//
// are more or less quick and dirty dummies for just providing the required
// input values. This is not related to getting acquainted to the libarcstk API.
// The actual example demonstrating the use of the AlbumMatcher class is
// contained in main(). It's very simple to use. Have fun!


/**
 * \brief Parse the ARId from the command line.
 *
 * @param[in] input_id A string representation of the ARId
 *
 * @return Parsed ARId
 */
arcstk::ARId parse_arid(const char* input_id)
{
	const std::string id_str { input_id };

	const uint16_t track_count = std::stoul(id_str.substr(0, 3), nullptr, 10);
	const uint32_t id_1 = std::stoul(id_str.substr(4, 8), nullptr, 16);
	const uint32_t id_2 = std::stoul(id_str.substr(13, 8), nullptr, 16);
	const uint32_t cddb_id = std::stoul(id_str.substr(22, 8), nullptr, 16);

	return arcstk::ARId(track_count, id_1, id_2, cddb_id);
}


/**
 * \brief Parse a comma-separated list of hexadecimal numbers.
 *
 * @param[in] list A string representation of the list
 * @param[in] t    The checksum type to declare (either ARCS1 or ARCS2)
 *
 * @return Parsed Checksums
 */
arcstk::Checksums parse_input_arcs(const char* list,
		const arcstk::checksum::type t)
{
	const std::string checksum_list { list };

	const auto total_tracks { static_cast<std::size_t>(
		1 + std::count(checksum_list.begin(), checksum_list.end(), ',')) };

	std::string::size_type token_start { 0 };
	std::string::size_type token_end   { checksum_list.find_first_of(',') };

	auto prev_settings { std::cout.flags() };
	std::cout << "My checksums to match:" << '\n';

	std::string token; // current token
	auto arcs = uint32_t { 0 };  // ARCS of the current token
	arcstk::Checksums checksums { total_tracks };
	for (std::size_t i = 0; i < total_tracks; ++i)
	{
		token = checksum_list.substr(token_start, token_end - token_start);
		arcs  = std::stoul(token, nullptr, 16);

		std::cout << "Track "
			<< std::dec
			<< std::setw(2) << std::setfill(' ') << (i + 1)
			<< ": "
			<< std::hex << std::uppercase
			<< std::setw(8) << std::setfill('0')
			<< arcs
			<< std::dec << "  (chars: "
			<< std::setw(3) << std::setfill(' ') << token_start
			<< " - " << std::setw(3) << std::setfill(' ') << token_end << ")"
			<< '\n';

		auto track_sum = arcstk::ChecksumSet { 0 };
		track_sum.insert(t, arcstk::Checksum(arcs));
		checksums.push_back(track_sum);

		token_start = token_end + 1;
		token_end   = checksum_list.find_first_of(',', token_start);

		if (token_end == std::string::npos)
		{
			token_end = checksum_list.length();
		}
	}
	std::cout.flags(prev_settings);

	return checksums;
}


/**
 * \brief Parse ARCSs from a non-empty response file or from stdin.
 *
 * @param[in] filename Name of the response file
 *
 * @return Parsed ARResponse
 *
 * @throws std::runtime_error           If filename is empty
 * @throws arcstk::StreamParseException If parsing fails
 */
arcstk::DBAR parse_match_arcs(const std::string &filename)
{
	if (filename.empty())
	{
		throw std::runtime_error("Filename must not be empty!");
	}

	return arcstk::load_file(filename);
}


int main(int argc, char* argv[])
{
	// Do only the absolutely inevitable checking
	if (argc < 3 or argc > 4)
	{
		std::cout << "Usage: "
			<< "albumverify --id=<ARId> --arcs2=0xA,0xB,0xC,... <dbar_file.bin>"
			<< '\n';

		return EXIT_SUCCESS;
	}

	// If you like, you can activate the internal logging of libarcstk to
	// see what's going on behind the scenes. We provide an appender for stdout
	// and set the loglevel to 'INFO', which means you should probably not see
	// anything unless you give libarcstk unexpected input.
	arcstk::Logging::instance().add_appender(
			std::make_unique<arcstk::Appender>("stdout", stdout));

	// Set this to DEBUG or DEBUG1 if you want to see what libarcstk is
	// doing with your input
	arcstk::Logging::instance().set_level(arcstk::LOGLEVEL::INFO);

	// Parse the AccurateRip id of the album passed from the command line
	const arcstk::ARId arid { parse_arid(argv[1] + 5) };

	std::cout << "Album ID: " << to_string(arid) << '\n';

	// Parse declared ARCS type (ARCSv1 or ARCSv2)
	arcstk::checksum::type type { argv[2][6] == '1'
		? arcstk::checksum::type::ARCS1
		: arcstk::checksum::type::ARCS2 };

	// Parse the checksums of the album passed from the command line
	const arcstk::Checksums checksums { parse_input_arcs(argv[2] + 8, type) };

	// Parse the checksums to be matched from file or stdin
	std::string filename;
	if (argc == 4)
	{
		filename = std::string(argv[3]);
	}
	const arcstk::DBAR dBAR { parse_match_arcs(filename) };

	// Now the interesting part: peform the match.
	// The AlbumVerifier class targets situations in which you have a list of
	// checksums and you _know_ in which order they form the album. Therefore
	// AlbumVerifier is the device of choice here.
	arcstk::AlbumVerifier verifier { checksums, arid };
	// It may also be the case that you have just some tracks of an album or you
	// cannot be sure about the order. In this case, you would use the
	// arcstk::TracksetVerifier
	const auto result { verifier.perform(dBAR) };
	const auto best = result->best_block();

	// Inform about the result
	std::cout << "RESULT: ";
	if (result->all_tracks_verified())
	{
		std::cout << "Response contains a total match in block "
			<< std::get<0>(best)
			<< ", which is of type ARCSv" << (std::get<1>(best) + 1)
			<< "." << '\n';
	} else
	{
		std::cout << "No total match. Best block is "
			<< std::get<0>(best)
			<< ", which is of type ARCSv" << (std::get<1>(best) + 1)
			<< " with difference " << std::get<2>(best)
			<< '\n';
	}

	// And now print the gory details

	const auto block     { std::get<0>(best) };
	auto trackno         { 0 };
	auto is_match = bool { false };

	auto prev_settings { std::cout.flags() };

	std::cout << "TRACK   MINE      THEIRS\n";

	for (const auto& track : dBAR.block(block))
	{
		// The match object stores flags for every check that the matcher
		// has performed. Thus, the result of the matching can be queried on the
		// match object by just giving the coordinate block/track/version.
		is_match =
			result->track(block, trackno,
					type == arcstk::checksum::type::ARCS2);

		std::cout << " " << std::dec << std::setw(2) << std::setfill('0')
			<< (trackno + 1) << ":  ";

		std::cout << std::hex << std::uppercase;

		std::cout << std::setw(8) << std::setfill('0')
			<< checksums[trackno].get(type).value();

		std::cout << (is_match ? " = " : "   ") ;

		std::cout << std::setw(8) << std::setfill('0') << track.arcs();

		std::cout << (is_match ? " [OK]" : " <- FAIL") ;

		std::cout << '\n';

		++trackno;
	}

	std::cout.flags(prev_settings);

	return EXIT_SUCCESS;
}


//
// Example for matching local AccurateRip checksums against the checksums
// provided by AccurateRip.
//

#include <algorithm> // for count
#include <cstdint>   // for uint32_t etc.
#include <cstring>   // for strtok
#include <iomanip>   // for setw, setfill, hex
#include <iostream>  // for cerr, cout
#include <stdexcept> // for runtime_error
#include <string>    // for string

#ifndef __LIBARCSTK_MATCH_HPP__      // libarcstk: match Checksums and ARResponse
#include <arcstk/match.hpp>
#endif
#ifndef __LIBARCSTK_CALCULATE_HPP__
#include <arcstk/calculate.hpp>      // for arcstk::Checksums
#endif
#ifndef __LIBARCSTK_PARSE_HPP__
#include <arcstk/parse.hpp>          // for arcstk::ARResponse
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__    // libarcstk: log what you do
#include <arcstk/logging.hpp>
#endif


// ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !
// NOTE! THIS IS EXAMPLE CODE! IT IS INTENDED TO DEMONSTRATE HOW LIBARCS COULD
// BE USED. IT IS NOT INTENDED TO BE USED IN REAL LIFE PRODUCTION. IT IS IN NO
// WAY TESTED FOR PRODUCTION. TAKE THIS AS A STARTING POINT TO YOUR OWN
// SOLUTION, NOT AS A TOOL.
// ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !


// NOTE 2: This example is rather long, much longer than I would prefer it to
// be. The reason for this longishness is that for using the match interface,
// you have to prepare a lot of input:
//
// - your own sums, i.e. the sums you have from your rip
// - the reference sums from AccurateRip and
// - the AccurateRip id of the album.
//
// Providing all this input in a way acceptable in example code, some
// boilerplate code is inevitably. Although the boilerplate does not carry any
// didactic evidence for libarcstk, I wanted to provide an example that  at least
// demonstrates the added value on your own input data.
// In fact, the functions parse_arid(), parse_input_arcs() and
// parse_match_arcs() are more or less quick and dirty boilerplate code for just
// providing the required input values. In real life code, you would have to
// invest (much) more effort to make the code robust but this is not related to
// learning the libarcstk API.
// The actual example demonstrating the use of the AlbumMatcher class is
// contained in main(). It's very simple to use. Have fun!


/**
 * Parse the ARId from the command line.
 *
 * @param[in] input_id A string representation of the ARId
 *
 * @return Parsed ARId
 */
arcstk::ARId parse_arid(const char* input_id)
{
	const std::string id_str { input_id };

	uint16_t track_count = std::stoul(id_str.substr(0, 3), nullptr, 10);
	uint32_t id_1 = std::stoul(id_str.substr(4, 8), nullptr, 16);
	uint32_t id_2 = std::stoul(id_str.substr(13, 8), nullptr, 16);
	uint32_t cddb_id = std::stoul(id_str.substr(22, 8), nullptr, 16);

	return arcstk::ARId(track_count, id_1, id_2, cddb_id);
}


/**
 * Parse a comma-separated list of hexadecimal numbers.
 *
 * @param[in] list A string representation of the list
 * @param[in] t The checksum type to declare (either ARCS1 or ARCS2)
 *
 * @return Parsed Checksums
 */
arcstk::Checksums parse_input_arcs(const char* list, const arcstk::checksum::type t)
{
	const std::string checksum_list { list };

	const size_t total_tracks =
		1 + std::count(checksum_list.begin(), checksum_list.end(), ',');

	arcstk::Checksums checksums { total_tracks };

	std::string token;
	uint32_t arcs;

	std::string::size_type token_start = 0;
	std::string::size_type token_end   = checksum_list.find_first_of(',');

	auto prev_settings = std::cout.flags();
	std::cout << "My checksums to match:" << std::endl;
	for (std::size_t i = 0; i < total_tracks; ++i)
	{
		token = checksum_list.substr(token_start, token_end - token_start);
		arcs = std::stoul(token, nullptr, 16);

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

		checksums[i].insert(t, arcstk::Checksum(std::stoul(token, nullptr, 16)));
		token_start = token_end + 1;
		token_end = checksum_list.find_first_of(',', token_start);

		if (token_end == std::string::npos)
		{
			token_end = checksum_list.length();
		}
	}
	std::cout.flags(prev_settings);

	return checksums;
}


/**
 * Parse ARCSs from a non-empty response file or from stdin.
 *
 * @param[in] filename Name of the response file (maybe empty, then stdin)
 *
 * @return Parsed ARResponse
 */
arcstk::ARResponse parse_match_arcs(const std::string &filename)
{
	auto content_hdlr = std::make_unique<arcstk::DefaultContentHandler>();

	arcstk::ARResponse response_data;
	content_hdlr->set_object(response_data);

	auto error_hdlr = std::make_unique<arcstk::DefaultErrorHandler>();

	std::unique_ptr<arcstk::ARStreamParser> parser;

	if (filename.empty())
	{
		parser = std::make_unique<arcstk::ARStdinParser>();
	} else
	{
		parser = std::make_unique<arcstk::ARFileParser>(filename);
	}

	// Register parser handlers
	parser->set_content_handler(std::move(content_hdlr));
	parser->set_error_handler(std::move(error_hdlr));

	// Run parser (this may throw!)
	parser->parse();

	return response_data;
}


int main(int argc, char* argv[])
{
	// Do only the absolutely inevitable checking
	if (argc < 3 or argc > 4)
	{
		std::cout <<
			"Usage: albumverify --id=<ARId> --arcs2=0xA,0xB,0xC,... <file.bin>"
			<< std::endl;

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
	const arcstk::ARId arid = parse_arid( argv[1] + 5 );

	std::cout << "Album ID: " << arid.to_string() << '\n';

	// Parse declared ARCS type (ARCSv1 or ARCSv2)
	arcstk::checksum::type type = argv[2][6] == '1'
		? arcstk::checksum::type::ARCS1
		: arcstk::checksum::type::ARCS2;

	// Parse the checksums of the album passed from the command line
	const arcstk::Checksums checksums = parse_input_arcs( argv[2] + 8, type );

	// Parse the checksums to be matched from file or stdin
	std::string filename;
	if (argc == 4)
	{
		filename = std::string(argv[3]);
	}
	const arcstk::ARResponse arcss = parse_match_arcs(filename);

	// Now the interesting part: peform the match.
	// The AlbumMatcher class targets situations in which you have a list of
	// checksums and you _know_ in which order they form the album. Therefore
	// AlbumMatcher is the device of choice here.
	arcstk::AlbumMatcher matcher(checksums, arid, arcss);
	// It may also be the case that you have just some tracks of an album or you
	// cannot be sure about the order. In this case, you would use the
	// arcstk::AnyMatcher.

	// Inform about the result
	std::cout << "RESULT: ";
	if (matcher.matches())
	{
		std::cout << "Response contains a total match in block "
			<< matcher.best_match()
			<< ", which is of type ARCSv" << (matcher.matches_v2() + 1)
			<< "." << std::endl;
	} else
	{
		std::cout << "No total match. Best block is "
			<< matcher.best_match()
			<< ", which is of type ARCSv" << (matcher.matches_v2() + 1)
			<< " with difference " << matcher.best_difference()
			<< std::endl;
	}

	// And now print the gory details

	auto match { matcher.match() };
	auto block { matcher.best_match() };
	int trackno {0};
	bool is_match = false;

	auto prev_settings = std::cout.flags();

	std::cout << "TRACK   MINE      THEIRS\n";

	for (const auto& track : arcss[block])
	{
		// The match object stores flags for every check that the matcher
		// has performed. Thus, the result of the matching can be queried on the
		// match object by just giving the coordinate block/track/version.
		is_match =
			match->track(block, trackno, type == arcstk::checksum::type::ARCS2);

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


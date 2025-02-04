//
// Example for parsing the binary AccurateRip query response to plaintext.
//

#ifndef __LIBARCSTK_DBAR_HPP__       // libarcstk: parse AccurateRip responses
#include "dbar.hpp"
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__ // libarcstk: calculate AccurateRip ids
#include "identifier.hpp"
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__    // libarcstk: log what you do
#include "logging.hpp"
#endif

#include <fstream>   // for ifstream
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

using arcstk::DBAR;
using arcstk::StreamParseException;

/**
 * A sophisticated way to read from a file via stream.
 *
 * Provides fine granular control over the input stream.
 */
DBAR load_sophisticated(const std::string& filename)
{
	// This example shows a way with fine granular control over the input
	// stream.

	// Open file if file is readable.

	std::ifstream stream;
	stream.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		stream.open(filename, std::ifstream::in | std::ifstream::binary);
	}
	catch (const std::ifstream::failure& f)
	{
		throw std::runtime_error(
			std::string("Failed to open file '") + filename +
			std::string("', got: ") + typeid(f).name() +
			std::string(", message: ") + f.what());
	}

	// Perform parsing

	using arcstk::DBARBuilder;
	using arcstk::DBARBlock;
	using arcstk::parse_stream;

	DBARBuilder builder;
	try {

		const auto total_bytes { parse_stream(stream, &builder, nullptr) };
		std::cout << "Read " << total_bytes << " bytes" << '\n';
		stream.close();

	} catch (const StreamParseException& e)
	{
		stream.close();
		throw;
	}
	// ... normally you would also catch other possible exceptions, we just
	// concentrate on libarcstk.

	return builder.result();
}

/**
 * A simple way to read from a file.
 */
DBAR load_simple(const std::string& filename)
{
	// This shows a very simple way to parse a file with reasonable defaults.

	return arcstk::load_file(filename);
}


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		std::cout << "Usage: arparse <response_file_name>" << '\n';
		return EXIT_SUCCESS;
	}

	DBAR dBAR;

	try
	{   // Try each of them:

		dBAR = load_sophisticated(argv[1]);
		//dBAR = load_simple(argv[1]);

	} catch(const StreamParseException& e)
	{
		std::cerr << e.what();
		return EXIT_FAILURE;
	}

	std::cout << "  ARCS   Conf. Frame450" << '\n';
	std::cout << "-----------------------" << '\n';

	// Save current cout settings
	const auto prev_cout_settings { std::cout.flags() };

	// Traverse the response data and print every parsed entity
	auto block_counter { 1 };
	for (const auto& block : dBAR)
	{
		std::cout << "Block: " << block_counter << "/"
			<< dBAR.size() << '\n';

		// Print the header of this block (which is the AccurateRip id)
		std::cout << "ID: " << block.id().url() << '\n';

		for (const auto& triplet : block)
		{
			std::cout
				<< std::hex << std::uppercase
				<< std::setw(8) << std::setfill('0')
					<< triplet.arcs()
				<< "  "
				<< std::dec << std::setw(2) << std::setfill(' ')
					<< triplet.confidence()
				<< "   "
				<< std::hex << std::uppercase
				<< std::setw(8) << std::setfill('0')
					<< triplet.frame450_arcs()
				<< '\n';
		}

		++block_counter;
	}

	std::cout.flags(prev_cout_settings); // Restore cout settings

	return EXIT_SUCCESS;
}


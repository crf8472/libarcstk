//
// Example for parsing the binary AccurateRip query response to plaintext.
//

#include <cstdint>   // for uint32_t etc.
#include <iomanip>   // for setw, setfill, hex
#include <iostream>  // for cerr, cout
#include <stdexcept> // for runtime_error
#include <string>    // for string

#ifndef __LIBARCSTK_PARSE_HPP__      // libarcstk: parse AccurateRip responses
#include <arcstk/parse.hpp>
#endif
#ifndef __LIBARCSTK_IDENTIFIER_HPP__ // libarcstk: calculate AccurateRip ids
#include <arcstk/identifier.hpp>
#endif
#ifndef __LIBARCSTK_LOGGING_HPP__    // libarcstk: log what you do
#include <arcstk/logging.hpp>
#endif


// ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !
// NOTE! THIS IS EXAMPLE CODE! IT IS INTENDED TO DEMONSTRATE HOW LIBARCSTK COULD
// BE USED. IT IS NOT INTENDED TO BE USED IN REAL LIFE PRODUCTION. IT IS IN NO
// WAY TESTED FOR PRODUCTION. TAKE THIS AS A STARTING POINT TO YOUR OWN
// SOLUTION, NOT AS A TOOL.
// ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! !


int main(int argc, char* argv[])
{
	// Use the default parser content handler that just returns the parsed
	// content as an object
	auto content_hdlr { std::make_unique<arcstk::DefaultContentHandler>() };
	// Of course you could just write a content handler that prints every parsed
	// entitity instead of constructing an object from it.

	arcstk::ARResponse response_data;
	content_hdlr->set_object(response_data);

	// Use the standard error handler that just throws an exception on invalid
	// input.
	auto error_hdlr { std::make_unique<arcstk::DefaultErrorHandler>() };

	// Pointer to the parser object, concrete parser type is not yet known.
	std::unique_ptr<arcstk::ARStreamParser> parser;

	if (argc == 2) // read from the file passed
	{
		const std::string infilename { argv[1] };

		// Create parser for files
		parser = std::make_unique<arcstk::ARFileParser>(infilename);
	}
	else if (argc == 1) // read from stdin
	{
		// Create parser for input piped via stdin
		parser = std::make_unique<arcstk::ARStdinParser>();
	}
	else
	{
		std::cout << "Arguments: " << argc << std::endl;
		std::cout << "Usage: arparse <response_file_name>" << std::endl;
		return EXIT_SUCCESS;
	}

	// Register parser handlers
	parser->set_content_handler(std::move(content_hdlr));
	parser->set_error_handler(std::move(error_hdlr));

	// Finally, run parser
	try {
		parser->parse();
	} catch (const arcstk::StreamReadException& e)
	{
		std::cerr << e.what();

		return EXIT_FAILURE;
	}
	// ... normally you would also catch other possible exceptions, we just
	// concentrate on libarcstk.

	std::cout << "  ARCS   Conf. Frame450" << std::endl;
	std::cout << "-----------------------" << std::endl;

	// Save current cout settings
	const auto prev_cout_settings { std::cout.flags() };

	// Traverse the response data and print every parsed entity
	auto block_counter { 1 };
	for (const auto& block : response_data)
	{
		std::cout << "Block: " << block_counter << "/"
			<< response_data.size() << std::endl;

		// Print the header of this block (which is the AccurateRip id)
		std::cout << "ID: " << block.id().to_string() << std::endl;

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
				<< std::endl;
		}

		++block_counter;
	}

	std::cout.flags(prev_cout_settings); // Restore cout settings

	return EXIT_SUCCESS;
}


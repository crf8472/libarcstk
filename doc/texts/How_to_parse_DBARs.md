# How to parse AccurateRip DBAR responses                   {#howto_parse_dbars}


## Have a look at the code example

Locate the example ``dbarparse`` in the top-level folder ``examples``. You may
find a compileable code example for parsing dBAR-files acquired from
AccurateRip.


## Parsing the dBAR format

### Prerequisites

Consult the module @ref dbar for the part of the API you want to use.

Insert

@code{.cpp}
	#include <arcstk/dbar.hpp>
	#include <arcstk/identifier.hpp>
@endcode

in the file where you intend to place your code.


### Simple way to load the dBAR file

The simplest way to load the file is to let libarcstk do it for you:

@code{.cpp}

	DBAR dBAR;

	try
	{
		dBAR = arcstk::load_file(filename);

	} catch(const StreamParseException& e)
	{
		std::cerr << e.what();
		// ... do some error handling, skip dBAR
	}
@endcode


### Sophisticated way to load the dBAR file


@code{.cpp}
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

		const auto total_bytes { arcstk::parse_stream(stream, &builder, nullptr) };
		std::cout << "Read " << total_bytes << " bytes" << '\n';
		stream.close();

	} catch (const StreamParseException& e)
	{
		stream.close();
		throw;
	}
	// ... normally you would also catch other possible exceptions, we just
	// concentrate on libarcstk.

	// you may now return builder.result();
@endcode


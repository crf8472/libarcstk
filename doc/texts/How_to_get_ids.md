# How to calculate AccurateRip IDs


## Have a look at the code example

Locate the example ``albumid`` in the top-level folder ``examples``. You may
find a compileable code example for calculating the AccurateRip id for the audio
image of an album.


## Calculating the AccurateRip ID of an Album

### Prerequisites

Consult the module @ref id for the part of the API you want to use.

Insert

@code{.cpp}
	#include <arcstk/metadata.hpp>
	#include <arcstk/identifier.hpp>
@endcode

in the file where you intend to place your code.


### Get Leadout and Offsets

To calculate the AccurateRip ID of an album, we need the index (represented as
total number of LBA frames) of each track's first sample. Those indices are
called offsets. To get also the last track's end correct we furthermore need the
total number of input frames, which is identical with the leadout of the
original compact disc. The leadout is as well represented as total number of LBA
frames.

You will have to acquire these informations from your image or your disc on your
own. We presuppose you have this values and show how to proceed from there.

@code{.cpp}
	int32_t total_number_of_frames ...;
	std::vector<int32_t> offsets ...;
@endcode


### Get the AccurateRip ID

@code{.cpp}
	const auto toc_data { arcstk::toc::construct(total_number_of_frames, offsets) };

	// You can validate the ToCData if you want to.
	try
	{
		arcstk::toc::validate(toc_data);

	} catch (const std::exception& e)
	{
		std::cerr << e.what() << '\n';
		return EXIT_FAILURE;
	}

	// If you had skipped the validation before, you would now have to
	// try-catch for the case when the ToC is not complete.
	const auto id { arcstk::make_arid(arcstk::ToC(toc_data)) };
@endcode


### Access the AccurateRip ID

You can now print the actual id as well as the canonical filename or the
AccurateRip URL of the album.

@code{.cpp}
	using std::string;
	std::cout << "ID:          " << to_string(*id) << '\n';
	std::cout << "Filename:    " << id->filename() << '\n';
	std::cout << "Request-URL: " << id->url()      << '\n';
@endcode

Consult the API documentation for module @ref id.


#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for identifier_details.hpp.
 */

#ifndef LIBARCSTK_IDENTIFIER_HPP_
#include "identifier.hpp"
#endif
#ifndef LIBARCSTK_IDENTIFIER_DETAILS_HPP_
#include "identifier_details.hpp" // TO BE TESTED
#endif

#include <string>                 // for string
#include <vector>                 // for vector


TEST_CASE ( "make", "[arid]" )
{
	const auto id4 = arcstk::arid::make(
			// offsets
			{
				0, 29042, 53880, 58227, 84420, 94192, 119165, 123030,
				147500, 148267, 174602, 208125, 212705, 239890, 268705,
				272055, 291720, 319992
			},
			// leadout
			332075
	);

	SECTION ( "make constructs ARIds" )
	{
		CHECK ( ! id4.empty() );
	}
}

// TODO print()


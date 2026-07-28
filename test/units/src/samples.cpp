#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for samples.hpp.
 */

#ifndef LIBARCSTK_SAMPLESEQ_HPP_
#include "samples.hpp"          // TO BE TESTED
#endif


TEST_CASE ( "channel", "[channel]" )
{
	using arcstk::details::channel;

	SECTION ( "left()")
	{
		CHECK ( channel::left(false) == 0 );
		CHECK ( channel::left(true)  == 1 );
	}

	SECTION ( "right()")
	{
		CHECK ( channel::right(false) == 1 );
		CHECK ( channel::right(true)  == 0 );
	}
}


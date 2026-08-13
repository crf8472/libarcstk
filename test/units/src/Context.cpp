#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Context.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif


TEST_CASE ( "Context", "[context] [calc]" )
{
	using arcstk::Context;

	SECTION ( "OR-ing Context works as expected" )
	{
		CHECK ((Context::FIRST_TRACK | Context::TRACK) == Context::FIRST_TRACK);
		CHECK ((Context::LAST_TRACK  | Context::TRACK) == Context::LAST_TRACK);
		CHECK ((Context::ALBUM       | Context::TRACK) == Context::ALBUM);

		CHECK ((Context::FIRST_TRACK | Context::LAST_TRACK)  == Context::ALBUM);
		CHECK ((Context::FIRST_TRACK | Context::ALBUM)       == Context::ALBUM);

		CHECK ((Context::LAST_TRACK  | Context::FIRST_TRACK) == Context::ALBUM);
		CHECK ((Context::LAST_TRACK  | Context::ALBUM)       == Context::ALBUM);

		CHECK ((Context::ALBUM       | Context::LAST_TRACK)  == Context::ALBUM);
		CHECK ((Context::ALBUM       | Context::FIRST_TRACK) == Context::ALBUM);
	}

	SECTION ( "any() is correct" )
	{
		CHECK ( not any(Context::TRACK) );

		CHECK ( any(Context::FIRST_TRACK) );
		CHECK ( any(Context::LAST_TRACK) );
		CHECK ( any(Context::ALBUM) );

		CHECK ( any(Context::FIRST_TRACK | Context::LAST_TRACK) );
	}
}


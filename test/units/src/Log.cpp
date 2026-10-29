#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Log.
 */

#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_LOGLEVEL_HPP_
#include "loglevel.hpp"         // for LOGLEVEL
#endif


TEST_CASE ( "Log", "[log] [logging]" )
{
	using arcstk::Log;
	using arcstk::Logger;
	using arcstk::LOGLEVEL;

	auto logger = Logger {};

	SECTION ("Parametized construction is correct")
	{
		auto instance = Log { logger, LOGLEVEL::ERROR };

		CHECK ( instance.level() == LOGLEVEL::ERROR );
	}
}


TEST_CASE ( "Log functions", "[log] [logging]" )
{
	using arcstk::Log;
	using arcstk::Logger;
	using arcstk::LOGLEVEL;

	auto logger = Logger {};

	SECTION ("get() is correct")
	{
		auto instance = Log { logger, LOGLEVEL::ERROR };

		CHECK_NOTHROW ( instance.get() << "This is quite an ERROr -" );
	}
}


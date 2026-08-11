#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Logging.
 */

#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_LOGGING_HPP_
#include "logging_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "Logging", "[logging] [logging] [logging]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::Logging;

	auto instance = Logging {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const Logging copied { Copy<Logging>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const Logging moved { Move<Logging>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = Logging {};
		Copy<Logging>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = Logging {};
		Move<Logging>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "Logging default constructed instance",
		"[logging] [logging] [logging]" )
{
	const auto defaulted = arcstk::Logging{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "Logging property", "[logging] [logging] [logging]" )
{
	using arcstk::Logging;

	auto instance = Logging {};


	SECTION ("Equality operator == is correct")
	{
		FAIL ( "Equality operator test is missing" );
	}

	SECTION ("Stream-in operator << is correct")
	{
		FAIL ( "Stream-in operator << test is missing" );
	}

	SECTION ("operator bool() is correct")
	{
		FAIL ( "operator bool() test is missing" );
	}

	SECTION ("swap() is correct")
	{
		FAIL ( "swap() test is missing" );
	}

	SECTION ("to_string() is correct")
	{
		FAIL ( "to_string() test is missing" );
	}

	SECTION ("clone() is correct")
	{
		FAIL ( "clone() test is missing" );
	}

	SECTION ("size() is correct")
	{
		FAIL ( "size() test is missing" );
	}

	SECTION ("empty() is correct")
	{
		FAIL ( "empty() test is missing" );
	}
}


TEST_CASE ( "Logging functions", "[logging] [logging] [logging]" )
{
	using arcstk::Logging;

	auto instance = Logging {};

	SECTION ("Logging() is correct")
	{
		FAIL ("Logging() test is missing");
	}

	SECTION ("on_logger_do() is correct")
	{
		FAIL ("on_logger_do() test is missing");
	}

	SECTION ("operator=() is correct")
	{
		FAIL ("operator=() test is missing");
	}

	SECTION ("~Logging() is correct")
	{
		FAIL ("~Logging() test is missing");
	}

	SECTION ("logger() is correct")
	{
		FAIL ("logger() test is missing");
	}

	SECTION ("level() is correct")
	{
		FAIL ("level() test is missing");
	}

	SECTION ("set_level() is correct")
	{
		FAIL ("set_level() test is missing");
	}

	SECTION ("has_level() is correct")
	{
		FAIL ("has_level() test is missing");
	}

	SECTION ("set_timestamps() is correct")
	{
		FAIL ("set_timestamps() test is missing");
	}

	SECTION ("has_timestamps() is correct")
	{
		FAIL ("has_timestamps() test is missing");
	}

	SECTION ("add_appender() is correct")
	{
		FAIL ("add_appender() test is missing");
	}

	SECTION ("remove_appender() is correct")
	{
		FAIL ("remove_appender() test is missing");
	}

	SECTION ("instance() is correct")
	{
		FAIL ("instance() test is missing");
	}

}


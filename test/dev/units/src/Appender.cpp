#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Appender.
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


TEST_CASE ( "Appender", "[appender] [logging] [logging]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::Appender;

	auto instance = Appender {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const Appender copied { Copy<Appender>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const Appender moved { Move<Appender>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = Appender {};
		Copy<Appender>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = Appender {};
		Move<Appender>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "Appender default constructed instance",
		"[appender] [logging] [logging]" )
{
	const auto defaulted = arcstk::Appender{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "Appender property", "[appender] [logging] [logging]" )
{
	using arcstk::Appender;

	auto instance = Appender {};


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


TEST_CASE ( "Appender functions", "[appender] [logging] [logging]" )
{
	using arcstk::Appender;

	auto instance = Appender {};

	SECTION ("Appender() is correct")
	{
		FAIL ("Appender() test is missing");
	}

	SECTION ("operator=() is correct")
	{
		FAIL ("operator=() test is missing");
	}

	SECTION ("~Appender() is correct")
	{
		FAIL ("~Appender() test is missing");
	}

	SECTION ("append() is correct")
	{
		FAIL ("append() test is missing");
	}

	SECTION ("name() is correct")
	{
		FAIL ("name() test is missing");
	}

}


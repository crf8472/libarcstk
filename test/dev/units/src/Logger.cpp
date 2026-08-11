#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Logger.
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


TEST_CASE ( "Logger", "[logger] [logging] [logging]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::Logger;

	auto instance = Logger {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const Logger copied { Copy<Logger>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const Logger moved { Move<Logger>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = Logger {};
		Copy<Logger>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = Logger {};
		Move<Logger>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "Logger default constructed instance",
		"[logger] [logging] [logging]" )
{
	const auto defaulted = arcstk::Logger{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "Logger property", "[logger] [logging] [logging]" )
{
	using arcstk::Logger;

	auto instance = Logger {};


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


TEST_CASE ( "Logger functions", "[logger] [logging] [logging]" )
{
	using arcstk::Logger;

	auto instance = Logger {};

	SECTION ("Logger() is correct")
	{
		FAIL ("Logger() test is missing");
	}

	SECTION ("operator=() is correct")
	{
		FAIL ("operator=() test is missing");
	}

	SECTION ("~Logger() is correct")
	{
		FAIL ("~Logger() test is missing");
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

	SECTION ("log() is correct")
	{
		FAIL ("log() test is missing");
	}

}


#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for StreamFlagsGuard.
 */

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "StreamFlagsGuard", "[streamflagsguard] [calc] [checksum]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::StreamFlagsGuard;

	auto instance = StreamFlagsGuard {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const StreamFlagsGuard copied { Copy<StreamFlagsGuard>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const StreamFlagsGuard moved { Move<StreamFlagsGuard>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = StreamFlagsGuard {};
		Copy<StreamFlagsGuard>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = StreamFlagsGuard {};
		Move<StreamFlagsGuard>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "StreamFlagsGuard default constructed instance",
		"[streamflagsguard] [calc] [checksum]" )
{
	const auto defaulted = arcstk::StreamFlagsGuard{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "StreamFlagsGuard property", "[streamflagsguard] [calc] [checksum]" )
{
	using arcstk::StreamFlagsGuard;

	auto instance = StreamFlagsGuard {};


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


TEST_CASE ( "StreamFlagsGuard functions", "[streamflagsguard] [calc] [checksum]" )
{
	using arcstk::StreamFlagsGuard;

	auto instance = StreamFlagsGuard {};

	SECTION ("StreamFlagsGuard() is correct")
	{
		FAIL ("StreamFlagsGuard() test is missing");
	}

	SECTION ("~StreamFlagsGuard() is correct")
	{
		FAIL ("~StreamFlagsGuard() test is missing");
	}

	SECTION ("operator=() is correct")
	{
		FAIL ("operator=() test is missing");
	}

}


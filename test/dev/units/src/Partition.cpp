#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Partition.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "Partition", "[partition] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::Partition;

	auto instance = Partition {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const Partition copied { Copy<Partition>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const Partition moved { Move<Partition>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = Partition {};
		Copy<Partition>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = Partition {};
		Move<Partition>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "Partition default constructed instance",
		"[partition] [calc] [calculate]" )
{
	const auto defaulted = arcstk::Partition{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "Partition property", "[partition] [calc] [calculate]" )
{
	using arcstk::Partition;

	auto instance = Partition {};


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


TEST_CASE ( "Partition functions", "[partition] [calc] [calculate]" )
{
	using arcstk::Partition;

	auto instance = Partition {};

	SECTION ("Partition() is correct")
	{
		FAIL ("Partition() test is missing");
	}

	SECTION ("begin_offset() is correct")
	{
		FAIL ("begin_offset() test is missing");
	}

	SECTION ("end_offset() is correct")
	{
		FAIL ("end_offset() test is missing");
	}

	SECTION ("starts_track() is correct")
	{
		FAIL ("starts_track() test is missing");
	}

	SECTION ("ends_track() is correct")
	{
		FAIL ("ends_track() test is missing");
	}

	SECTION ("track() is correct")
	{
		FAIL ("track() test is missing");
	}

	SECTION ("size() is correct")
	{
		FAIL ("size() test is missing");
	}

}


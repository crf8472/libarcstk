#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for TrackPartitioner.
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


TEST_CASE ( "TrackPartitioner", "[trackpartitioner] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::TrackPartitioner;

	auto instance = TrackPartitioner {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const TrackPartitioner copied { Copy<TrackPartitioner>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const TrackPartitioner moved { Move<TrackPartitioner>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = TrackPartitioner {};
		Copy<TrackPartitioner>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = TrackPartitioner {};
		Move<TrackPartitioner>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "TrackPartitioner default constructed instance",
		"[trackpartitioner] [calc] [calculate]" )
{
	const auto defaulted = arcstk::TrackPartitioner{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "TrackPartitioner property", "[trackpartitioner] [calc] [calculate]" )
{
	using arcstk::TrackPartitioner;

	auto instance = TrackPartitioner {};


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


TEST_CASE ( "TrackPartitioner functions", "[trackpartitioner] [calc] [calculate]" )
{
	using arcstk::TrackPartitioner;

	auto instance = TrackPartitioner {};

	SECTION ("do_create_partitioning() is correct")
	{
		FAIL ("do_create_partitioning() test is missing");
	}

	SECTION ("do_clone() is correct")
	{
		FAIL ("do_clone() test is missing");
	}

	SECTION ("TrackPartitioner() is correct")
	{
		FAIL ("TrackPartitioner() test is missing");
	}

}


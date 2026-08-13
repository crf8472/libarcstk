#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Partition.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "Partition", "[partition] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::details::Partition;

	auto instance = Partition { 33 * 588, 5225 * 588 - 1, true, true, 1 };


	SECTION ("Parametized construction is correct")
	{
		CHECK ( instance.begin_offset() == 33 * 588 );
		CHECK ( instance.end_offset()   == 5225 * 588 - 1 );
		CHECK ( instance.starts_track() );
		CHECK ( instance.ends_track() );
		CHECK ( instance.track() == 1 );
	}

	SECTION ("Copy construction is correct")
	{
		const Partition copied { Copy<Partition>::construct(instance) };

		// --

		CHECK ( copied.begin_offset() == 33 * 588 );
		CHECK ( copied.end_offset()   == 5225 * 588 - 1 );
		CHECK ( copied.starts_track() );
		CHECK ( copied.ends_track() );
		CHECK ( copied.track() == 1 );
	}

	SECTION ("Move construction is correct")
	{
		const Partition moved { Move<Partition>::construct(instance) };

		// --

		CHECK ( moved.begin_offset() == 33 * 588 );
		CHECK ( moved.end_offset()   == 5225 * 588 - 1 );
		CHECK ( moved.starts_track() );
		CHECK ( moved.ends_track() );
		CHECK ( moved.track() == 1 );
	}

	SECTION ("Copy assignment is correct")
	{
		// other values than instance
		auto copied = Partition { 86 * 588, 6111 * 588 - 1, false, false, 6 };
		Copy<Partition>::assign(copied, instance);

		// --

		CHECK ( copied.begin_offset() == 33 * 588 );
		CHECK ( copied.end_offset()   == 5225 * 588 - 1 );
		CHECK ( copied.starts_track() );
		CHECK ( copied.ends_track() );
		CHECK ( copied.track() == 1 );
	}

	SECTION ("Move assignment is correct")
	{
		// other values than instance
		auto moved = Partition { 86 * 588, 6111 * 588 - 1, false, false, 6 };
		Move<Partition>::assign(moved, instance);

		// --

		CHECK ( moved.begin_offset() == 33 * 588 );
		CHECK ( moved.end_offset()   == 5225 * 588 - 1 );
		CHECK ( moved.starts_track() );
		CHECK ( moved.ends_track() );
		CHECK ( moved.track() == 1 );
	}
}


// TEST_CASE ( "Partition property", "[partition] [calc] [calculate]" )
// {
// 	using arcstk::details::Partition;
//
// 	auto instance = Partition {};
//
//
// 	SECTION ("Equality operator == is correct")
// 	{
// 		FAIL ( "Equality operator test is missing" );
// 	}
//
// 	SECTION ("Stream-in operator << is correct")
// 	{
// 		FAIL ( "Stream-in operator << test is missing" );
// 	}
//
// 	SECTION ("operator bool() is correct")
// 	{
// 		FAIL ( "operator bool() test is missing" );
// 	}
//
// 	SECTION ("swap() is correct")
// 	{
// 		FAIL ( "swap() test is missing" );
// 	}
//
// 	SECTION ("to_string() is correct")
// 	{
// 		FAIL ( "to_string() test is missing" );
// 	}
//
// 	SECTION ("size() is correct")
// 	{
// 		FAIL ( "size() test is missing" );
// 	}
//
// 	SECTION ("empty() is correct")
// 	{
// 		FAIL ( "empty() test is missing" );
// 	}
// }


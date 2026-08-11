#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Result.
 */

#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_VERIFY_HPP_
#include "verify_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "Result", "[result] [verify] [verify]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::Result;

	auto instance = Result {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const Result copied { Copy<Result>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const Result moved { Move<Result>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = Result {};
		Copy<Result>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = Result {};
		Move<Result>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "Result default constructed instance",
		"[result] [verify] [verify]" )
{
	const auto defaulted = arcstk::Result{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "Result property", "[result] [verify] [verify]" )
{
	using arcstk::Result;

	auto instance = Result {};


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


TEST_CASE ( "Result functions", "[result] [verify] [verify]" )
{
	using arcstk::Result;

	auto instance = Result {};

	SECTION ("~VerificationResult() is correct")
	{
		FAIL ("~VerificationResult() test is missing");
	}

	SECTION ("all_tracks_verified() is correct")
	{
		FAIL ("all_tracks_verified() test is missing");
	}

	SECTION ("total_unverified_tracks() is correct")
	{
		FAIL ("total_unverified_tracks() test is missing");
	}

	SECTION ("is_verified() is correct")
	{
		FAIL ("is_verified() test is missing");
	}

	SECTION ("verify_track() is correct")
	{
		FAIL ("verify_track() test is missing");
	}

	SECTION ("track() is correct")
	{
		FAIL ("track() test is missing");
	}

	SECTION ("verify_id() is correct")
	{
		FAIL ("verify_id() test is missing");
	}

	SECTION ("id() is correct")
	{
		FAIL ("id() test is missing");
	}

	SECTION ("difference() is correct")
	{
		FAIL ("difference() test is missing");
	}

	SECTION ("total_blocks() is correct")
	{
		FAIL ("total_blocks() test is missing");
	}

	SECTION ("tracks_per_block() is correct")
	{
		FAIL ("tracks_per_block() test is missing");
	}

	SECTION ("size() is correct")
	{
		FAIL ("size() test is missing");
	}

	SECTION ("best_block() is correct")
	{
		FAIL ("best_block() test is missing");
	}

	SECTION ("best_block_difference() is correct")
	{
		FAIL ("best_block_difference() test is missing");
	}

	SECTION ("strict() is correct")
	{
		FAIL ("strict() test is missing");
	}

	SECTION ("clone() is correct")
	{
		FAIL ("clone() test is missing");
	}

	SECTION ("do_total_unverified_tracks() is correct")
	{
		FAIL ("do_total_unverified_tracks() test is missing");
	}

	SECTION ("do_is_verified() is correct")
	{
		FAIL ("do_is_verified() test is missing");
	}

	SECTION ("do_verify_track() is correct")
	{
		FAIL ("do_verify_track() test is missing");
	}

	SECTION ("do_track() is correct")
	{
		FAIL ("do_track() test is missing");
	}

	SECTION ("do_verify_id() is correct")
	{
		FAIL ("do_verify_id() test is missing");
	}

	SECTION ("do_id() is correct")
	{
		FAIL ("do_id() test is missing");
	}

	SECTION ("do_difference() is correct")
	{
		FAIL ("do_difference() test is missing");
	}

	SECTION ("do_total_blocks() is correct")
	{
		FAIL ("do_total_blocks() test is missing");
	}

	SECTION ("do_tracks_per_block() is correct")
	{
		FAIL ("do_tracks_per_block() test is missing");
	}

	SECTION ("do_size() is correct")
	{
		FAIL ("do_size() test is missing");
	}

	SECTION ("do_best_block() is correct")
	{
		FAIL ("do_best_block() test is missing");
	}

	SECTION ("do_best_block_difference() is correct")
	{
		FAIL ("do_best_block_difference() test is missing");
	}

	SECTION ("do_strict() is correct")
	{
		FAIL ("do_strict() test is missing");
	}

	SECTION ("do_clone() is correct")
	{
		FAIL ("do_clone() test is missing");
	}

}


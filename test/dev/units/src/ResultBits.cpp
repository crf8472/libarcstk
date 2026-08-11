#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for ResultBits.
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


TEST_CASE ( "ResultBits", "[resultbits] [verify] [verify]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::ResultBits;

	auto instance = ResultBits {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const ResultBits copied { Copy<ResultBits>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const ResultBits moved { Move<ResultBits>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = ResultBits {};
		Copy<ResultBits>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = ResultBits {};
		Move<ResultBits>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "ResultBits default constructed instance",
		"[resultbits] [verify] [verify]" )
{
	const auto defaulted = arcstk::ResultBits{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "ResultBits property", "[resultbits] [verify] [verify]" )
{
	using arcstk::ResultBits;

	auto instance = ResultBits {};


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


TEST_CASE ( "ResultBits functions", "[resultbits] [verify] [verify]" )
{
	using arcstk::ResultBits;

	auto instance = ResultBits {};

	SECTION ("ResultBits() is correct")
	{
		FAIL ("ResultBits() test is missing");
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

	SECTION ("set_track() is correct")
	{
		FAIL ("set_track() test is missing");
	}

	SECTION ("track() is correct")
	{
		FAIL ("track() test is missing");
	}

	SECTION ("set_id() is correct")
	{
		FAIL ("set_id() test is missing");
	}

	SECTION ("id() is correct")
	{
		FAIL ("id() test is missing");
	}

	SECTION ("total_tracks_set() is correct")
	{
		FAIL ("total_tracks_set() test is missing");
	}

	SECTION ("operator[]() is correct")
	{
		FAIL ("operator[]() test is missing");
	}

	SECTION ("index() is correct")
	{
		FAIL ("index() test is missing");
	}

	SECTION ("block_offset() is correct")
	{
		FAIL ("block_offset() test is missing");
	}

	SECTION ("track_offset() is correct")
	{
		FAIL ("track_offset() test is missing");
	}

	SECTION ("set_flag() is correct")
	{
		FAIL ("set_flag() test is missing");
	}

	SECTION ("flags_per_block() is correct")
	{
		FAIL ("flags_per_block() test is missing");
	}

	SECTION ("check_maximums() is correct")
	{
		FAIL ("check_maximums() test is missing");
	}

	SECTION ("bounds_check_block() is correct")
	{
		FAIL ("bounds_check_block() test is missing");
	}

	SECTION ("bounds_check_track() is correct")
	{
		FAIL ("bounds_check_track() test is missing");
	}

	SECTION ("create_flag_store() is correct")
	{
		FAIL ("create_flag_store() test is missing");
	}

}


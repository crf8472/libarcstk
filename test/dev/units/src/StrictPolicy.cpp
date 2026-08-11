#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for StrictPolicy.
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


TEST_CASE ( "StrictPolicy", "[strictpolicy] [verify] [verify]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::StrictPolicy;

	auto instance = StrictPolicy {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const StrictPolicy copied { Copy<StrictPolicy>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const StrictPolicy moved { Move<StrictPolicy>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = StrictPolicy {};
		Copy<StrictPolicy>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = StrictPolicy {};
		Move<StrictPolicy>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "StrictPolicy default constructed instance",
		"[strictpolicy] [verify] [verify]" )
{
	const auto defaulted = arcstk::StrictPolicy{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "StrictPolicy property", "[strictpolicy] [verify] [verify]" )
{
	using arcstk::StrictPolicy;

	auto instance = StrictPolicy {};


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


TEST_CASE ( "StrictPolicy functions", "[strictpolicy] [verify] [verify]" )
{
	using arcstk::StrictPolicy;

	auto instance = StrictPolicy {};

	SECTION ("do_is_verified() is correct")
	{
		FAIL ("do_is_verified() test is missing");
	}

	SECTION ("do_total_unverified_tracks() is correct")
	{
		FAIL ("do_total_unverified_tracks() test is missing");
	}

	SECTION ("do_is_strict() is correct")
	{
		FAIL ("do_is_strict() test is missing");
	}

	SECTION ("do_clone() is correct")
	{
		FAIL ("do_clone() test is missing");
	}

}


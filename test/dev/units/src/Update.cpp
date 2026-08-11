#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Update.
 */

#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_ACCURATERIP_HPP_
#include "accuraterip_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "Update", "[update] [calc] [accuraterip]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::Update;

	auto instance = Update {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const Update copied { Copy<Update>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const Update moved { Move<Update>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = Update {};
		Copy<Update>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = Update {};
		Move<Update>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "Update default constructed instance",
		"[update] [calc] [accuraterip]" )
{
	const auto defaulted = arcstk::Update{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "Update property", "[update] [calc] [accuraterip]" )
{
	using arcstk::Update;

	auto instance = Update {};


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


TEST_CASE ( "Update functions", "[update] [calc] [accuraterip]" )
{
	using arcstk::Update;

	auto instance = Update {};

}


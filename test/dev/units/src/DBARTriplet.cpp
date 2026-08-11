#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for DBARTriplet.
 */

#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_DBAR_HPP_
#include "dbar_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "DBARTriplet", "[dbartriplet] [dbar] [dbar]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::DBARTriplet;

	auto instance = DBARTriplet {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const DBARTriplet copied { Copy<DBARTriplet>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const DBARTriplet moved { Move<DBARTriplet>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = DBARTriplet {};
		Copy<DBARTriplet>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = DBARTriplet {};
		Move<DBARTriplet>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "DBARTriplet default constructed instance",
		"[dbartriplet] [dbar] [dbar]" )
{
	const auto defaulted = arcstk::DBARTriplet{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "DBARTriplet property", "[dbartriplet] [dbar] [dbar]" )
{
	using arcstk::DBARTriplet;

	auto instance = DBARTriplet {};


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


TEST_CASE ( "DBARTriplet functions", "[dbartriplet] [dbar] [dbar]" )
{
	using arcstk::DBARTriplet;

	auto instance = DBARTriplet {};

	SECTION ("DBARTriplet() is correct")
	{
		FAIL ("DBARTriplet() test is missing");
	}

	SECTION ("arcs() is correct")
	{
		FAIL ("arcs() test is missing");
	}

	SECTION ("confidence() is correct")
	{
		FAIL ("confidence() test is missing");
	}

	SECTION ("frame450_arcs() is correct")
	{
		FAIL ("frame450_arcs() test is missing");
	}

	SECTION ("swap() is correct")
	{
		FAIL ("swap() test is missing");
	}

	SECTION ("equals() is correct")
	{
		FAIL ("equals() test is missing");
	}

	SECTION ("to_string() is correct")
	{
		FAIL ("to_string() test is missing");
	}

}


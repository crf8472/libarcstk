#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for DBARBlockHeader.
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


TEST_CASE ( "DBARBlockHeader", "[dbarblockheader] [dbar] [dbar]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::DBARBlockHeader;

	auto instance = DBARBlockHeader {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const DBARBlockHeader copied { Copy<DBARBlockHeader>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const DBARBlockHeader moved { Move<DBARBlockHeader>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = DBARBlockHeader {};
		Copy<DBARBlockHeader>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = DBARBlockHeader {};
		Move<DBARBlockHeader>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "DBARBlockHeader default constructed instance",
		"[dbarblockheader] [dbar] [dbar]" )
{
	const auto defaulted = arcstk::DBARBlockHeader{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "DBARBlockHeader property", "[dbarblockheader] [dbar] [dbar]" )
{
	using arcstk::DBARBlockHeader;

	auto instance = DBARBlockHeader {};


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


TEST_CASE ( "DBARBlockHeader functions", "[dbarblockheader] [dbar] [dbar]" )
{
	using arcstk::DBARBlockHeader;

	auto instance = DBARBlockHeader {};

	SECTION ("DBARBlockHeader() is correct")
	{
		FAIL ("DBARBlockHeader() test is missing");
	}

	SECTION ("total_tracks() is correct")
	{
		FAIL ("total_tracks() test is missing");
	}

	SECTION ("id1() is correct")
	{
		FAIL ("id1() test is missing");
	}

	SECTION ("id2() is correct")
	{
		FAIL ("id2() test is missing");
	}

	SECTION ("cddb_id() is correct")
	{
		FAIL ("cddb_id() test is missing");
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


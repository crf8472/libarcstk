#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for DBARSource.
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


TEST_CASE ( "DBARSource", "[dbarsource] [verify] [verify]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::DBARSource;

	auto instance = DBARSource {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const DBARSource copied { Copy<DBARSource>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const DBARSource moved { Move<DBARSource>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = DBARSource {};
		Copy<DBARSource>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = DBARSource {};
		Move<DBARSource>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "DBARSource default constructed instance",
		"[dbarsource] [verify] [verify]" )
{
	const auto defaulted = arcstk::DBARSource{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "DBARSource property", "[dbarsource] [verify] [verify]" )
{
	using arcstk::DBARSource;

	auto instance = DBARSource {};


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


TEST_CASE ( "DBARSource functions", "[dbarsource] [verify] [verify]" )
{
	using arcstk::DBARSource;

	auto instance = DBARSource {};

	SECTION ("do_id() is correct")
	{
		FAIL ("do_id() test is missing");
	}

	SECTION ("do_checksum() is correct")
	{
		FAIL ("do_checksum() test is missing");
	}

	SECTION ("do_arcs_value() is correct")
	{
		FAIL ("do_arcs_value() test is missing");
	}

	SECTION ("do_confidence() is correct")
	{
		FAIL ("do_confidence() test is missing");
	}

	SECTION ("do_frame450_arcs_value() is correct")
	{
		FAIL ("do_frame450_arcs_value() test is missing");
	}

	SECTION ("do_size() is correct")
	{
		FAIL ("do_size() test is missing");
	}

	SECTION ("do_clone() is correct")
	{
		FAIL ("do_clone() test is missing");
	}

	SECTION ("DBARSource() is correct")
	{
		FAIL ("DBARSource() test is missing");
	}

	SECTION ("~DBARSource() is correct")
	{
		FAIL ("~DBARSource() test is missing");
	}

	SECTION ("operator=() is correct")
	{
		FAIL ("operator=() test is missing");
	}

	SECTION ("dbar() is correct")
	{
		FAIL ("dbar() test is missing");
	}

}


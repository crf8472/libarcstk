#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for ACCURATERIP.
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


TEST_CASE ( "ACCURATERIP", "[accuraterip] [calc] [accuraterip]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::ACCURATERIP;

	auto instance = ACCURATERIP {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const ACCURATERIP copied { Copy<ACCURATERIP>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const ACCURATERIP moved { Move<ACCURATERIP>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = ACCURATERIP {};
		Copy<ACCURATERIP>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = ACCURATERIP {};
		Move<ACCURATERIP>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "ACCURATERIP default constructed instance",
		"[accuraterip] [calc] [accuraterip]" )
{
	const auto defaulted = arcstk::ACCURATERIP{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "ACCURATERIP property", "[accuraterip] [calc] [accuraterip]" )
{
	using arcstk::ACCURATERIP;

	auto instance = ACCURATERIP {};


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


TEST_CASE ( "ACCURATERIP functions", "[accuraterip] [calc] [accuraterip]" )
{
	using arcstk::ACCURATERIP;

	auto instance = ACCURATERIP {};

	SECTION ("request_url_prefix() is correct")
	{
		FAIL ("request_url_prefix() test is missing");
	}

	SECTION ("default_request_url_prefix() is correct")
	{
		FAIL ("default_request_url_prefix() test is missing");
	}

	SECTION ("set_request_url_prefix() is correct")
	{
		FAIL ("set_request_url_prefix() test is missing");
	}

	SECTION ("reset_request_url_prefix() is correct")
	{
		FAIL ("reset_request_url_prefix() test is missing");
	}

	SECTION ("default_arcs_format() is correct")
	{
		FAIL ("default_arcs_format() test is missing");
	}

}


#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for Checksum.
 */

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"          // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "Checksum", "[checksum] [calc] [checksum]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::Checksum;

	auto instance = Checksum { 0xB89992E5 };

	REQUIRE ( instance.value() == 0xB89992E5 );


	SECTION ("Parametized construction is correct")
	{
		auto checksum01 = Checksum { 0xB89992E5 };
		CHECK ( checksum01.value() == 0xB89992E5 );

		auto checksum02 = Checksum { 0x98B10E0F };
		CHECK ( checksum02.value() == 0x98B10E0F );

		CHECK ( checksum01 != checksum02 );
	}

	SECTION ("Copy construction is correct")
	{
		const Checksum copied { Copy<Checksum>::construct(instance) };

		// --

		CHECK ( copied.value() == 0xB89992E5 );
	}

	SECTION ("Move construction is correct")
	{
		const Checksum moved { Move<Checksum>::construct(instance) };

		// --

		CHECK ( moved.value() == 0xB89992E5 );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = Checksum { 0 };
		REQUIRE ( copied.value() == 0 );

		Copy<Checksum>::assign(copied, instance);

		// --

		CHECK ( copied.value() == 0xB89992E5 );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = Checksum { 0 };
		REQUIRE ( moved.value() == 0 );

		Move<Checksum>::assign(moved, instance);

		// --

		CHECK ( moved.value() == 0xB89992E5 );
	}
}


TEST_CASE ( "Checksum default constructed instance",
		"[checksum] [calc] [checksum]" )
{
	const auto instance = arcstk::Checksum{};

	SECTION ( "is zero()")
	{
		CHECK ( instance.zero() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !instance );
	}
}

// TODO Implement Checksum property tests
	/*
TEST_CASE ( "Checksum property", "[checksum] [calc] [checksum]" )
{
	using arcstk::Checksum;

	auto defaulted = Checksum {};


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
}
	*/


	/*
TEST_CASE ( "Checksum functions", "[checksum] [calc] [checksum]" )
{
	using arcstk::Checksum;

	auto instance = Checksum {};

	SECTION ("value is correct")
	{
		FAIL ("value test is missing");
	}

	SECTION ("zero is correct")
	{
		FAIL ("zero test is missing");
	}

	SECTION ("operator bool is correct")
	{
		FAIL ("operator bool test is missing");
	}

	SECTION ("operator value_type is correct")
	{
		FAIL ("operator value_type test is missing");
	}

	SECTION ("swap is correct")
	{
		FAIL ("swap test is missing");
	}

	SECTION ("equals is correct")
	{
		FAIL ("equals test is missing");
	}

	SECTION ("equals_value is correct")
	{
		FAIL ("equals_value test is missing");
	}

	SECTION ("to_string is correct")
	{
		FAIL ("to_string test is missing");
	}

	SECTION ("checksum is correct")
	{
		FAIL ("checksum test is missing");
	}
}
	*/


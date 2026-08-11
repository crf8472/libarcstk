#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for AlgorithmTypes.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "AlgorithmTypes", "[algorithmtypes] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::AlgorithmTypes;

	auto instance = AlgorithmTypes {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const AlgorithmTypes copied { Copy<AlgorithmTypes>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const AlgorithmTypes moved { Move<AlgorithmTypes>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = AlgorithmTypes {};
		Copy<AlgorithmTypes>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = AlgorithmTypes {};
		Move<AlgorithmTypes>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "AlgorithmTypes default constructed instance",
		"[algorithmtypes] [calc] [calculate]" )
{
	const auto defaulted = arcstk::AlgorithmTypes{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "AlgorithmTypes property", "[algorithmtypes] [calc] [calculate]" )
{
	using arcstk::AlgorithmTypes;

	auto instance = AlgorithmTypes {};


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


TEST_CASE ( "AlgorithmTypes functions", "[algorithmtypes] [calc] [calculate]" )
{
	using arcstk::AlgorithmTypes;

	auto instance = AlgorithmTypes {};

	SECTION ("configure() is correct")
	{
		FAIL ("configure() test is missing");
	}

	SECTION ("typed_calculationset_for() is correct")
	{
		FAIL ("typed_calculationset_for() test is missing");
	}

	SECTION ("calculationset_for() is correct")
	{
		FAIL ("calculationset_for() test is missing");
	}

}


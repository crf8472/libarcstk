#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for factor_impl.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"          // TO BE TESTED
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata_details.hpp"  // for
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "factor_impl", "[factor_impl] [metadata] [metadata]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::factor_impl;

	auto instance = factor_impl {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const factor_impl copied { Copy<factor_impl>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const factor_impl moved { Move<factor_impl>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = factor_impl {};
		Copy<factor_impl>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = factor_impl {};
		Move<factor_impl>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "factor_impl default constructed instance",
		"[factor_impl] [metadata] [metadata]" )
{
	const auto defaulted = arcstk::factor_impl{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "factor_impl property", "[factor_impl] [metadata] [metadata]" )
{
	using arcstk::factor_impl;

	auto instance = factor_impl {};


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


TEST_CASE ( "factor_impl functions", "[factor_impl] [metadata] [metadata]" )
{
	using arcstk::factor_impl;

	auto instance = factor_impl {};

}


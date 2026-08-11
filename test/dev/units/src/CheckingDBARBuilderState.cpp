#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for CheckingDBARBuilderState.
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


TEST_CASE ( "CheckingDBARBuilderState", "[checkingdbarbuilderstate] [dbar] [dbar]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::CheckingDBARBuilderState;

	auto instance = CheckingDBARBuilderState {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const CheckingDBARBuilderState copied { Copy<CheckingDBARBuilderState>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const CheckingDBARBuilderState moved { Move<CheckingDBARBuilderState>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = CheckingDBARBuilderState {};
		Copy<CheckingDBARBuilderState>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = CheckingDBARBuilderState {};
		Move<CheckingDBARBuilderState>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "CheckingDBARBuilderState default constructed instance",
		"[checkingdbarbuilderstate] [dbar] [dbar]" )
{
	const auto defaulted = arcstk::CheckingDBARBuilderState{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "CheckingDBARBuilderState property", "[checkingdbarbuilderstate] [dbar] [dbar]" )
{
	using arcstk::CheckingDBARBuilderState;

	auto instance = CheckingDBARBuilderState {};


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


TEST_CASE ( "CheckingDBARBuilderState functions", "[checkingdbarbuilderstate] [dbar] [dbar]" )
{
	using arcstk::CheckingDBARBuilderState;

	auto instance = CheckingDBARBuilderState {};

	SECTION ("update_uniformity() is correct")
	{
		FAIL ("update_uniformity() test is missing");
	}

	SECTION ("update_validity() is correct")
	{
		FAIL ("update_validity() test is missing");
	}

	SECTION ("CheckingDBARBuilderState() is correct")
	{
		FAIL ("CheckingDBARBuilderState() test is missing");
	}

	SECTION ("header() is correct")
	{
		FAIL ("header() test is missing");
	}

	SECTION ("triplet() is correct")
	{
		FAIL ("triplet() test is missing");
	}

	SECTION ("end_block() is correct")
	{
		FAIL ("end_block() test is missing");
	}

	SECTION ("is_valid() is correct")
	{
		FAIL ("is_valid() test is missing");
	}

	SECTION ("is_uniform() is correct")
	{
		FAIL ("is_uniform() test is missing");
	}

	SECTION ("reset() is correct")
	{
		FAIL ("reset() test is missing");
	}

}


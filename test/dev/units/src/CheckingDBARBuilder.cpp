#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for CheckingDBARBuilder.
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


TEST_CASE ( "CheckingDBARBuilder", "[checkingdbarbuilder] [dbar] [dbar]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::CheckingDBARBuilder;

	auto instance = CheckingDBARBuilder {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const CheckingDBARBuilder copied { Copy<CheckingDBARBuilder>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const CheckingDBARBuilder moved { Move<CheckingDBARBuilder>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = CheckingDBARBuilder {};
		Copy<CheckingDBARBuilder>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = CheckingDBARBuilder {};
		Move<CheckingDBARBuilder>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "CheckingDBARBuilder default constructed instance",
		"[checkingdbarbuilder] [dbar] [dbar]" )
{
	const auto defaulted = arcstk::CheckingDBARBuilder{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "CheckingDBARBuilder property", "[checkingdbarbuilder] [dbar] [dbar]" )
{
	using arcstk::CheckingDBARBuilder;

	auto instance = CheckingDBARBuilder {};


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


TEST_CASE ( "CheckingDBARBuilder functions", "[checkingdbarbuilder] [dbar] [dbar]" )
{
	using arcstk::CheckingDBARBuilder;

	auto instance = CheckingDBARBuilder {};

	SECTION ("do_start_input() is correct")
	{
		FAIL ("do_start_input() test is missing");
	}

	SECTION ("do_start_block() is correct")
	{
		FAIL ("do_start_block() test is missing");
	}

	SECTION ("do_header() is correct")
	{
		FAIL ("do_header() test is missing");
	}

	SECTION ("do_start_triplets() is correct")
	{
		FAIL ("do_start_triplets() test is missing");
	}

	SECTION ("do_triplet() is correct")
	{
		FAIL ("do_triplet() test is missing");
	}

	SECTION ("do_end_triplets() is correct")
	{
		FAIL ("do_end_triplets() test is missing");
	}

	SECTION ("do_end_block() is correct")
	{
		FAIL ("do_end_block() test is missing");
	}

	SECTION ("do_end_input() is correct")
	{
		FAIL ("do_end_input() test is missing");
	}

	SECTION ("CheckingDBARBuilder() is correct")
	{
		FAIL ("CheckingDBARBuilder() test is missing");
	}

	SECTION ("result_is_valid() is correct")
	{
		FAIL ("result_is_valid() test is missing");
	}

	SECTION ("result_is_uniform() is correct")
	{
		FAIL ("result_is_uniform() test is missing");
	}

	SECTION ("result_is_regular() is correct")
	{
		FAIL ("result_is_regular() test is missing");
	}

	SECTION ("result() is correct")
	{
		FAIL ("result() test is missing");
	}

	SECTION ("reset() is correct")
	{
		FAIL ("reset() test is missing");
	}

}


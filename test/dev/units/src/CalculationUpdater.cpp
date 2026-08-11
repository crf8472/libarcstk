#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for CalculationUpdater.
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


TEST_CASE ( "CalculationUpdater", "[calculationupdater] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::CalculationUpdater;

	auto instance = CalculationUpdater {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const CalculationUpdater copied { Copy<CalculationUpdater>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const CalculationUpdater moved { Move<CalculationUpdater>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = CalculationUpdater {};
		Copy<CalculationUpdater>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = CalculationUpdater {};
		Move<CalculationUpdater>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "CalculationUpdater default constructed instance",
		"[calculationupdater] [calc] [calculate]" )
{
	const auto defaulted = arcstk::CalculationUpdater{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "CalculationUpdater property", "[calculationupdater] [calc] [calculate]" )
{
	using arcstk::CalculationUpdater;

	auto instance = CalculationUpdater {};


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


TEST_CASE ( "CalculationUpdater functions", "[calculationupdater] [calc] [calculate]" )
{
	using arcstk::CalculationUpdater;

	auto instance = CalculationUpdater {};

	SECTION ("CalculationUpdater() is correct")
	{
		FAIL ("CalculationUpdater() test is missing");
	}

	SECTION ("operator=() is correct")
	{
		FAIL ("operator=() test is missing");
	}

	SECTION ("~CalculationUpdater() is correct")
	{
		FAIL ("~CalculationUpdater() test is missing");
	}

	SECTION ("update() is correct")
	{
		FAIL ("update() test is missing");
	}

	SECTION ("algorithm_name() is correct")
	{
		FAIL ("algorithm_name() test is missing");
	}

	SECTION ("swap() is correct")
	{
		FAIL ("swap() test is missing");
	}

	SECTION ("do_algorithm() is correct")
	{
		FAIL ("do_algorithm() test is missing");
	}

	SECTION ("do_total_tracks() is correct")
	{
		FAIL ("do_total_tracks() test is missing");
	}

	SECTION ("do_offsets() is correct")
	{
		FAIL ("do_offsets() test is missing");
	}

	SECTION ("do_leadout() is correct")
	{
		FAIL ("do_leadout() test is missing");
	}

	SECTION ("do_init() is correct")
	{
		FAIL ("do_init() test is missing");
	}

	SECTION ("on_settings_changed() is correct")
	{
		FAIL ("on_settings_changed() test is missing");
	}

	SECTION ("on_completion() is correct")
	{
		FAIL ("on_completion() test is missing");
	}

	SECTION ("update_impl() is correct")
	{
		FAIL ("update_impl() test is missing");
	}

}


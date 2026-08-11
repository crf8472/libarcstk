#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for CalculationState.
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


TEST_CASE ( "CalculationState", "[calculationstate] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::CalculationState;

	auto instance = CalculationState {};


	SECTION ("Parametized construction is correct")
	{
		FAIL ( "Parametized construction test is missing" );
	}

	SECTION ("Copy construction is correct")
	{
		const CalculationState copied { Copy<CalculationState>::construct(instance) };

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy construction test is missing" );
	}

	SECTION ("Move construction is correct")
	{
		const CalculationState moved { Move<CalculationState>::construct(instance) };

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move construction test is missing" );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = CalculationState {};
		Copy<CalculationState>::assign(copied, instance);

		// --

        //CHECK ( copied.value() == instance.value() );
        //CHECK ( copied.name()  == instance.name() );
		FAIL ( "Copy assignment test is missing" );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = CalculationState {};
		Move<CalculationState>::assign(moved, instance);

		// --

        //CHECK ( moved.value() == instance.value() );
        //CHECK ( moved.name()  == instance.name() );
		FAIL ( "Move assignment test is missing" );
	}
}


TEST_CASE ( "CalculationState default constructed instance",
		"[calculationstate] [calc] [calculate]" )
{
	const auto defaulted = arcstk::CalculationState{};

	SECTION ( "is empty()")
	{
		CHECK ( defaulted.empty() );
	}

	SECTION ( "converts to FALSE")
	{
		CHECK ( !defaulted );
	}
}


TEST_CASE ( "CalculationState property", "[calculationstate] [calc] [calculate]" )
{
	using arcstk::CalculationState;

	auto instance = CalculationState {};


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


TEST_CASE ( "CalculationState functions", "[calculationstate] [calc] [calculate]" )
{
	using arcstk::CalculationState;

	auto instance = CalculationState {};

	SECTION ("current_offset() is correct")
	{
		FAIL ("current_offset() test is missing");
	}

	SECTION ("samples_processed() is correct")
	{
		FAIL ("samples_processed() test is missing");
	}

	SECTION ("track_samples_processed() is correct")
	{
		FAIL ("track_samples_processed() test is missing");
	}

	SECTION ("tracks_processed() is correct")
	{
		FAIL ("tracks_processed() test is missing");
	}

	SECTION ("sequences_processed() is correct")
	{
		FAIL ("sequences_processed() test is missing");
	}

	SECTION ("algo_time_elapsed() is correct")
	{
		FAIL ("algo_time_elapsed() test is missing");
	}

	SECTION ("increment_algo_time_elapsed() is correct")
	{
		FAIL ("increment_algo_time_elapsed() test is missing");
	}

	SECTION ("update_time_elapsed() is correct")
	{
		FAIL ("update_time_elapsed() test is missing");
	}

	SECTION ("increment_update_time_elapsed() is correct")
	{
		FAIL ("increment_update_time_elapsed() test is missing");
	}

	SECTION ("advance() is correct")
	{
		FAIL ("advance() test is missing");
	}

	SECTION ("update() is correct")
	{
		FAIL ("update() test is missing");
	}

	SECTION ("track_finished() is correct")
	{
		FAIL ("track_finished() test is missing");
	}

	SECTION ("swap() is correct")
	{
		FAIL ("swap() test is missing");
	}

}


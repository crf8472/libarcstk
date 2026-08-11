#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for CalculationState.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "CalculationState", "[calculationstate] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::details::CalculationState;

	auto instance = CalculationState {};

	instance.advance(237);

	REQUIRE ( instance.current_offset() == 237 );


	SECTION ("Copy construction is correct")
	{
		const CalculationState copied {
			Copy<CalculationState>::construct(instance) };

		// --

		CHECK ( copied.current_offset() == 237 );
	}

	SECTION ("Move construction is correct")
	{
		const CalculationState moved {
			Move<CalculationState>::construct(instance) };

		// --

		CHECK ( moved.current_offset() == 237 );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = CalculationState {};
		Copy<CalculationState>::assign(copied, instance);

		// --

		CHECK ( copied.current_offset() == 237 );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = CalculationState {};
		Move<CalculationState>::assign(moved, instance);

		// --

		CHECK ( moved.current_offset() == 237 );
	}
}


TEST_CASE ( "CalculationState default constructed instance",
		"[calculationstate] [calc] [calculate]" )
{
	using duration = std::chrono::duration<float>;

	const auto instance = arcstk::details::CalculationState{};

	SECTION ( "offset & all counters & all times == zero")
	{
		CHECK ( instance.current_offset()          == 0 );

		CHECK ( instance.samples_processed()       == 0 );
		CHECK ( instance.track_samples_processed() == 0 );
		CHECK ( instance.tracks_processed()        == 0 );
		CHECK ( instance.sequences_processed()     == 0 );

		CHECK ( instance.algo_time_elapsed()       == duration { 0 } );
		CHECK ( instance.update_time_elapsed()     == duration { 0 } );
	}
}


// TEST_CASE ( "CalculationState property",
// 		"[calculationstate] [calc] [calculate]" )
// {
// 	using arcstk::details::CalculationState;
//
// 	auto defaulted = CalculationState {};
//
//
// 	SECTION ("swap() is correct")
// 	{
// 		FAIL ( "swap() test is missing" );
// 	}
// }


// TEST_CASE ( "CalculationState functions",
// 		"[calculationstate] [calc] [calculate]" )
// {
// 	using arcstk::details::CalculationState;
//
// 	auto instance = CalculationState {};
//
// 	SECTION ("current_offset is correct")
// 	{
// 		FAIL ("current_offset test is missing");
// 	}
//
// 	SECTION ("samples_processed is correct")
// 	{
// 		FAIL ("samples_processed test is missing");
// 	}
//
// 	SECTION ("track_samples_processed is correct")
// 	{
// 		FAIL ("track_samples_processed test is missing");
// 	}
//
// 	SECTION ("tracks_processed is correct")
// 	{
// 		FAIL ("tracks_processed test is missing");
// 	}
//
// 	SECTION ("sequences_processed is correct")
// 	{
// 		FAIL ("sequences_processed test is missing");
// 	}
//
// 	SECTION ("algo_time_elapsed is correct")
// 	{
// 		FAIL ("algo_time_elapsed test is missing");
// 	}
//
// 	SECTION ("increment_algo_time_elapsed is correct")
// 	{
// 		FAIL ("increment_algo_time_elapsed test is missing");
// 	}
//
// 	SECTION ("update_time_elapsed is correct")
// 	{
// 		FAIL ("update_time_elapsed test is missing");
// 	}
//
// 	SECTION ("increment_update_time_elapsed is correct")
// 	{
// 		FAIL ("increment_update_time_elapsed test is missing");
// 	}
//
// 	SECTION ("advance is correct")
// 	{
// 		FAIL ("advance test is missing");
// 	}
//
// 	SECTION ("update is correct")
// 	{
// 		FAIL ("update test is missing");
// 	}
//
// 	SECTION ("track_finished is correct")
// 	{
// 		FAIL ("track_finished test is missing");
// 	}
//
// 	SECTION ("swap is correct")
// 	{
// 		FAIL ("swap test is missing");
// 	}
//
// }


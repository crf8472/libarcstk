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

#include <chrono>                 // for steady_clock::now
#include <iterator>               // for begin, cbegin, cend, end
#include <memory>                 // for make_unique, unique_ptr
#include <numeric>                // for iota
#include <type_traits>            // for is_default_constructible,....
#include <utility>                // for move
#include <vector>                 // for vector


TEST_CASE ( "CalculationState-Old", "[calculationstate] [calc] [calculate]" )
{
	using arcstk::AccurateRip::V1andV2;
	using arcstk::Algorithm;
	using arcstk::details::CalculationState;

	using std::begin;
	using std::end;

	auto algorithm { std::make_unique<V1andV2>() };
	auto state1 = CalculationState{};

	{
		const auto start_time { std::chrono::steady_clock::now() };

		auto dummy_data = std::vector<uint32_t>(1000000);
		std::iota(begin(dummy_data), end(dummy_data), 1);

		const auto time_elapsed {
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - start_time)
		};

		state1.update(1000000, time_elapsed);
		state1.track_finished();

		//state1.increment_update_time_elapsed(time_elapsed);
	}


	SECTION ("Construction is correct")
	{
		auto impl { CalculationState {} };

		CHECK ( impl.samples_processed() == 0 );
	}


	SECTION ("Copy construction is as declared")
	{
		CHECK ( std::is_copy_constructible<CalculationState>::value );

		CHECK (
			std::is_nothrow_copy_constructible<CalculationState>::value );
	}


	SECTION ("Move construction is as declared")
	{
		CHECK ( std::is_move_constructible<CalculationState>::value );

		CHECK ( std::is_nothrow_move_constructible<CalculationState>::value );
	}


	SECTION ("Copy construction is correct")
	{
		auto impl2 { state1 };

		CHECK ( impl2.current_offset()    == 1000000 );
		CHECK ( impl2.samples_processed() == 1000000 );
		CHECK ( impl2.track_samples_processed() == 0 );
		CHECK ( impl2.tracks_processed()  == 1 );
		//CHECK ( impl2.algo_time_elapsed()   > std::chrono::milliseconds::zero() );
		//CHECK ( impl2.update_time_elapsed() > std::chrono::milliseconds::zero() );
	}


	SECTION ("Move construction is correct")
	{
		auto impl3 { std::move(state1) };

		CHECK ( impl3.current_offset()    == 1000000 );
		CHECK ( impl3.samples_processed() == 1000000 );
		CHECK ( impl3.track_samples_processed() == 0 );
		CHECK ( impl3.tracks_processed()  == 1 );
		//CHECK ( impl3.algo_time_elapsed()   > std::chrono::milliseconds::zero() );
		//CHECK ( impl3.update_time_elapsed() > std::chrono::milliseconds::zero() );
		// track_samples_processed
		// tracks_processed
	}


	SECTION ("update() counts the amount of samples processed correctly")
	{
		CHECK ( state1.samples_processed() == 1000000 );
	}


	/*
	SECTION ("increment_update_time_elapsed() updates time counter")
	{
		CHECK ( state1.update_time_elapsed() > std::chrono::milliseconds::zero() );
	}
	*/
}


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


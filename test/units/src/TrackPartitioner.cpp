#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for TrackPartitioner.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif

#ifndef LIBARCSTK_CONSTRUCTION_HPP_
#include "construction.hpp"       // for Copy, Move
#endif


TEST_CASE ( "TrackPartitioner", "[trackpartitioner] [calc] [calculate]" )
{
	using arcstk::testing::Copy;
	using arcstk::testing::Move;

	using arcstk::AudioSize;
	using arcstk::UNIT;
	using arcstk::details::TrackPartitioner;

	auto instance = TrackPartitioner {
		{ /* use Bach, Organ Concertos, Simon Preston, DGG */
			{     33 * 588, UNIT::SAMPLES },
			{   5225 * 588, UNIT::SAMPLES },
			{   7390 * 588, UNIT::SAMPLES },
			{  23380 * 588, UNIT::SAMPLES },
			{  35608 * 588, UNIT::SAMPLES },
			{  49820 * 588, UNIT::SAMPLES },
			{  69508 * 588, UNIT::SAMPLES },
			{  87733 * 588, UNIT::SAMPLES },
			{ 106333 * 588, UNIT::SAMPLES },
			{ 139495 * 588, UNIT::SAMPLES },
			{ 157863 * 588, UNIT::SAMPLES },
			{ 198495 * 588, UNIT::SAMPLES },
			{ 213368 * 588, UNIT::SAMPLES },
			{ 225320 * 588, UNIT::SAMPLES },
			{ 234103 * 588, UNIT::SAMPLES }
		},
		{ 253038 * 588, UNIT::SAMPLES },
		{ /* accuraterip range */ 33 * 588 + 2939, 253038 * 588 - 2940 }
	};


	SECTION ("Parametized construction is correct")
	{
		CHECK ( instance.total_samples()       ==
				AudioSize { 253038 * 588, UNIT::SAMPLES } );
		CHECK ( instance.legal_range().lower() == 33 * 588 + 2939 );
		CHECK ( instance.legal_range().upper() == 253038 * 588 - 2940 );
	}

	SECTION ("Copy construction is correct")
	{
		const TrackPartitioner copied {
			Copy<TrackPartitioner>::construct(instance) };

		// --

		CHECK ( copied.total_samples() ==
				AudioSize { 253038 * 588, UNIT::SAMPLES } );
	}

	SECTION ("Move construction is correct")
	{
		const TrackPartitioner moved {
			Move<TrackPartitioner>::construct(instance) };

		// --

		CHECK ( moved.total_samples() ==
				AudioSize { 253038 * 588, UNIT::SAMPLES } );
	}

	SECTION ("Copy assignment is correct")
	{
		auto copied = TrackPartitioner { /* empty */ {}, {}, {} };
		Copy<TrackPartitioner>::assign(copied, instance);

		// --

		CHECK ( copied.total_samples() ==
				AudioSize { 253038 * 588, UNIT::SAMPLES } );
	}

	SECTION ("Move assignment is correct")
	{
		auto moved = TrackPartitioner { /* empty */ {}, {}, {} };
		Move<TrackPartitioner>::assign(moved, instance);

		// --

		CHECK ( moved.total_samples() ==
				AudioSize { 253038 * 588, UNIT::SAMPLES } );
	}
}

// TODO Implement function test
// TEST_CASE ( "TrackPartitioner functions",
// 		"[trackpartitioner] [calc] [calculate]" )
// {
// 	using arcstk::details::TrackPartitioner;
//
// 	auto instance = TrackPartitioner {};
//
//
// 	SECTION ("create_partitioning() is correct")
// 	{
// 		FAIL ("do_create_partitioning() test is missing");
// 	}
// }


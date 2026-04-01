#include "catch2/catch_test_macros.hpp"
#include "catch2/catch_template_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for calculate_details.hpp.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // for Algorithm, Points
#endif
#ifndef LIBARCSTK_CALCULATE_DETAILS_HPP_
#include "calculate_details.hpp"  // TO BE TESTED
#endif

#include <chrono>                 // for steady_clock::now
#include <iterator>               // for begin, cbegin, cend, end
#include <memory>                 // for make_unique
#include <numeric>                // for iota
#include <type_traits>            // for is_copy_constructible,...
#include <vector>                 // for vector

#ifndef LIBARCSTK_ALGORITHMS_HPP_
#include "algorithms.hpp"         // for AccurateRipV1V2
#endif
#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"           // for checksum::type, Checksums
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"           // for AudioSize, UNIT
#endif


TEST_CASE ( "Interval", "[calculate_details] [calc]" )
{
	using arcstk::details::Interval;

	const auto i1 = Interval<int32_t> {  10,  17 };
	const auto i2 = Interval<int32_t> { -12, 123 };
	const auto i3 = Interval<int32_t> {   2,   1 };

	SECTION ("Construction is correct")
	{
		CHECK ( i1.lower() ==  10 );
		CHECK ( i2.lower() == -12 );
		CHECK ( i3.lower() ==   1 );

		CHECK ( i1.upper() ==  17 );
		CHECK ( i2.upper() == 123 );
		CHECK ( i3.upper() ==   2 );
	}

	SECTION ("contains() is correct")
	{
		CHECK ( !i1.contains( 9) );
		CHECK (  i1.contains(10) );
		CHECK (  i1.contains(11) );
		CHECK (  i1.contains(12) );
		CHECK (  i1.contains(13) );
		CHECK (  i1.contains(14) );
		CHECK (  i1.contains(15) );
		CHECK (  i1.contains(16) );
		CHECK (  i1.contains(17) );
		CHECK ( !i1.contains(18) );

		CHECK ( !i2.contains(-13) );
		CHECK (  i2.contains(-12) );
		CHECK (  i2.contains(-11) );
		CHECK (  i2.contains(-10) );
		CHECK (  i2.contains(122) );
		CHECK (  i2.contains(123) );
		CHECK ( !i2.contains(124) );

		CHECK ( !i3.contains(0) );
		CHECK (  i3.contains(1) );
		CHECK (  i3.contains(2) );
		CHECK ( !i3.contains(3) );
	}
}


TEST_CASE ( "get_partitioning", "[get_partitioning] [calc]" )
{
	// Use samples as points, not frames
	const auto points = std::vector<int32_t> {
		/* Bach, Organ Concertos, Simon Preston, DGG */
		    33 * 588,
		  5225 * 588,
		  7390 * 588,
		 23380 * 588,
		 35608 * 588,
		 49820 * 588,
		 69508 * 588,
		 87733 * 588,
		106333 * 588,
		139495 * 588,
		157863 * 588,
		198495 * 588,
		213368 * 588,
		225320 * 588,
		234103 * 588
	};
	// leadout 253038 * 588 == 148786344


	SECTION ( "Partitioning with ToC in 1 block is correct" )
	{
		using arcstk::AudioSize;
		using arcstk::UNIT;

		auto p { arcstk::details::get_partitioning(
				{ /* use samples in one block  */    0, 253038 * 588        },
				{ /* use accuraterip algorithm */ 22343, 253038 * 588 - 2940 },
				{ /* use Bach, Organ Concertos, Simon Preston, DGG */
					AudioSize {     33 * 588, UNIT::SAMPLES },
					AudioSize {   5225 * 588, UNIT::SAMPLES },
					AudioSize {   7390 * 588, UNIT::SAMPLES },
					AudioSize {  23380 * 588, UNIT::SAMPLES },
					AudioSize {  35608 * 588, UNIT::SAMPLES },
					AudioSize {  49820 * 588, UNIT::SAMPLES },
					AudioSize {  69508 * 588, UNIT::SAMPLES },
					AudioSize {  87733 * 588, UNIT::SAMPLES },
					AudioSize { 106333 * 588, UNIT::SAMPLES },
					AudioSize { 139495 * 588, UNIT::SAMPLES },
					AudioSize { 157863 * 588, UNIT::SAMPLES },
					AudioSize { 198495 * 588, UNIT::SAMPLES },
					AudioSize { 213368 * 588, UNIT::SAMPLES },
					AudioSize { 225320 * 588, UNIT::SAMPLES },
					AudioSize { 234103 * 588, UNIT::SAMPLES }
				}
		)};

		CHECK ( p.size() == 15 );

		// CHECK ( p[ 0].begin_offset()     ==     2939 ); // [i]: offset * 588 + 2939
		// CHECK ( p[ 0].end_offset()       ==    19403 ); // [i+1]: offset * 588
		// CHECK ( p[ 0].starts_track()     ==    false );
		// CHECK ( p[ 0].ends_track()       ==     true );
		// CHECK ( p[ 0].track()            ==        0 );
		// CHECK ( p[ 0].size()             ==    16464 ); // end - begin offset

		CHECK ( p[ 0].begin_offset()     ==    22343 ); // [i]: offset * 588
		CHECK ( p[ 0].end_offset()       ==  3072299 ); // [i+1]: offset * 588
		CHECK ( p[ 0].starts_track()     ==     true );
		CHECK ( p[ 0].ends_track()       ==     true );
		CHECK ( p[ 0].track()            ==        1 );
		CHECK ( p[ 0].size()             ==  3049956 ); // end - begin offset

		CHECK ( p[ 1].begin_offset()     ==  3072300 ); // == 0:end_offset()
		CHECK ( p[ 1].end_offset()       ==  4345319 );
		CHECK ( p[ 1].starts_track()     ==     true );
		CHECK ( p[ 1].ends_track()       ==     true );
		CHECK ( p[ 1].track()            ==        2 );
		CHECK ( p[ 1].size()             ==  1273019 );

		CHECK ( p[ 2].begin_offset()     ==   4345320 );
		CHECK ( p[ 2].end_offset()       ==  13747439 );
		CHECK ( p[ 2].starts_track()     ==      true );
		CHECK ( p[ 2].ends_track()       ==      true );
		CHECK ( p[ 2].track()            ==         3 );
		CHECK ( p[ 2].size()             ==   9402119 );

		CHECK ( p[ 3].begin_offset()     ==  13747440 );
		CHECK ( p[ 3].end_offset()       ==  20937503 );
		CHECK ( p[ 3].starts_track()     ==      true );
		CHECK ( p[ 3].ends_track()       ==      true );
		CHECK ( p[ 3].track()            ==         4 );
		CHECK ( p[ 3].size()             ==   7190063 );

		CHECK ( p[ 4].begin_offset()     ==  20937504 );
		CHECK ( p[ 4].end_offset()       ==  29294159 );
		CHECK ( p[ 4].starts_track()     ==      true );
		CHECK ( p[ 4].ends_track()       ==      true );
		CHECK ( p[ 4].track()            ==         5 );
		CHECK ( p[ 4].size()             ==   8356655 );

		CHECK ( p[ 5].begin_offset()     ==  29294160 );
		CHECK ( p[ 5].end_offset()       ==  40870703 );
		CHECK ( p[ 5].starts_track()     ==      true );
		CHECK ( p[ 5].ends_track()       ==      true );
		CHECK ( p[ 5].track()            ==         6 );
		CHECK ( p[ 5].size()             ==  11576543 );

		CHECK ( p[ 6].begin_offset()     ==  40870704 );
		CHECK ( p[ 6].end_offset()       ==  51587003 );
		CHECK ( p[ 6].starts_track()     ==      true );
		CHECK ( p[ 6].ends_track()       ==      true );
		CHECK ( p[ 6].track()            ==         7 );
		CHECK ( p[ 6].size()             ==  10716299 );

		CHECK ( p[ 7].begin_offset()     ==  51587004 );
		CHECK ( p[ 7].end_offset()       ==  62523803 );
		CHECK ( p[ 7].starts_track()     ==      true );
		CHECK ( p[ 7].ends_track()       ==      true );
		CHECK ( p[ 7].track()            ==         8 );
		CHECK ( p[ 7].size()             ==  10936799 );

		CHECK ( p[ 8].begin_offset()     ==  62523804 );
		CHECK ( p[ 8].end_offset()       ==  82023059 );
		CHECK ( p[ 8].starts_track()     ==      true );
		CHECK ( p[ 8].ends_track()       ==      true );
		CHECK ( p[ 8].track()            ==         9 );
		CHECK ( p[ 8].size()             ==  19499255 );

		CHECK ( p[ 9].begin_offset()     ==  82023060 );
		CHECK ( p[ 9].end_offset()       ==  92823443 );
		CHECK ( p[ 9].starts_track()     ==      true );
		CHECK ( p[ 9].ends_track()       ==      true );
		CHECK ( p[ 9].track()            ==        10 );
		CHECK ( p[ 9].size()             ==  10800383 );

		CHECK ( p[10].begin_offset()     ==  92823444 );
		CHECK ( p[10].end_offset()       == 116715059 );
		CHECK ( p[10].starts_track()     ==      true );
		CHECK ( p[10].ends_track()       ==      true );
		CHECK ( p[10].track()            ==        11 );
		CHECK ( p[10].size()             ==  23891615 );

		CHECK ( p[11].begin_offset()     == 116715060 );
		CHECK ( p[11].end_offset()       == 125460383 );
		CHECK ( p[11].starts_track()     ==      true );
		CHECK ( p[11].ends_track()       ==      true );
		CHECK ( p[11].track()            ==        12 );
		CHECK ( p[11].size()             ==   8745323 );

		CHECK ( p[12].begin_offset()     == 125460384 );
		CHECK ( p[12].end_offset()       == 132488159 );
		CHECK ( p[12].starts_track()     ==      true );
		CHECK ( p[12].ends_track()       ==      true );
		CHECK ( p[12].track()            ==        13 );
		CHECK ( p[12].size()             ==   7027775 );

		CHECK ( p[13].begin_offset()     == 132488160 );
		CHECK ( p[13].end_offset()       == 137652563 );
		CHECK ( p[13].starts_track()     ==      true );
		CHECK ( p[13].ends_track()       ==      true );
		CHECK ( p[13].track()            ==        14 );
		CHECK ( p[13].size()             ==   5164403 );

		CHECK ( p[14].begin_offset()     == 137652564 );
		CHECK ( p[14].end_offset()       == 148783404 ); // 148783405
		CHECK ( p[14].starts_track()     ==      true );
		CHECK ( p[14].ends_track()       ==      true );
		CHECK ( p[14].track()            ==        15 );
		CHECK ( p[14].size()             ==  11130840 ); // 11130842
	}

	SECTION ( "Partitioning with ToC: first block is correct" )
	{
		using arcstk::AudioSize;
		using arcstk::UNIT;

		auto p { arcstk::details::get_partitioning(
				{ /* use samples in one block  */    0, 29000000 },
				{ /* use accuraterip algorithm */ 22343, 148786344 - 2940 },
				{ /* use Bach, Organ Concertos, Simon Preston, DGG */
					AudioSize {     33 * 588, UNIT::SAMPLES },
					AudioSize {   5225 * 588, UNIT::SAMPLES },
					AudioSize {   7390 * 588, UNIT::SAMPLES },
					AudioSize {  23380 * 588, UNIT::SAMPLES },
					AudioSize {  35608 * 588, UNIT::SAMPLES },
					AudioSize {  49820 * 588, UNIT::SAMPLES },
					AudioSize {  69508 * 588, UNIT::SAMPLES },
					AudioSize {  87733 * 588, UNIT::SAMPLES },
					AudioSize { 106333 * 588, UNIT::SAMPLES },
					AudioSize { 139495 * 588, UNIT::SAMPLES },
					AudioSize { 157863 * 588, UNIT::SAMPLES },
					AudioSize { 198495 * 588, UNIT::SAMPLES },
					AudioSize { 213368 * 588, UNIT::SAMPLES },
					AudioSize { 225320 * 588, UNIT::SAMPLES },
					AudioSize { 234103 * 588, UNIT::SAMPLES }
				}
		)};

		CHECK ( p.size() == 5 );

		// CHECK ( p[ 0].begin_offset()     ==     2939 ); // [i]: offset * 588
		// CHECK ( p[ 0].end_offset()       ==    19403 ); // [i+1]: offset * 588
		// CHECK ( p[ 0].starts_track()     ==    false );
		// CHECK ( p[ 0].ends_track()       ==     true );
		// CHECK ( p[ 0].track()            ==        0 );
		// CHECK ( p[ 0].size()             ==    16464 ); // end - begin offset

		CHECK ( p[ 0].begin_offset()     ==    22343 ); // [i]: offset * 588
		CHECK ( p[ 0].end_offset()       ==  3072299 ); // [i+1]: offset * 588
		CHECK ( p[ 0].starts_track()     ==     true );
		CHECK ( p[ 0].ends_track()       ==     true );
		CHECK ( p[ 0].track()            ==        1 );
		CHECK ( p[ 0].size()             ==  3049956 ); // end - begin offset

		CHECK ( p[ 1].begin_offset()     ==  3072300 ); // == 0:end_offset()
		CHECK ( p[ 1].end_offset()       ==  4345319 );
		CHECK ( p[ 1].starts_track()     ==     true );
		CHECK ( p[ 1].ends_track()       ==     true );
		CHECK ( p[ 1].track()            ==        2 );
		CHECK ( p[ 1].size()             ==  1273019 );

		CHECK ( p[ 2].begin_offset()     ==   4345320 );
		CHECK ( p[ 2].end_offset()       ==  13747439 );
		CHECK ( p[ 2].starts_track()     ==      true );
		CHECK ( p[ 2].ends_track()       ==      true );
		CHECK ( p[ 2].track()            ==         3 );
		CHECK ( p[ 2].size()             ==   9402119 );

		CHECK ( p[ 3].begin_offset()     ==  13747440 );
		CHECK ( p[ 3].end_offset()       ==  20937503 );
		CHECK ( p[ 3].starts_track()     ==      true );
		CHECK ( p[ 3].ends_track()       ==      true );
		CHECK ( p[ 3].track()            ==         4 );
		CHECK ( p[ 3].size()             ==   7190063 );

		CHECK ( p[ 4].begin_offset()     ==  20937504 );
		CHECK ( p[ 4].end_offset()       ==  29000000 );
		CHECK ( p[ 4].starts_track()     ==      true );
		CHECK ( p[ 4].ends_track()       ==     false );
		CHECK ( p[ 4].track()            ==         5 );
		CHECK ( p[ 4].size()             ==   8062496 );
	}

	SECTION ( "Partitioning with ToC: last block is correct" )
	{
		using arcstk::AudioSize;
		using arcstk::UNIT;

		auto p { arcstk::details::get_partitioning(
				{ /* use samples in one block  */  120000000, 148786344 },
				{ /* use accuraterip algorithm */ 2940, 148786344 - 2939 },
				{ /* use Bach, Organ Concertos, Simon Preston, DGG */
					AudioSize {     33 * 588, UNIT::SAMPLES },
					AudioSize {   5225 * 588, UNIT::SAMPLES },
					AudioSize {   7390 * 588, UNIT::SAMPLES },
					AudioSize {  23380 * 588, UNIT::SAMPLES },
					AudioSize {  35608 * 588, UNIT::SAMPLES },
					AudioSize {  49820 * 588, UNIT::SAMPLES },
					AudioSize {  69508 * 588, UNIT::SAMPLES },
					AudioSize {  87733 * 588, UNIT::SAMPLES },
					AudioSize { 106333 * 588, UNIT::SAMPLES },
					AudioSize { 139495 * 588, UNIT::SAMPLES },
					AudioSize { 157863 * 588, UNIT::SAMPLES },
					AudioSize { 198495 * 588, UNIT::SAMPLES },
					AudioSize { 213368 * 588, UNIT::SAMPLES },
					AudioSize { 225320 * 588, UNIT::SAMPLES },
					AudioSize { 234103 * 588, UNIT::SAMPLES }
				}
		)};

		CHECK ( p.size() == 4 );

		CHECK ( p[ 0].begin_offset()     == 120000000 );
		CHECK ( p[ 0].end_offset()       == 125460383 );
		CHECK ( p[ 0].starts_track()     ==     false );
		CHECK ( p[ 0].ends_track()       ==      true );
		CHECK ( p[ 0].track()            ==        12 );
		CHECK ( p[ 0].size()             ==   5460383 );

		CHECK ( p[ 1].begin_offset()     == 125460384 );
		CHECK ( p[ 1].end_offset()       == 132488159 );
		CHECK ( p[ 1].starts_track()     ==      true );
		CHECK ( p[ 1].ends_track()       ==      true );
		CHECK ( p[ 1].track()            ==        13 );
		CHECK ( p[ 1].size()             ==   7027775 );

		CHECK ( p[ 2].begin_offset()     == 132488160 );
		CHECK ( p[ 2].end_offset()       == 137652563 );
		CHECK ( p[ 2].starts_track()     ==      true );
		CHECK ( p[ 2].ends_track()       ==      true );
		CHECK ( p[ 2].track()            ==        14 );
		CHECK ( p[ 2].size()             ==   5164403 );

		CHECK ( p[ 3].begin_offset()     == 137652564 );
		CHECK ( p[ 3].end_offset()       == 148783405 );
		CHECK ( p[ 3].starts_track()     ==      true );
		CHECK ( p[ 3].ends_track()       ==      true );
		CHECK ( p[ 3].track()            ==        15 );
		CHECK ( p[ 3].size()             ==  11130841 );
	}


	SECTION ( "First block is correct if it does not contain complete track" )
	{
		using arcstk::AudioSize;
		using arcstk::UNIT;

		// 5. block of 4095 contains beginning of first track but not its end

		auto p { arcstk::details::get_partitioning(
				{ /* use samples in one block  */ 20475, 24570 },
				{ /* use accuraterip algorithm */  2939, 253038 * 588 - 2940 },
				{ /* use Bach, Organ Concertos, Simon Preston, DGG */
					AudioSize {     33 * 588, UNIT::SAMPLES },
					AudioSize {   5225 * 588, UNIT::SAMPLES },
					AudioSize {   7390 * 588, UNIT::SAMPLES },
					AudioSize {  23380 * 588, UNIT::SAMPLES },
					AudioSize {  35608 * 588, UNIT::SAMPLES },
					AudioSize {  49820 * 588, UNIT::SAMPLES },
					AudioSize {  69508 * 588, UNIT::SAMPLES },
					AudioSize {  87733 * 588, UNIT::SAMPLES },
					AudioSize { 106333 * 588, UNIT::SAMPLES },
					AudioSize { 139495 * 588, UNIT::SAMPLES },
					AudioSize { 157863 * 588, UNIT::SAMPLES },
					AudioSize { 198495 * 588, UNIT::SAMPLES },
					AudioSize { 213368 * 588, UNIT::SAMPLES },
					AudioSize { 225320 * 588, UNIT::SAMPLES },
					AudioSize { 234103 * 588, UNIT::SAMPLES }
				}
		)};

		CHECK ( p.size() == 1 );

		CHECK ( p[ 0].begin_offset()     == 20475 );
		CHECK ( p[ 0].end_offset()       == 24570 );
		CHECK ( p[ 0].starts_track()     == false );
		CHECK ( p[ 0].ends_track()       == false );
		CHECK ( p[ 0].track()            ==     1 );
		CHECK ( p[ 0].size()             ==  4095 );
	}

	SECTION ( "Last block is correct if it does not contain complete track" )
	{
		using arcstk::AudioSize;
		using arcstk::UNIT;

		// 5. block of 4095 contains beginning of first track but not its end

		auto p { arcstk::details::get_partitioning(
				{ /* use samples in one block  */ 148782249, 148786344 },
				{ /* use accuraterip algorithm */  2939, 253038 * 588 - 2940 },
				{ /* use Bach, Organ Concertos, Simon Preston, DGG */
					AudioSize {     33 * 588, UNIT::SAMPLES },
					AudioSize {   5225 * 588, UNIT::SAMPLES },
					AudioSize {   7390 * 588, UNIT::SAMPLES },
					AudioSize {  23380 * 588, UNIT::SAMPLES },
					AudioSize {  35608 * 588, UNIT::SAMPLES },
					AudioSize {  49820 * 588, UNIT::SAMPLES },
					AudioSize {  69508 * 588, UNIT::SAMPLES },
					AudioSize {  87733 * 588, UNIT::SAMPLES },
					AudioSize { 106333 * 588, UNIT::SAMPLES },
					AudioSize { 139495 * 588, UNIT::SAMPLES },
					AudioSize { 157863 * 588, UNIT::SAMPLES },
					AudioSize { 198495 * 588, UNIT::SAMPLES },
					AudioSize { 213368 * 588, UNIT::SAMPLES },
					AudioSize { 225320 * 588, UNIT::SAMPLES },
					AudioSize { 234103 * 588, UNIT::SAMPLES }
				}
		)};

		CHECK ( p.size() == 1 );

		CHECK ( p[ 0].begin_offset()     == 148782249 );
		CHECK ( p[ 0].end_offset()       == 148783404 );
		CHECK ( p[ 0].starts_track()     == false );
		CHECK ( p[ 0].ends_track()       ==  true );
		CHECK ( p[ 0].track()            ==    15 );
		CHECK ( p[ 0].size()             ==  1155 );
	}
}


/*
TEST_CASE ( "Partition", "[partitioner] [calc]" )
{
	// TODO Implement
}


TEST_CASE ( "TrackPartitioner", "[trackpartitioner] [calc]" )
{
	// TODO Implement
}
*/

// Commented out: intended use is:
//
//TEMPLATE_TEST_CASE ( "Counter", "[counter] [calc]", int32_t )
//{
//	...
//	auto c = Counter<TestType>{ 0 };
//
// But this causes a compile warning by Clang 19:
// https://github.com/catchorg/Catch2/issues/2910

TEST_CASE ( "Counter", "[counter] [calc]" )
{
	using arcstk::details::Counter;

	auto c = Counter<int32_t>{ 0 };

	REQUIRE ( c.value() == 0 );

	SECTION ( "increment() is correct" )
	{
		c.increment(5);
		c.increment(28);
		c.increment(10191);

		CHECK ( c.value() == 10224 );
	}

	SECTION ( "reset() is correct" )
	{
		c.increment(5);
		c.increment(10191);

		REQUIRE ( c.value() == 10196 );

		c.reset();

		CHECK ( c.value() == 0 );
	}
}


TEST_CASE ( "CalculationStateImpl",
		"[calculationstateimpl] [calc]" )
{
	using arcstk::AccurateRip::V1andV2;
	using arcstk::Algorithm;
	using arcstk::details::CalculationStateImpl;

	using std::begin;
	using std::end;

	auto algorithm { std::make_unique<V1andV2>() };
	auto impl1 { CalculationStateImpl { algorithm.get() } };

	{
		auto dummy_data = std::vector<uint32_t>(1000000);
		std::iota(begin(dummy_data), end(dummy_data), 1);

		const auto start_time { std::chrono::steady_clock::now() };

		impl1.update(begin(dummy_data), end(dummy_data));
		impl1.track_finished();

		const auto time_elapsed {
			std::chrono::duration_cast<std::chrono::milliseconds>(
				std::chrono::steady_clock::now() - start_time)
		};

		impl1.increment_update_time_elapsed(time_elapsed);
	}


	SECTION ("Construction is correct")
	{
		auto impl { CalculationStateImpl { algorithm.get() } };

		CHECK ( impl.algorithm()         == algorithm.get() );
		CHECK ( impl.samples_processed() == 0 );
	}


	SECTION ("Copy construction is as declared")
	{
		CHECK ( std::is_copy_constructible<CalculationStateImpl>::value );

		CHECK ( not
			std::is_nothrow_copy_constructible<CalculationStateImpl>::value );
	}


	SECTION ("Move construction is as declared")
	{
		CHECK ( std::is_move_constructible<CalculationStateImpl>::value );

		CHECK ( std::is_nothrow_move_constructible<CalculationStateImpl>::value );
	}


	SECTION ("Copy construction is correct")
	{
		auto impl2 { impl1 };

		CHECK ( impl2.algorithm()         == algorithm.get() );
		CHECK ( impl2.samples_processed() == 1000000 );
		CHECK ( impl2.update_time_elapsed() > std::chrono::milliseconds::zero() );
	}


	SECTION ("Move construction is correct")
	{
		auto impl3 { std::move(impl1) };

		CHECK ( impl3.algorithm()         == algorithm.get() );
		CHECK ( impl3.samples_processed() == 1000000 );
		CHECK ( impl3.update_time_elapsed() > std::chrono::milliseconds::zero() );
	}


	SECTION ("update() counts the amount of samples processed correctly")
	{
		CHECK ( impl1.samples_processed() == 1000000 );
	}


	SECTION ("increment_update_time_elapsed() updates time counter")
	{
		CHECK ( impl1.update_time_elapsed() > std::chrono::milliseconds::zero() );
	}


	SECTION ("current_subtotal() returns the subtotals")
	{
		auto checksums { impl1.current_subtotal() };

		CHECK ( checksums.size() == 2 );
	}
}


TEST_CASE ( "perform_update", "[perform_update] [calc]" )
{
	using arcstk::AccurateRip::V1andV2;
	using arcstk::Algorithm;
	using arcstk::AudioSize;
	using arcstk::Checksums;
	using arcstk::Context;
	using arcstk::Points;
	using arcstk::Settings;
	using arcstk::UNIT;
	using arcstk::checksum::type;

	using arcstk::details::ind2am;
	using arcstk::details::Interval;
	using arcstk::details::TrackPartitioner;
	using arcstk::details::CalculationStateImpl;
	using arcstk::details::perform_update;

	using std::cbegin;
	using std::cend;
	using std::begin;
	using std::end;

	// This test simulates the calculation of an album

	const auto s { Settings { Context::ALBUM } };

	auto algorithm { std::make_unique<V1andV2>() };
	algorithm->set_settings(&s);

	auto state { CalculationStateImpl { algorithm.get() } };

	REQUIRE ( state.algorithm() == algorithm.get() );

	/* use Bach, Organ Concertos, Simon Preston, DGG */
	const auto partitioner { TrackPartitioner {
		AudioSize { 253038 * 588 /* 148786344 */, UNIT::SAMPLES },
		{ /* split points (track offsets) */
			AudioSize {     33 * 588, UNIT::SAMPLES },
			AudioSize {   5225 * 588, UNIT::SAMPLES },
			AudioSize {   7390 * 588, UNIT::SAMPLES },
			AudioSize {  23380 * 588, UNIT::SAMPLES },
			AudioSize {  35608 * 588, UNIT::SAMPLES },
			AudioSize {  49820 * 588, UNIT::SAMPLES },
			AudioSize {  69508 * 588, UNIT::SAMPLES },
			AudioSize {  87733 * 588, UNIT::SAMPLES },
			AudioSize { 106333 * 588, UNIT::SAMPLES },
			AudioSize { 139495 * 588, UNIT::SAMPLES },
			AudioSize { 157863 * 588, UNIT::SAMPLES },
			AudioSize { 198495 * 588, UNIT::SAMPLES },
			AudioSize { 213368 * 588, UNIT::SAMPLES },
			AudioSize { 225320 * 588, UNIT::SAMPLES },
			AudioSize { 234103 * 588, UNIT::SAMPLES }
		},
		/* legal range w/ skips */ { 33 * 588 + 2939, 253038 * 588 - 2940 },
	}};

	REQUIRE ( partitioner.total_samples().samples() == 148786344 );
	REQUIRE ( partitioner.legal_range().lower()     == 22343 );
	REQUIRE ( partitioner.legal_range().upper()     == 148783404 );
	// TODO Verify split points

	Checksums buffer { Checksums{}  };

	REQUIRE ( buffer.size() == 0 );

	// for convenience
	const int32_t skipped_front { 19404 + 2939 }; // equivalent to legal lower

	auto dummy_data = std::vector<uint32_t>(148786344 );
	std::iota(begin(dummy_data), end(dummy_data), 1);


	SECTION ("Updating album w/ block_size 16777216 yields correct checksums")
	{
		// This simulates libarcsdec:readerwav

		const auto block_size = int { 16777216 }; // samples
		auto r = bool { true };

		r = perform_update(	cbegin(dummy_data) + 0 * block_size,
							cbegin(dummy_data) + 1 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == block_size );
		CHECK ( state.samples_processed() == block_size - skipped_front );
		CHECK ( buffer.size()             == 3 );

		CHECK ( buffer[ 0].get(type::ARCS1).first == 0x0AF18BB6u );
		CHECK ( buffer[ 0].get(type::ARCS2).first == 0x8FBB68BAu );

		CHECK ( buffer[ 1].get(type::ARCS1).first == 0x60F64E9Au );
		CHECK ( buffer[ 1].get(type::ARCS2).first == 0x8D040A9Au );

		CHECK ( buffer[ 2].get(type::ARCS1).first == 0xBC5C57ECu );
		CHECK ( buffer[ 2].get(type::ARCS2).first == 0x2A4FD377u );

		r = perform_update( cbegin(dummy_data) + 1 * block_size,
							cbegin(dummy_data) + 2 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 2 * block_size );
		CHECK ( state.samples_processed() == 2 * block_size - skipped_front );
		CHECK ( buffer.size()             == 5 );

		CHECK ( buffer[ 3].get(type::ARCS1).first == 0xD394FC08u );
		CHECK ( buffer[ 3].get(type::ARCS2).first == 0xCE55344Bu );

		CHECK ( buffer[ 4].get(type::ARCS1).first == 0xD52E3008u );
		CHECK ( buffer[ 4].get(type::ARCS2).first == 0x022C486Du );

		r = perform_update( cbegin(dummy_data) + 2 * block_size,
							cbegin(dummy_data) + 3 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 3 * block_size );
		CHECK ( state.samples_processed() == 3 * block_size - skipped_front );
		CHECK ( buffer.size()             == 6 );

		CHECK ( buffer[ 5].get(type::ARCS1).first == 0x528B55D0u );
		CHECK ( buffer[ 5].get(type::ARCS2).first == 0xC4778057u );

		r = perform_update( cbegin(dummy_data) + 3 * block_size,
							cbegin(dummy_data) + 4 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 4 * block_size );
		CHECK ( state.samples_processed() == 4 * block_size - skipped_front );
		CHECK ( buffer.size()             == 8 );

		CHECK ( buffer[ 6].get(type::ARCS1).first == 0xB53625EAu );
		CHECK ( buffer[ 6].get(type::ARCS2).first == 0x29DF16E5u );

		CHECK ( buffer[ 7].get(type::ARCS1).first == 0x55480A90u );
		CHECK ( buffer[ 7].get(type::ARCS2).first == 0x390C2F05u );

		r = perform_update( cbegin(dummy_data) + 4 * block_size,
							cbegin(dummy_data) + 5 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 5 * block_size );
		CHECK ( state.samples_processed() == 5 * block_size - skipped_front );
		CHECK ( buffer.size()             == 9 );

		CHECK ( buffer[ 8].get(type::ARCS1).first == 0x53262404u );
		CHECK ( buffer[ 8].get(type::ARCS2).first == 0xA8B5ADDDu );

		r = perform_update( cbegin(dummy_data) + 5 * block_size,
							cbegin(dummy_data) + 6 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 6 * block_size );
		CHECK ( state.samples_processed() == 6 * block_size - skipped_front );
		CHECK ( buffer.size()             == 10 );

		CHECK ( buffer[ 9].get(type::ARCS1).first == 0x33A23980u );
		CHECK ( buffer[ 9].get(type::ARCS2).first == 0x4D9350B0u );

		r = perform_update( cbegin(dummy_data) + 6 * block_size,
							cbegin(dummy_data) + 7 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 7 * block_size );
		CHECK ( state.samples_processed() == 7 * block_size - skipped_front );
		CHECK ( buffer.size()             == 11 );

		CHECK ( buffer[10].get(type::ARCS1).first == 0xB66906B0u );
		CHECK ( buffer[10].get(type::ARCS2).first == 0x49D26578u );

		r = perform_update( cbegin(dummy_data) + 7 * block_size,
							cbegin(dummy_data) + 8 * block_size,
							partitioner, state, buffer);

		CHECK ( !r );
		CHECK ( state.current_offset()    == 8 * block_size );
		CHECK ( state.samples_processed() == 8 * block_size - skipped_front );
		CHECK ( buffer.size()             == 13 );

		CHECK ( buffer[11].get(type::ARCS1).first == 0x2BE3B232u );
		CHECK ( buffer[11].get(type::ARCS2).first == 0x355C7E28u );

		CHECK ( buffer[12].get(type::ARCS1).first == 0x5D229B60u );
		CHECK ( buffer[12].get(type::ARCS2).first == 0x970C0A35u );

		r = perform_update( cbegin(dummy_data) + 8 * block_size,
							cbegin(dummy_data) + 9 * block_size,
							partitioner, state, buffer);

		CHECK ( r );

		// After the last partition, current_offset() will be 1 index ahead
		// as before. However, the block is smaller than block_size.

		CHECK ( state.current_offset()    ==
				ind2am(partitioner.legal_range().upper()) );

		CHECK ( state.samples_processed() ==
				ind2am(partitioner.legal_range().upper()) - skipped_front );

		CHECK ( buffer.size()             == 15 );

		CHECK ( buffer[13].get(type::ARCS1).first == 0x3EF9CE06u );
		CHECK ( buffer[13].get(type::ARCS2).first == 0x8348C62Fu );

		CHECK ( buffer[14].get(type::ARCS1).first == 0x9F4BF9D9u );
		CHECK ( buffer[14].get(type::ARCS2).first == 0xCE22774Eu );
	}

	// TODO Simulate the same album but with block_size 4096 like ffmpeg does
}

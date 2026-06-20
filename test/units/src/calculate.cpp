#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for calculate.hpp.
 */

#ifndef LIBARCSTK_CALCULATE_HPP_
#include "calculate.hpp"          // TO BE TESTED
#endif

#include <chrono>                 // for steady_clock::now
#include <iterator>               // for begin, cbegin, cend, end
#include <memory>                 // for make_unique, unique_ptr
#include <numeric>                // for iota
#include <type_traits>            // for is_default_constructible,....
#include <unordered_set>          // for unordered_set
#include <utility>                // for move
#include <vector>                 // for vector

#ifndef LIBARCSTK_CHECKSUM_HPP_
#include "checksum.hpp"           // for checksum::type
#endif
#ifndef LIBARCSTK_ALGORITHMS_HPP_
#include "algorithms.hpp"         // for AccurateRipV1V2
#endif
#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"           // for AudioSize, ToC, make_toc, UNIT
#endif
#ifndef LIBARCSTK_SAMPLES_HPP_
#include "samples.hpp"            // for SampleIterator<>
#endif


TEST_CASE ( "Context", "[context] [calc]" )
{
	using arcstk::Context;

	SECTION ( "OR-ing Context works as expected" )
	{
		CHECK ((Context::FIRST_TRACK | Context::TRACK) == Context::FIRST_TRACK);
		CHECK ((Context::LAST_TRACK  | Context::TRACK) == Context::LAST_TRACK);
		CHECK ((Context::ALBUM       | Context::TRACK) == Context::ALBUM);

		CHECK ((Context::FIRST_TRACK | Context::LAST_TRACK)  == Context::ALBUM);
		CHECK ((Context::FIRST_TRACK | Context::ALBUM)       == Context::ALBUM);

		CHECK ((Context::LAST_TRACK  | Context::FIRST_TRACK) == Context::ALBUM);
		CHECK ((Context::LAST_TRACK  | Context::ALBUM)       == Context::ALBUM);

		CHECK ((Context::ALBUM       | Context::LAST_TRACK)  == Context::ALBUM);
		CHECK ((Context::ALBUM       | Context::FIRST_TRACK) == Context::ALBUM);
	}

	SECTION ( "any() is correct" )
	{
		CHECK ( not any(Context::TRACK) );

		CHECK ( any(Context::FIRST_TRACK) );
		CHECK ( any(Context::LAST_TRACK) );
		CHECK ( any(Context::ALBUM) );

		CHECK ( any(Context::FIRST_TRACK | Context::LAST_TRACK) );
	}
}


TEST_CASE ( "Calculation", "[calculation] [calc]" )
{
	using arcstk::AccurateRip::V1;
	using arcstk::AccurateRip::V1andV2;
	using arcstk::Algorithm;
	using arcstk::AudioSize;
	using arcstk::Calculation;
	using arcstk::Context;
	using arcstk::Points;
	using arcstk::Settings;
	using arcstk::ToC;
	using arcstk::UNIT;
	using arcstk::Updater;
	using arcstk::checksum::type;
	using arcstk::make_toc;


	const auto toc = make_toc(
		// leadout
		253038,
		// offsets
		std::vector<int32_t>{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733,
			106333, 139495, 157863, 198495, 213368, 225320, 234103 }
	);

	const auto size      { AudioSize { 253038, UNIT::FRAMES } };

	auto calculation     { Updater<V1andV2>(Settings { Context::ALBUM },
			toc.offsets(), size) };

	const auto algorithm { calculation.algorithm() };

	const auto result    { calculation.result() };

	//

	SECTION ("Default construction is as declared")
	{
		CHECK ( std::is_default_constructible<Updater<V1andV2>>::value );
		CHECK ( not std::is_trivially_default_constructible<Updater<V1andV2>>::value );
		CHECK ( not std::is_nothrow_default_constructible<Updater<V1andV2>>::value );
	}


	SECTION ("Parametized construction is as declared")
	{
		CHECK ( std::is_constructible<Updater<V1andV2>,
				const Settings&,
				const Points&, const AudioSize&>::value
				);

		CHECK ( not std::is_trivially_constructible<Updater<V1andV2>,
				const Settings&,
				const Points&, const AudioSize&>::value
				);

		CHECK ( not std::is_nothrow_constructible<Updater<V1andV2>,
				const Settings&,
				const Points&, const AudioSize&>::value
				);
	}


	SECTION ("Parametized construction is correct")
	{
		CHECK ( calculation.algorithm() == algorithm );
		CHECK ( calculation.algorithm()->types() ==
				std::unordered_set<type> { type::ARCS1, type::ARCS2 } );

		CHECK ( calculation.samples_expected() == 148786344 );

		CHECK ( calculation.samples_processed() == 0 );

		CHECK ( calculation.samples_todo() == 148786344 );
		CHECK ( calculation.samples_todo() == calculation.samples_expected() );

		CHECK ( calculation.update_time_elapsed().count() == 0 );

		CHECK ( not calculation.complete() );

		//CHECK ( result.empty() );
	}


	/*
	SECTION ("Copy construction is as declared")
	{
		CHECK ( std::is_copy_constructible<Calculation<V1andV2>>::value );

		CHECK ( not std::is_nothrow_copy_constructible<Calculation<V1andV2>>::value );
	}


	SECTION ("Copy construction is correct")
	{
		auto c2 { calculation };

		// Algorithm instance is cloned when constructing c2
		CHECK ( c2.algorithm() != calculation.algorithm() );
		CHECK ( c2.algorithm() != algorithm );

		CHECK ( c2.algorithm()->types() ==
				std::unordered_set<type> { type::ARCS1, type::ARCS2 } );

		CHECK ( c2.samples_expected() == 148786344 );

		CHECK ( c2.samples_processed() == 0 );

		CHECK ( c2.samples_todo() == 148786344 );
		CHECK ( c2.samples_todo() == calculation.samples_expected() );

		CHECK ( c2.update_time_elapsed().count() == 0 );

		CHECK ( not c2.complete() );

		CHECK ( c2.result().empty() );
	}
	*/


	SECTION ("Move construction is as declared")
	{
		CHECK ( std::is_move_constructible<Updater<V1andV2>>::value );

		CHECK ( std::is_nothrow_move_constructible<Updater<V1andV2>>::value );
	}


	SECTION ("Move construction is correct")
	{
		auto c3 { std::move(calculation) };

		// Algorithm instance is moved when constructing c2
		CHECK ( c3.algorithm() == algorithm );

		CHECK ( c3.algorithm()->types() ==
				std::unordered_set<type> { type::ARCS1, type::ARCS2 } );

		CHECK ( c3.samples_expected() == 148786344 );

		CHECK ( c3.samples_processed() == 0 );

		CHECK ( c3.samples_todo() == 148786344 );
		CHECK ( c3.samples_todo() == c3.samples_expected() );

		CHECK ( c3.update_time_elapsed().count() == 0 );

		CHECK ( not c3.complete() );

		//CHECK ( c3.result().empty() );
	}


	SECTION ("Instantiating a vector<Calculation> succeeds")
	{
		auto calculations = std::vector<Updater<V1andV2>>();
		calculations.reserve(5);

		CHECK ( calculations.capacity() == 5 );
	}


	SECTION ("Construction with complete ToC succeeds")
	{
		using arcstk::checksum::type;

		const auto toc_1 = make_toc(
			// leadout
			253038,
			// offsets
			std::vector<int32_t>{ 33, 5225, 7390, 23380, 35608, 49820, 69508,
				87733, 106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		);

		auto algorithmV1V2 { std::make_unique<V1andV2>() };

		const auto algo_types = algorithmV1V2->types();

		auto calc { Updater<V1andV2>(Settings { Context::ALBUM }, toc_1) };

		CHECK ( calc.samples_expected()  == 253038 * 588 );
		CHECK ( calc.samples_processed() == 0 );
		CHECK ( calc.samples_todo()      == 253038 * 588 );
		CHECK ( calc.update_time_elapsed().count() == 0 );
		CHECK ( not calc.complete() );
		//CHECK ( calc.result().empty() );

		CHECK ( calc.types() == algo_types );
	}


	SECTION ("Construction with incomplete ToC succeeds")
	{
		using arcstk::checksum::type;

		const auto toc_1 = make_toc(
			// offsets
			std::vector<int32_t>{ 33, 5225, 7390, 23380, 35608, 49820, 69508,
				87733, 106333, 139495, 157863, 198495, 213368, 225320, 234103 }
			//// lengths
			//{ 5192, 2165, 15885, 12228, 13925, 19513, 18155, 18325, 33075,
			//	18368, 40152, 14798, 11952, 8463, -1 /* instead of 18935 */ }
		);

		auto algorithmV1V2 { std::make_unique<V1andV2>() };

		auto c { Updater<V1andV2>(Settings { Context::ALBUM }, toc_1) };

		CHECK ( c.types() ==
				std::unordered_set<type>{ type::ARCS1, type::ARCS2 } );
	}
}


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


TEST_CASE ( "CalculationState", "[calculationstate] [calc]" )
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


TEST_CASE ( "CalculationSet", "[calculationset] [calc]" )
{
	using arcstk::AlgorithmTypes;
	using arcstk::AccurateRip::V1andV2;
	using arcstk::AudioSize;
	using arcstk::UNIT;
	using arcstk::checksum::type;

	using std::cbegin;
	using std::cend;

	std::vector<uint32_t> samples { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };

	using start_type = decltype( samples.cbegin() );
	using stop_type  = decltype( samples.cend() );

	auto calc_set = AlgorithmTypes<V1andV2>::typed_calculationset_for<
		start_type, stop_type>({});

	REQUIRE ( calc_set.size() == 1 );
	REQUIRE ( ! calc_set.complete() );
	REQUIRE ( calc_set.result().empty() );

	calc_set.init({ { 0, UNIT::FRAMES } }, { 10, UNIT::SAMPLES });

	calc_set.update(cbegin(samples),     cbegin(samples) + 4);
	calc_set.update(cbegin(samples) + 5, cbegin(samples) + 9);

	const auto checksums = calc_set.result();
	const auto& track = checksums[0];
	const auto types = track.types();
	const auto [ checksum1, exists1 ] = track.get(type::ARCS1);
	const auto [ checksum2, exists2 ] = track.get(type::ARCS2);

	SECTION ("Result of an instantiated CalculationSet is as expected")
	{
		CHECK ( not checksums.empty() );
		CHECK ( checksums.size() == 1 );

		CHECK ( track.length() == AudioSize { 0, UNIT::SAMPLES } );
		CHECK ( ! track.contains( type::ARCS1 ) );
		CHECK ( ! track.contains( type::ARCS2 ) );

		CHECK ( ! exists1 );
		CHECK ( checksum1.zero() );

		CHECK ( ! exists2 );
		CHECK ( checksum2.zero() );
	}
}


/*
TEST_CASE ( "SampleInputIterator", "[sampleinputiterator]" )
{
	using std::begin;
	using std::end;

	using arcstk::csample_t;
	using arcstk::SampleInputIterator;

	auto object1 = std::vector<csample_t>(100);   // vector with 100 samples
	std::iota (begin(object1), end(object1), 0); // fill with 0, 1, ..., 99.

	auto it = SampleInputIterator { begin(object1) };

	REQUIRE( *it == 0 ); // Dereference operator works
	REQUIRE( *(it+99) == 99 );

	SECTION ( "Prefix increment works" )
	{
		for (unsigned i = 0; i < 99; ++i)
		{
			CHECK ( *it == i );

			++it;
		}
	}

	SECTION ( "Postfix increment works" )
	{
		for (unsigned i = 0; i < 99; ++i)
		{
			CHECK ( *it == i );

			it++;
		}
	}

	SECTION ( "iterator + amount works" )
	{
		const auto it2 = it + 66;
		CHECK ( *it2 == 66 );
	}

	SECTION ( "amount + iterator works" )
	{
		const auto it3 = 48 + it;
		CHECK ( *it3 == 48 );
	}

	SECTION ( "Equality works" )
	{
		auto it2 = SampleInputIterator { begin(object1) };

		CHECK ( it == it2 );

		for (unsigned i = 0; i < 57; ++i) { ++it; }
		for (unsigned i = 0; i < 57; ++i) { ++it2; }

		CHECK ( it == it2 );
	}

	SECTION ( "Equality works for different type iterators" )
	{
		CHECK ( it + 100 == SampleInputIterator { end(object1) } );

		auto it2 = begin(object1);

		CHECK ( it == SampleInputIterator { it2 } );

		for (unsigned i = 0; i < 72; ++i) { ++it;  }
		for (unsigned i = 0; i < 72; ++i) { ++it2; }

		CHECK ( it == SampleInputIterator { it2 } );
	}

	SECTION ( "Assignment operator works" )
	{
		auto it2 = end(object1);

		CHECK ( it != SampleInputIterator { it2 } );

		it = SampleInputIterator { it2 };

		CHECK ( it == SampleInputIterator { it2 } );
	}


	SECTION ( "swap works" )
	{
		it = it + 18;

		REQUIRE ( *it == 18 );

		auto it2 = SampleInputIterator { begin(object1) };
		for (unsigned i = 0; i < 61; ++i) { ++it2; }

		REQUIRE ( *it2 == 61 );

		swap(it, it2);

		CHECK ( *it  == 61 );
		CHECK ( *it2 == 18 );
	}
}


TEST_CASE ( "SampleInputIterator wraps SampleIterator",
		"[sampleinputiterator]" )
{
	using std::begin;
	using std::cbegin;
	using std::cend;
	using std::end;

	//using arcstk::csample_t;
	using arcstk::SampleInputIterator;

	auto seq = std::vector<int32_t>(100);   // vector with 100 samples
	std::iota (begin(seq), end(seq), 0);    // fill with 0, 1, ..., 99.


	SECTION ( "Iteration on interleaved samples works" )
	{
		using sequence_type = arcstk::InterleavedSamples<int32_t>;

		const auto sequence = sequence_type { seq.data(), seq.size(), false };

		auto it = SampleInputIterator { cbegin(sequence) };

		CHECK ( *it == 65536 ); // base 2: 1000 0000 0000 0000
	}


	SECTION ( "Iteration on planar samples works" )
	{
		using sequence_type = arcstk::PlanarSamples<int32_t>;

		const auto sequence = sequence_type { seq.data(),
			seq.data() + seq.size()/2, seq.size()/2, false };

		auto it = SampleInputIterator { cbegin(sequence) };

		CHECK ( *it == 3276800 );
	}
}
*/


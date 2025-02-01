#include "catch2/catch_test_macros.hpp"

/**
 * \file Fixtures for details in module calculate
 */

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"               // for TrackNo
#endif

#ifndef __LIBARCSTK_CALCULATE_HPP__
#include "calculate.hpp"                // for Points
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif


TEST_CASE ( "Interval", "[calculate_details]" )
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

TEST_CASE ( "get_partitioning", "[get_partitioning]" )
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
TEST_CASE ( "Partition", "[partitioner]" )
{
	// TODO Implement
}


TEST_CASE ( "TrackPartitioner", "[trackpartitioner]" )
{
	// TODO Implement
}


TEST_CASE ( "CalculationState", "[calculationstate]" )
{
	// TODO Implement
}
*/


TEST_CASE ( "Counter", "[calculate_details]" )
{
	using arcstk::details::Counter;

	/*
	SECTION ( "increment() is correct" )
	{
		// TODO Implement
	}
	*/
}

/*
TEST_CASE ( "get_offset_sample_indices", "[get_offset_sample_indices]" )
{
	using arcstk::make_toc;

	const auto toc = make_toc(
		// leadout
		253038,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 }
	);

	SECTION ( "is correct" )
	{
		const auto points = arcstk::details::get_offset_sample_indices(*toc);

		CHECK ( points.size() == 15 );

		CHECK ( points[ 0] ==     19404 );
		CHECK ( points[ 1] ==   3072300 );
		CHECK ( points[ 2] ==   4345320 );
		CHECK ( points[ 3] ==  13747440 );
		CHECK ( points[ 4] ==  20937504 );
		CHECK ( points[ 5] ==  29294160 );
		CHECK ( points[ 6] ==  40870704 );
		CHECK ( points[ 7] ==  51587004 );
		CHECK ( points[ 8] ==  62523804 );
		CHECK ( points[ 9] ==  82023060 );
		CHECK ( points[10] ==  92823444 );
		CHECK ( points[11] == 116715060 );
		CHECK ( points[12] == 125460384 );
		CHECK ( points[13] == 132488160 );
		CHECK ( points[14] == 137652564 );
	}
}
*/


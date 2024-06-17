#include "catch2/catch_test_macros.hpp"

/**
 * \file Fixtures for details in module calculate2
 */

#ifndef __LIBARCSTK_IDENTIFIER_HPP__
#include "identifier.hpp"    // for TrackNo
#endif
#ifndef __LIBARCSTK_CALCULATE_DETAILS_HPP__
#include "calculate_details.hpp"
#endif

TEST_CASE ( "Unit conversions", "[calculate_details]" )
{
	SECTION ( "frames2samples" )
	{
		FAIL ("Implement");
	}

	SECTION ( "samples2frames" )
	{
		FAIL ("Implement");
	}

	SECTION ( "frames2bytes" )
	{
		FAIL ("Implement");
	}

	SECTION ( "bytes2frames" )
	{
		FAIL ("Implement");
	}

	SECTION ( "samples2bytes" )
	{
		FAIL ("Implement");
	}

	SECTION ( "bytes2samples" )
	{
		FAIL ("Implement");
	}
}

TEST_CASE ( "TrackNo validity", "[calculate_details]" )
{
	SECTION ( "is_valid_track_number" )
	{
		FAIL ("Implement");
	}

	SECTION ( "is_valid_track" )
	{
		FAIL ("Implement");
	}
}

TEST_CASE ( "Interval", "[calculate_details]" )
{
	using arcstk::details::Interval;

	const auto i1 = Interval<int32_t> {  10,  17 };
	const auto i2 = Interval<int32_t> { -12, 123 };
	const auto i3 = Interval<int32_t> {   1,   2 };

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

TEST_CASE ( "First and Last Relevant Sample", "[calculate_details]" )
{
	SECTION ( "first_relevant_sample" )
	{
		FAIL ("Implement");
	}

	SECTION ( "last_relevant_sample" )
	{
		FAIL ("Implement");
	}
}

TEST_CASE ( "Counter", "[calculate_details]" )
{

}

TEST_CASE ( "Partition", "[calculate_details]" )
{

}


TEST_CASE ( "get_partitioning", "[partitioner]" )
{
	using arcstk::details::TOCBuilder;

	const auto toc = TOCBuilder::build(
			// track count
			15,
			// offsets
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 },
			// leadout
			253038
	);

	SECTION ( "Partitioning with TOC in 1 block is correct" )
	{
		auto p { arcstk::details::get_partitioning(
				{ /* use samples in one block  */    0,    148786344 },
				{ /* use accuraterip algorithm */ 2940, 10000000 - 2939 },
				{ /* use Bach, Organ Concertos, Simon Preston, DGG */
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
				}
		)};

		CHECK ( p.size() == 15 );

		CHECK ( p[ 0].begin_offset()     ==    19404 ); // [i]: offset * 588
		CHECK ( p[ 0].end_offset()       ==  3072300 ); // [i+1]: offset * 588
		CHECK ( p[ 0].first_sample_idx() ==    19404 );
		CHECK ( p[ 0].last_sample_idx()  ==  3072299 ); // == end_offset() - 1
		CHECK ( p[ 0].starts_track()     ==     true );
		CHECK ( p[ 0].ends_track()       ==     true );
		CHECK ( p[ 0].track()            ==        1 );
		CHECK ( p[ 0].size()             ==  3052896 ); // end - begin offset

		CHECK ( p[ 1].begin_offset()     ==  3072300 ); // == 0:end_offset()
		CHECK ( p[ 1].end_offset()       ==  4345320 );
		CHECK ( p[ 1].first_sample_idx() ==  3072300 );
		CHECK ( p[ 1].last_sample_idx()  ==  4345319 );
		CHECK ( p[ 1].starts_track()     ==     true );
		CHECK ( p[ 1].ends_track()       ==     true );
		CHECK ( p[ 1].track()            ==        2 );
		CHECK ( p[ 1].size()             ==  1273020 );

		CHECK ( p[ 2].begin_offset()     ==   4345320 );
		CHECK ( p[ 2].end_offset()       ==  13747440 );
		CHECK ( p[ 2].first_sample_idx() ==   4345320 );
		CHECK ( p[ 2].last_sample_idx()  ==  13747439 );
		CHECK ( p[ 2].starts_track()     ==      true );
		CHECK ( p[ 2].ends_track()       ==      true );
		CHECK ( p[ 2].track()            ==         3 );
		CHECK ( p[ 2].size()             ==   9402120 );

		CHECK ( p[ 3].begin_offset()     ==  13747440 );
		CHECK ( p[ 3].end_offset()       ==  20937504 );
		CHECK ( p[ 3].first_sample_idx() ==  13747440 );
		CHECK ( p[ 3].last_sample_idx()  ==  20937503 );
		CHECK ( p[ 3].starts_track()     ==      true );
		CHECK ( p[ 3].ends_track()       ==      true );
		CHECK ( p[ 3].track()            ==         4 );
		CHECK ( p[ 3].size()             ==   7190064 );

		CHECK ( p[ 4].begin_offset()     ==  20937504 );
		CHECK ( p[ 4].end_offset()       ==  29294160 );
		CHECK ( p[ 4].first_sample_idx() ==  20937504 );
		CHECK ( p[ 4].last_sample_idx()  ==  29294159 );
		CHECK ( p[ 4].starts_track()     ==      true );
		CHECK ( p[ 4].ends_track()       ==      true );
		CHECK ( p[ 4].track()            ==         5 );
		CHECK ( p[ 4].size()             ==   8356656 );

		CHECK ( p[ 5].begin_offset()     ==  29294160 );
		CHECK ( p[ 5].end_offset()       ==  40870704 );
		CHECK ( p[ 5].first_sample_idx() ==  29294160 );
		CHECK ( p[ 5].last_sample_idx()  ==  40870703 );
		CHECK ( p[ 5].starts_track()     ==      true );
		CHECK ( p[ 5].ends_track()       ==      true );
		CHECK ( p[ 5].track()            ==         6 );
		CHECK ( p[ 5].size()             ==  11576544 );

		CHECK ( p[ 6].begin_offset()     ==  40870704 );
		CHECK ( p[ 6].end_offset()       ==  51587004 );
		CHECK ( p[ 6].first_sample_idx() ==  40870704 );
		CHECK ( p[ 6].last_sample_idx()  ==  51587003 );
		CHECK ( p[ 6].starts_track()     ==      true );
		CHECK ( p[ 6].ends_track()       ==      true );
		CHECK ( p[ 6].track()            ==         7 );
		CHECK ( p[ 6].size()             ==  10716300 );

		CHECK ( p[ 7].begin_offset()     ==  51587004 );
		CHECK ( p[ 7].end_offset()       ==  62523804 );
		CHECK ( p[ 7].first_sample_idx() ==  51587004 );
		CHECK ( p[ 7].last_sample_idx()  ==  62523803 );
		CHECK ( p[ 7].starts_track()     ==      true );
		CHECK ( p[ 7].ends_track()       ==      true );
		CHECK ( p[ 7].track()            ==         8 );
		CHECK ( p[ 7].size()             ==  10936800 );

		CHECK ( p[ 8].begin_offset()     ==  62523804 );
		CHECK ( p[ 8].end_offset()       ==  82023060 );
		CHECK ( p[ 8].first_sample_idx() ==  62523804 );
		CHECK ( p[ 8].last_sample_idx()  ==  82023059 );
		CHECK ( p[ 8].starts_track()     ==      true );
		CHECK ( p[ 8].ends_track()       ==      true );
		CHECK ( p[ 8].track()            ==         9 );
		CHECK ( p[ 8].size()             ==  19499256 );

		CHECK ( p[ 9].begin_offset()     ==  82023060 );
		CHECK ( p[ 9].end_offset()       ==  92823444 );
		CHECK ( p[ 9].first_sample_idx() ==  82023060 );
		CHECK ( p[ 9].last_sample_idx()  ==  92823443 );
		CHECK ( p[ 9].starts_track()     ==      true );
		CHECK ( p[ 9].ends_track()       ==      true );
		CHECK ( p[ 9].track()            ==        10 );
		CHECK ( p[ 9].size()             ==  10800384 );

		CHECK ( p[10].begin_offset()     ==  92823444 );
		CHECK ( p[10].end_offset()       == 116715060 );
		CHECK ( p[10].first_sample_idx() ==  92823444 );
		CHECK ( p[10].last_sample_idx()  == 116715059 );
		CHECK ( p[10].starts_track()     ==      true );
		CHECK ( p[10].ends_track()       ==      true );
		CHECK ( p[10].track()            ==        11 );
		CHECK ( p[10].size()             ==  23891616 );

		CHECK ( p[11].begin_offset()     == 116715060 );
		CHECK ( p[11].end_offset()       == 125460384 );
		CHECK ( p[11].first_sample_idx() == 116715060 );
		CHECK ( p[11].last_sample_idx()  == 125460383 );
		CHECK ( p[11].starts_track()     ==      true );
		CHECK ( p[11].ends_track()       ==      true );
		CHECK ( p[11].track()            ==        12 );
		CHECK ( p[11].size()             ==   8745324 );

		CHECK ( p[12].begin_offset()     == 125460384 );
		CHECK ( p[12].end_offset()       == 132488160 );
		CHECK ( p[12].first_sample_idx() == 125460384 );
		CHECK ( p[12].last_sample_idx()  == 132488159 );
		CHECK ( p[12].starts_track()     ==      true );
		CHECK ( p[12].ends_track()       ==      true );
		CHECK ( p[12].track()            ==        13 );
		CHECK ( p[12].size()             ==   7027776 );

		CHECK ( p[13].begin_offset()     == 132488160 );
		CHECK ( p[13].end_offset()       == 137652564 );
		CHECK ( p[13].first_sample_idx() == 132488160 );
		CHECK ( p[13].last_sample_idx()  == 137652563 );
		CHECK ( p[13].starts_track()     ==      true );
		CHECK ( p[13].ends_track()       ==      true );
		CHECK ( p[13].track()            ==        14 );
		CHECK ( p[13].size()             ==   5164404 );

		CHECK ( p[14].begin_offset()     == 137652564 );
		CHECK ( p[14].end_offset()       == 148786344 );
		CHECK ( p[14].first_sample_idx() == 137652564 );
		CHECK ( p[14].last_sample_idx()  == 148786343 );
		CHECK ( p[14].starts_track()     ==      true );
		CHECK ( p[14].ends_track()       ==      true );
		CHECK ( p[14].track()            ==        15 );
		CHECK ( p[14].size()             ==  11133780 );
	}
}


TEST_CASE ( "TrackPartitioner", "[calculate_details]" )
{
	using arcstk::details::TOCBuilder;

	// "Bach: Organ Concertos", Simon Preston, DGG
	const auto toc0 = TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
		// no filenames
	);

	using arcstk::details::get_partitioning;

	//const auto t = arcstk::details::TrackPartitioner { 148786344, 2940, 2939 };


	SECTION ( "create_partitioning() with TOC is correct" )
	{
        // total in Bach, Organ Concertos: 148786344
		auto p { get_partitioning( { 0, 10000000 },
				{ 2940, 10000000 - 2939 },
				{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333,
					139495, 157863, 198495, 213368, 225320, 234103 }
				)};

		CHECK ( p.size() == 15 );

		CHECK ( p[0].begin_offset() == 2940 );
		CHECK ( p[0].end_offset()   == 5224 * 588 );

		CHECK ( p[0].first_sample_idx() == 2940 + 33 -1 );
		CHECK ( p[0].last_sample_idx()  == 5224 * 588 );

		CHECK ( p[0].starts_track() );
		CHECK ( !p[0].ends_track() );

		CHECK ( p[0].track() == 0 );
		CHECK ( p[0].size()  == 5225 - 33 - 2940 );

		CHECK ( p[14].last_sample_idx()  == 10000000 - 2939 - 1 );
	}

	SECTION ( "create_partitioning() without TOC is correct" )
	{
		auto p { get_partitioning( { 0, 10000 }, { 2940, 10000 - 2939 }, {} )};
		//                         /* offset, block size, skip front + back */

		CHECK ( p.size() == 1 );

		CHECK ( p[0].begin_offset() == 2940 );
		CHECK ( p[0].end_offset()   == 7062 );

		CHECK ( p[0].first_sample_idx() == 2940 );
		CHECK ( p[0].last_sample_idx()  == 7061 );

		CHECK ( p[0].starts_track() );
		CHECK ( p[0].ends_track() );

		CHECK ( p[0].track() == 0 );
		CHECK ( p[0].size()  == 4122 );
	}
}

TEST_CASE ( "CalculationState", "[calculate_details]" )
{

}


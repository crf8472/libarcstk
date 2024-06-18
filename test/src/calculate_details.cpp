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
	using arcstk::details::frames2samples;
	using arcstk::details::samples2frames;
	using arcstk::details::bytes2samples;
	using arcstk::details::samples2bytes;
	using arcstk::details::frames2bytes;
	using arcstk::details::bytes2frames;

	SECTION ( "frames2samples() is correct" )
	{
		CHECK ( frames2samples(-2) == -1176 );
		CHECK ( frames2samples(-1) ==  -588 );
		CHECK ( frames2samples( 0) ==     0 );
		CHECK ( frames2samples( 1) ==   588 );
		CHECK ( frames2samples( 2) ==  1176 );

		CHECK ( frames2samples(253038) == 148786344 );
	}

	SECTION ( "samples2frames() is correct" )
	{
		CHECK ( samples2frames(-588) == -1 );
		CHECK ( samples2frames(-587) ==  0 );
		CHECK ( samples2frames(-586) ==  0 );
		CHECK ( samples2frames(-585) ==  0 );
		//...
		CHECK ( samples2frames(  -2) ==  0 );
		CHECK ( samples2frames(  -1) ==  0 );
		CHECK ( samples2frames(   0) ==  0 );
		CHECK ( samples2frames(   1) ==  0 );
		CHECK ( samples2frames(   2) ==  0 );
		// ...
		CHECK ( samples2frames( 585) ==  0 );
		CHECK ( samples2frames( 586) ==  0 );
		CHECK ( samples2frames( 587) ==  0 );

		// every i : -588 < i < 588 will be 0

		CHECK ( samples2frames(588)  == 1 );
		CHECK ( samples2frames(589)  == 1 );
		CHECK ( samples2frames(590)  == 1 );

		CHECK ( samples2frames(1176) == 2 );

		CHECK ( samples2frames(148786344) == 253038 );
	}

	SECTION ( "samples2bytes() is correct" )
	{
		CHECK ( samples2bytes(-2)   == -8 );
		CHECK ( samples2bytes(-1)   == -4 );
		CHECK ( samples2bytes( 0)   ==  0 );
		CHECK ( samples2bytes( 1)   ==  4 );
		CHECK ( samples2bytes( 2)   ==  8 );

		CHECK ( samples2bytes(586)  == 2344 );
		CHECK ( samples2bytes(587)  == 2348 );
		CHECK ( samples2bytes(588)  == 2352 );

		CHECK ( samples2bytes(1176) == 4704 );

		CHECK ( samples2bytes(148786344) == 595145376 );
	}

	SECTION ( "bytes2samples() is correct" )
	{
		CHECK ( bytes2samples(-5)   == -1 );
		CHECK ( bytes2samples(-4)   == -1 );
		CHECK ( bytes2samples(-3)   ==  0 );
		CHECK ( bytes2samples(-2)   ==  0 );
		CHECK ( bytes2samples(-1)   ==  0 );
		CHECK ( bytes2samples( 0)   ==  0 );
		CHECK ( bytes2samples( 1)   ==  0 );
		CHECK ( bytes2samples( 2)   ==  0 );
		CHECK ( bytes2samples( 3)   ==  0 );
		CHECK ( bytes2samples( 4)   ==  1 );
		CHECK ( bytes2samples( 5)   ==  1 );
		CHECK ( bytes2samples( 6)   ==  1 );
		CHECK ( bytes2samples( 7)   ==  1 );
		CHECK ( bytes2samples( 8)   ==  2 );
		CHECK ( bytes2samples( 9)   ==  2 );

		// every i : -4 < i < 4 will be 0

		CHECK ( bytes2samples(2344) == 586 );
		CHECK ( bytes2samples(2348) == 587 );
		CHECK ( bytes2samples(2352) == 588 );

		CHECK ( bytes2samples(4704) == 1176 );

		CHECK ( bytes2samples(595145376) == 148786344 );
	}

	SECTION ( "frames2bytes() is correct" )
	{
		CHECK ( frames2bytes( -2)  == -4704 );
		CHECK ( frames2bytes( -1)  == -2352 );
		CHECK ( frames2bytes(  0)  ==     0 );
		CHECK ( frames2bytes(  1)  ==  2352 );
		CHECK ( frames2bytes(  2)  ==  4704 );

		CHECK ( frames2bytes(253038) == 595145376 );
	}

	SECTION ( "bytes2frames() is correct" )
	{
		CHECK ( bytes2frames( 2352)   ==  1 );
		CHECK ( bytes2frames( 2351)   ==  0 );
		// ...
		CHECK ( bytes2frames(    0)   ==  0 );
		CHECK ( bytes2frames(    1)   ==  0 );
		CHECK ( bytes2frames(    2)   ==  0 );
		// ...
		CHECK ( bytes2frames( 2351)   ==  0 );
		CHECK ( bytes2frames( 2352)   ==  1 );

		CHECK ( bytes2frames(595145376) == 253038 );
	}
}


TEST_CASE ( "get_offset_sample_indices", "[get_offset_sample_indices]" )
{
	const auto toc = arcstk::details::TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
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


TEST_CASE ( "is_valid_track_number()", "[is_valid_track_number]" )
{
	using arcstk::details::is_valid_track_number;
	using arcstk::TrackNo;

	SECTION ( "is correct" )
	{
		for (TrackNo t = -1; t <= -99; --t)
		{
			CHECK ( ! is_valid_track_number(t) );
		}

		CHECK ( ! is_valid_track_number(0) );

		// Only values in interval [1,99] are valid, everything else is invalid
		for (TrackNo t = 1; t <= 99; ++t)
		{
			CHECK ( is_valid_track_number(t) );
		}

		CHECK ( ! is_valid_track_number(100) );
		CHECK ( ! is_valid_track_number(101) );
		CHECK ( ! is_valid_track_number(102) );

		CHECK ( ! is_valid_track_number(500) );

		CHECK ( ! is_valid_track_number(1000) );
	}
}


TEST_CASE ( "is_valid_track()", "[is_valid_track]" )
{
	using arcstk::details::is_valid_track;
	using arcstk::TrackNo;

	const auto toc = arcstk::details::TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	);

	SECTION ( "is correct" )
	{
		for (TrackNo t = 1; t <= 15; ++t)
		{
			CHECK ( is_valid_track(t, *toc) );
		}

		for (TrackNo t = 16; t <= 99; ++t)
		{
			CHECK ( ! is_valid_track(t, *toc) );
		}
	}
}


TEST_CASE ( "track()", "[track]" )
{
	using arcstk::details::is_valid_track;
	using arcstk::details::track;
	using arcstk::TrackNo;

	const auto toc = arcstk::details::TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	);

	SECTION ( "is correct" )
	{
		CHECK ( ! is_valid_track(track(         0, *toc, 148786344), *toc) );

		CHECK ( ! is_valid_track(track(     19403, *toc, 148786344), *toc) );
		CHECK (  0 ==            track(     19403, *toc, 148786344)  );
		CHECK (  1 ==            track(     19404, *toc, 148786344)  );
		CHECK (  1 ==            track(     19405, *toc, 148786344)  );

		CHECK (  1 ==            track(   3072299, *toc, 148786344)  );
		CHECK (  2 ==            track(   3072300, *toc, 148786344)  );
		CHECK (  2 ==            track(   3072301, *toc, 148786344)  );

		CHECK (  2 ==            track(   4345319, *toc, 148786344)  );
		CHECK (  3 ==            track(   4345320, *toc, 148786344)  );
		CHECK (  3 ==            track(   4345321, *toc, 148786344)  );

		CHECK (  3 ==            track(  13747439, *toc, 148786344)  );
		CHECK (  4 ==            track(  13747440, *toc, 148786344)  );
		CHECK (  4 ==            track(  13747441, *toc, 148786344)  );

		CHECK (  4 ==            track(  20937503, *toc, 148786344)  );
		CHECK (  5 ==            track(  20937504, *toc, 148786344)  );
		CHECK (  5 ==            track(  20937505, *toc, 148786344)  );

		CHECK (  5 ==            track(  29294159, *toc, 148786344)  );
		CHECK (  6 ==            track(  29294160, *toc, 148786344)  );
		CHECK (  6 ==            track(  29294161, *toc, 148786344)  );

		CHECK (  6 ==            track(  40870703, *toc, 148786344)  );
		CHECK (  7 ==            track(  40870704, *toc, 148786344)  );
		CHECK (  7 ==            track(  40870705, *toc, 148786344)  );

		CHECK (  7 ==            track(  51587003, *toc, 148786344)  );
		CHECK (  8 ==            track(  51587004, *toc, 148786344)  );
		CHECK (  8 ==            track(  51587005, *toc, 148786344)  );

		CHECK (  8 ==            track(  62523803, *toc, 148786344)  );
		CHECK (  9 ==            track(  62523804, *toc, 148786344)  );
		CHECK (  9 ==            track(  62523805, *toc, 148786344)  );

		CHECK (  9 ==            track(  82023059, *toc, 148786344)  );
		CHECK ( 10 ==            track(  82023060, *toc, 148786344)  );
		CHECK ( 10 ==            track(  82023061, *toc, 148786344)  );

		CHECK ( 10 ==            track(  92823443, *toc, 148786344)  );
		CHECK ( 11 ==            track(  92823444, *toc, 148786344)  );
		CHECK ( 11 ==            track(  92823445, *toc, 148786344)  );

		CHECK ( 11 ==            track( 116715059, *toc, 148786344)  );
		CHECK ( 12 ==            track( 116715060, *toc, 148786344)  );
		CHECK ( 12 ==            track( 116715061, *toc, 148786344)  );

		CHECK ( 12 ==            track( 125460383, *toc, 148786344)  );
		CHECK ( 13 ==            track( 125460384, *toc, 148786344)  );
		CHECK ( 13 ==            track( 125460385, *toc, 148786344)  );

		CHECK ( 13 ==            track( 132488159, *toc, 148786344)  );
		CHECK ( 14 ==            track( 132488160, *toc, 148786344)  );
		CHECK ( 14 ==            track( 132488161, *toc, 148786344)  );

		CHECK ( 14 ==            track( 137652563, *toc, 148786344)  );
		CHECK ( 15 ==            track( 137652564, *toc, 148786344)  );
		CHECK ( 15 ==            track( 137652565, *toc, 148786344)  );

		CHECK ( 15 ==            track( 148786343, *toc, 148786344)  );
		CHECK ( 15 ==            track( 148786344, *toc, 148786344)  );

		CHECK ( ! is_valid_track(track( 148786345, *toc, 148786344), *toc) );
	}
}


TEST_CASE ( "first_relevant_sample()", "[first_relevant_sample]" )
{
	using arcstk::details::first_relevant_sample;

	const auto toc = arcstk::details::TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	);

	// Define the AccurateRip interval with excluding the first 5 frames
	const auto i {
		arcstk::details::Interval<int32_t>( 2940, 253038 * 588 - 2939 )};

	SECTION ( "is correct" )
	{
		// The first relevant sample is the first physical sample with the
		// exception of the first track, where 5 frames are excluded.

		CHECK (2940 + 33 * 588 == first_relevant_sample( 1, *toc, i) );
		CHECK (     5225 * 588 == first_relevant_sample( 2, *toc, i) );
		CHECK (     7390 * 588 == first_relevant_sample( 3, *toc, i) );
		CHECK (    23380 * 588 == first_relevant_sample( 4, *toc, i) );
		CHECK (    35608 * 588 == first_relevant_sample( 5, *toc, i) );
		CHECK (    49820 * 588 == first_relevant_sample( 6, *toc, i) );
		CHECK (    69508 * 588 == first_relevant_sample( 7, *toc, i) );
		CHECK (    87733 * 588 == first_relevant_sample( 8, *toc, i) );
		CHECK (   106333 * 588 == first_relevant_sample( 9, *toc, i) );
		CHECK (   139495 * 588 == first_relevant_sample(10, *toc, i) );
		CHECK (   157863 * 588 == first_relevant_sample(11, *toc, i) );
		CHECK (   198495 * 588 == first_relevant_sample(12, *toc, i) );
		CHECK (   213368 * 588 == first_relevant_sample(13, *toc, i) );
		CHECK (   225320 * 588 == first_relevant_sample(14, *toc, i) );
		CHECK (   234103 * 588 == first_relevant_sample(15, *toc, i) );
	}
}


TEST_CASE ( "last_relevant_sample()",
		"[last_relevant_sample] [last_in_bounds]" )
{
	using arcstk::details::last_relevant_sample;

	const auto toc = arcstk::details::TOCBuilder::build(
		// track count
		15,
		// offsets
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 },
		// leadout
		253038
	);

	// Define the AccurateRip interval with excluding the first 5 frames
	const auto i {
		arcstk::details::Interval<int32_t>( 2940, 253038 * 588 - 2939 )};

	SECTION ( "is correct" )
	{
		// The last relevant sample is the last physical sample with the
		// exception of the last track, where 2939 samples are excluded.

		CHECK (  5225 * 588 -    1 == last_relevant_sample( 1, *toc, i) );
		CHECK (  7390 * 588 -    1 == last_relevant_sample( 2, *toc, i) );
		CHECK ( 23380 * 588 -    1 == last_relevant_sample( 3, *toc, i) );
		CHECK ( 35608 * 588 -    1 == last_relevant_sample( 4, *toc, i) );
		CHECK ( 49820 * 588 -    1 == last_relevant_sample( 5, *toc, i) );
		CHECK ( 69508 * 588 -    1 == last_relevant_sample( 6, *toc, i) );
		CHECK ( 87733 * 588 -    1 == last_relevant_sample( 7, *toc, i) );
		CHECK (106333 * 588 -    1 == last_relevant_sample( 8, *toc, i) );
		CHECK (139495 * 588 -    1 == last_relevant_sample( 9, *toc, i) );
		CHECK (157863 * 588 -    1 == last_relevant_sample(10, *toc, i) );
		CHECK (198495 * 588 -    1 == last_relevant_sample(11, *toc, i) );
		CHECK (213368 * 588 -    1 == last_relevant_sample(12, *toc, i) );
		CHECK (225320 * 588 -    1 == last_relevant_sample(13, *toc, i) );
		CHECK (234103 * 588 -    1 == last_relevant_sample(14, *toc, i) );
		CHECK (253038 * 588 - 2939 == last_relevant_sample(15, *toc, i) );
	}

	SECTION ( "last_in_bounds() is correct" )
	{
		CHECK ( last_in_bounds(i,   5225 * 588 - 1   ) ==   5225 * 588 - 1    );
		CHECK ( last_in_bounds(i,   7390 * 588 - 1   ) ==   7390 * 588 - 1    );
		CHECK ( last_in_bounds(i,  23380 * 588 - 1   ) ==  23380 * 588 - 1    );
		CHECK ( last_in_bounds(i,  35608 * 588 - 1   ) ==  35608 * 588 - 1    );
		CHECK ( last_in_bounds(i,  49820 * 588 - 1   ) ==  49820 * 588 - 1    );
		CHECK ( last_in_bounds(i,  69508 * 588 - 1   ) ==  69508 * 588 - 1    );
		CHECK ( last_in_bounds(i,  87733 * 588 - 1   ) ==  87733 * 588 - 1    );
		CHECK ( last_in_bounds(i, 106333 * 588 - 1   ) == 106333 * 588 - 1    );
		CHECK ( last_in_bounds(i, 139495 * 588 - 1   ) == 139495 * 588 - 1    );
		CHECK ( last_in_bounds(i, 157863 * 588 - 1   ) == 157863 * 588 - 1    );
		CHECK ( last_in_bounds(i, 198495 * 588 - 1   ) == 198495 * 588 - 1    );
		CHECK ( last_in_bounds(i, 213368 * 588 - 1   ) == 213368 * 588 - 1    );
		CHECK ( last_in_bounds(i, 225320 * 588 - 1   ) == 225320 * 588 - 1    );
		CHECK ( last_in_bounds(i, 234103 * 588 - 1   ) == 234103 * 588 - 1    );
		CHECK ( last_in_bounds(i, 253038 * 588 - 2939) == 253038 * 588 - 2939 );
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
	// leadout 253038


	SECTION ( "Partitioning with TOC in 1 block is correct" )
	{
		auto p { arcstk::details::get_partitioning(
				{ /* use samples in one block  */    0, 148786344 },
				{ /* use accuraterip algorithm */ 2940, 148786344 - 2939 },
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
		CHECK ( p[ 0].starts_track()     ==     true );
		CHECK ( p[ 0].ends_track()       ==     true );
		CHECK ( p[ 0].track()            ==        1 );
		CHECK ( p[ 0].size()             ==  3052896 ); // end - begin offset

		CHECK ( p[ 1].begin_offset()     ==  3072300 ); // == 0:end_offset()
		CHECK ( p[ 1].end_offset()       ==  4345320 );
		CHECK ( p[ 1].starts_track()     ==     true );
		CHECK ( p[ 1].ends_track()       ==     true );
		CHECK ( p[ 1].track()            ==        2 );
		CHECK ( p[ 1].size()             ==  1273020 );

		CHECK ( p[ 2].begin_offset()     ==   4345320 );
		CHECK ( p[ 2].end_offset()       ==  13747440 );
		CHECK ( p[ 2].starts_track()     ==      true );
		CHECK ( p[ 2].ends_track()       ==      true );
		CHECK ( p[ 2].track()            ==         3 );
		CHECK ( p[ 2].size()             ==   9402120 );

		CHECK ( p[ 3].begin_offset()     ==  13747440 );
		CHECK ( p[ 3].end_offset()       ==  20937504 );
		CHECK ( p[ 3].starts_track()     ==      true );
		CHECK ( p[ 3].ends_track()       ==      true );
		CHECK ( p[ 3].track()            ==         4 );
		CHECK ( p[ 3].size()             ==   7190064 );

		CHECK ( p[ 4].begin_offset()     ==  20937504 );
		CHECK ( p[ 4].end_offset()       ==  29294160 );
		CHECK ( p[ 4].starts_track()     ==      true );
		CHECK ( p[ 4].ends_track()       ==      true );
		CHECK ( p[ 4].track()            ==         5 );
		CHECK ( p[ 4].size()             ==   8356656 );

		CHECK ( p[ 5].begin_offset()     ==  29294160 );
		CHECK ( p[ 5].end_offset()       ==  40870704 );
		CHECK ( p[ 5].starts_track()     ==      true );
		CHECK ( p[ 5].ends_track()       ==      true );
		CHECK ( p[ 5].track()            ==         6 );
		CHECK ( p[ 5].size()             ==  11576544 );

		CHECK ( p[ 6].begin_offset()     ==  40870704 );
		CHECK ( p[ 6].end_offset()       ==  51587004 );
		CHECK ( p[ 6].starts_track()     ==      true );
		CHECK ( p[ 6].ends_track()       ==      true );
		CHECK ( p[ 6].track()            ==         7 );
		CHECK ( p[ 6].size()             ==  10716300 );

		CHECK ( p[ 7].begin_offset()     ==  51587004 );
		CHECK ( p[ 7].end_offset()       ==  62523804 );
		CHECK ( p[ 7].starts_track()     ==      true );
		CHECK ( p[ 7].ends_track()       ==      true );
		CHECK ( p[ 7].track()            ==         8 );
		CHECK ( p[ 7].size()             ==  10936800 );

		CHECK ( p[ 8].begin_offset()     ==  62523804 );
		CHECK ( p[ 8].end_offset()       ==  82023060 );
		CHECK ( p[ 8].starts_track()     ==      true );
		CHECK ( p[ 8].ends_track()       ==      true );
		CHECK ( p[ 8].track()            ==         9 );
		CHECK ( p[ 8].size()             ==  19499256 );

		CHECK ( p[ 9].begin_offset()     ==  82023060 );
		CHECK ( p[ 9].end_offset()       ==  92823444 );
		CHECK ( p[ 9].starts_track()     ==      true );
		CHECK ( p[ 9].ends_track()       ==      true );
		CHECK ( p[ 9].track()            ==        10 );
		CHECK ( p[ 9].size()             ==  10800384 );

		CHECK ( p[10].begin_offset()     ==  92823444 );
		CHECK ( p[10].end_offset()       == 116715060 );
		CHECK ( p[10].starts_track()     ==      true );
		CHECK ( p[10].ends_track()       ==      true );
		CHECK ( p[10].track()            ==        11 );
		CHECK ( p[10].size()             ==  23891616 );

		CHECK ( p[11].begin_offset()     == 116715060 );
		CHECK ( p[11].end_offset()       == 125460384 );
		CHECK ( p[11].starts_track()     ==      true );
		CHECK ( p[11].ends_track()       ==      true );
		CHECK ( p[11].track()            ==        12 );
		CHECK ( p[11].size()             ==   8745324 );

		CHECK ( p[12].begin_offset()     == 125460384 );
		CHECK ( p[12].end_offset()       == 132488160 );
		CHECK ( p[12].starts_track()     ==      true );
		CHECK ( p[12].ends_track()       ==      true );
		CHECK ( p[12].track()            ==        13 );
		CHECK ( p[12].size()             ==   7027776 );

		CHECK ( p[13].begin_offset()     == 132488160 );
		CHECK ( p[13].end_offset()       == 137652564 );
		CHECK ( p[13].starts_track()     ==      true );
		CHECK ( p[13].ends_track()       ==      true );
		CHECK ( p[13].track()            ==        14 );
		CHECK ( p[13].size()             ==   5164404 );

		CHECK ( p[14].begin_offset()     == 137652564 );
		CHECK ( p[14].end_offset()       == 148786344 ); // 148783405
		CHECK ( p[14].starts_track()     ==      true );
		CHECK ( p[14].ends_track()       ==      true );
		CHECK ( p[14].track()            ==        15 );
		CHECK ( p[14].size()             ==  11133780 ); // 11130842
	}

	SECTION ( "Partitioning with TOC: first block is correct" )
	{
		auto p { arcstk::details::get_partitioning(
				{ /* use samples in one block  */    0, 29000000 },
				{ /* use accuraterip algorithm */ 2940, 148786344 - 2939 },
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

		CHECK ( p.size() == 5 );

		CHECK ( p[ 0].begin_offset()     ==    19404 ); // [i]: offset * 588
		CHECK ( p[ 0].end_offset()       ==  3072300 ); // [i+1]: offset * 588
		CHECK ( p[ 0].starts_track()     ==     true );
		CHECK ( p[ 0].ends_track()       ==     true );
		CHECK ( p[ 0].track()            ==        1 );
		CHECK ( p[ 0].size()             ==  3052896 ); // end - begin offset

		CHECK ( p[ 1].begin_offset()     ==  3072300 ); // == 0:end_offset()
		CHECK ( p[ 1].end_offset()       ==  4345320 );
		CHECK ( p[ 1].starts_track()     ==     true );
		CHECK ( p[ 1].ends_track()       ==     true );
		CHECK ( p[ 1].track()            ==        2 );
		CHECK ( p[ 1].size()             ==  1273020 );

		CHECK ( p[ 2].begin_offset()     ==   4345320 );
		CHECK ( p[ 2].end_offset()       ==  13747440 );
		CHECK ( p[ 2].starts_track()     ==      true );
		CHECK ( p[ 2].ends_track()       ==      true );
		CHECK ( p[ 2].track()            ==         3 );
		CHECK ( p[ 2].size()             ==   9402120 );

		CHECK ( p[ 3].begin_offset()     ==  13747440 );
		CHECK ( p[ 3].end_offset()       ==  20937504 );
		CHECK ( p[ 3].starts_track()     ==      true );
		CHECK ( p[ 3].ends_track()       ==      true );
		CHECK ( p[ 3].track()            ==         4 );
		CHECK ( p[ 3].size()             ==   7190064 );

		CHECK ( p[ 4].begin_offset()     ==  20937504 );
		CHECK ( p[ 4].end_offset()       ==  29000000 );
		CHECK ( p[ 4].starts_track()     ==      true );
		CHECK ( p[ 4].ends_track()       ==     false );
		CHECK ( p[ 4].track()            ==         5 );
		CHECK ( p[ 4].size()             ==   8062496 );
	}

	SECTION ( "Partitioning with TOC: last block is correct" )
	{
		auto p { arcstk::details::get_partitioning(
				{ /* use samples in one block  */  120000000, 148786344 },
				{ /* use accuraterip algorithm */ 2940, 148786344 - 2939 },
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

		CHECK ( p.size() == 4 );

		CHECK ( p[ 0].begin_offset()     == 120000000 );
		CHECK ( p[ 0].end_offset()       == 125460384 );
		CHECK ( p[ 0].starts_track()     ==     false );
		CHECK ( p[ 0].ends_track()       ==      true );
		CHECK ( p[ 0].track()            ==        12 );
		CHECK ( p[ 0].size()             ==   5460384 );

		CHECK ( p[ 1].begin_offset()     == 125460384 );
		CHECK ( p[ 1].end_offset()       == 132488160 );
		CHECK ( p[ 1].starts_track()     ==      true );
		CHECK ( p[ 1].ends_track()       ==      true );
		CHECK ( p[ 1].track()            ==        13 );
		CHECK ( p[ 1].size()             ==   7027776 );

		CHECK ( p[ 2].begin_offset()     == 132488160 );
		CHECK ( p[ 2].end_offset()       == 137652564 );
		CHECK ( p[ 2].starts_track()     ==      true );
		CHECK ( p[ 2].ends_track()       ==      true );
		CHECK ( p[ 2].track()            ==        14 );
		CHECK ( p[ 2].size()             ==   5164404 );

		CHECK ( p[ 3].begin_offset()     == 137652564 );
		CHECK ( p[ 3].end_offset()       == 148786344 );
		CHECK ( p[ 3].starts_track()     ==      true );
		CHECK ( p[ 3].ends_track()       ==      true );
		CHECK ( p[ 3].track()            ==        15 );
		CHECK ( p[ 3].size()             ==  11133780 );
	}
}


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


TEST_CASE ( "Counter", "[calculate_details]" )
{
	using arcstk::details::Counter;

	SECTION ( "increment() is correct" )
	{
		// TODO Implement
	}
}


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


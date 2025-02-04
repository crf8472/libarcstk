#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for metadata.hpp
 */

#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"           // for AudioSize, UNIT
#endif
#ifndef __LIBARCSTK_METADATA_CONV_HPP__
#include "metadata_conv.hpp"      // TO BE TESTED
#endif


TEST_CASE ( "convert<>()", "[convert] [meta]" )
{
	using arcstk::AudioSize;
	using arcstk::details::convert;
	using arcstk::UNIT;

	// 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
	//	157863, 198495, 213368, 225320, 234103

	SECTION ( "converts frames to bytes correctly" )
	{
		CHECK ( convert<UNIT::FRAMES, UNIT::BYTES>(5225) == 12289200 );
	}

	SECTION ( "converts frames to samples correctly" )
	{
		CHECK ( convert<UNIT::FRAMES, UNIT::SAMPLES>(  5225) ==   3072300 );
		CHECK ( convert<UNIT::FRAMES, UNIT::SAMPLES>(253038) == 148786344 );
	}

	SECTION ( "converts bytes to frames correctly" )
	{
		CHECK ( convert<UNIT::BYTES, UNIT::FRAMES>(12289200) == 5225 );
	}

	SECTION ( "converts bytes to samples correctly" )
	{
		CHECK ( convert<UNIT::BYTES, UNIT::SAMPLES>(12289200) == 3072300 );
	}

	SECTION ( "converts samples to frames correctly" )
	{
		CHECK ( convert<UNIT::SAMPLES, UNIT::FRAMES>(  3072300) ==   5225 );
		CHECK ( convert<UNIT::SAMPLES, UNIT::FRAMES>(148786344) == 253038 );
	}

	SECTION ( "converts samples to bytes correctly" )
	{
		CHECK ( convert<UNIT::SAMPLES, UNIT::BYTES>(3072300) == 12289200 );
	}
}

/*
TEST_CASE ( "Unit conversions", "[metadata_details]" )
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
*/


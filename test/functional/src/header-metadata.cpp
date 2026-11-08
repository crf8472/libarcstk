#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for metadata.hpp.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"           // TO BE TESTED
#endif


TEST_CASE ( "factor<>()", "[meta][details]" )
{
	using arcstk::conv::factor;

	using arcstk::UNIT;

	SECTION ( "is correct" )
	{
		CHECK ( factor<UNIT::BYTES, UNIT::FRAMES>()    == 2352 );
		CHECK ( factor<UNIT::BYTES, UNIT::SAMPLES>()   ==    4 );

		CHECK ( factor<UNIT::FRAMES, UNIT::BYTES>()    == 2352 );
		CHECK ( factor<UNIT::FRAMES, UNIT::SAMPLES>()  ==  588 );

		CHECK ( factor<UNIT::SAMPLES, UNIT::BYTES>()   ==    4 );
		CHECK ( factor<UNIT::SAMPLES, UNIT::FRAMES>()  ==  588 );

		CHECK ( factor<UNIT::BYTES, UNIT::BYTES>()     ==    1 );
		CHECK ( factor<UNIT::FRAMES, UNIT::FRAMES>()   ==    1 );
		CHECK ( factor<UNIT::SAMPLES, UNIT::SAMPLES>() ==    1 );
	}
}


TEST_CASE ( "convert<>()", "[meta]" )
{
	using arcstk::convert;
	using arcstk::UNIT;

	SECTION ( "is correct" )
	{
		CHECK ( convert<UNIT::BYTES, UNIT::FRAMES>(72912) == 31 );
	}
}


TEST_CASE ( "CDDA", "[meta]" )
{
	using arcstk::CDDA;

	SECTION ( "constants are correct" )
	{
		CHECK ( CDDA::SAMPLES_PER_SECOND ==  44100 );
		CHECK ( CDDA::BITS_PER_SAMPLE    ==     16 );
		CHECK ( CDDA::NUMBER_OF_CHANNELS ==      2 );
		CHECK ( CDDA::FRAMES_PER_SEC     ==     75 );
		CHECK ( CDDA::BYTES_PER_SAMPLE   ==      4 );
		CHECK ( CDDA::SAMPLES_PER_FRAME  ==    588 );
		CHECK ( CDDA::BYTES_PER_FRAME    ==   2352 );
	}

	SECTION ( "max values are correct" )
	{
		CHECK ( CDDA::MAX_TRACKCOUNT     ==     99 );
		CHECK ( CDDA::MAX_BLOCK_ADDRESS  == 449999 );
		CHECK ( CDDA::MAX_OFFSET         == 359999 );
	}

	SECTION ( "min values are correct" )
	{
		CHECK ( CDDA::MIN_TRACK_OFFSET_DIST == 300 );
		CHECK ( CDDA::MIN_TRACK_LEN_FRAMES  == 150 );
	}
}


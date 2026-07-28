#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Unit tests for metadata.hpp.
 */

#ifndef LIBARCSTK_METADATA_HPP_
#include "metadata.hpp"           // TO BE TESTED
#endif

#include <cstdint>                // for int32_t
#include <vector>                 // for vector


TEST_CASE ( "cdda_max<>", "[meta]" )
{
	SECTION ("cdda_max<>() yields correct maximum values")
	{
		using arcstk::UNIT;
		using arcstk::cdda_max;

		CHECK (     449999 == cdda_max<UNIT::FRAMES>  );
		CHECK (  264599412 == cdda_max<UNIT::SAMPLES> );
		CHECK ( 1058397648 == cdda_max<UNIT::BYTES>   );
	}
}


// 	const auto size_too_big = AudioSize { // bigger than allowed MAX
// 		CDDA::MAX_OFFSET + 1, UNIT::FRAMES };
//
// 	const auto size_too_small = AudioSize { // smaller than allowed MIN
// 		CDDA::MIN_TRACK_LEN_FRAMES - 1, UNIT::FRAMES };


TEST_CASE ( "convert<>()", "[convert] [meta]" )
{
	using arcstk::AudioSize;
	using arcstk::convert;
	using arcstk::UNIT;
	using arcstk::cdda_max;

	// 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
	//	157863, 198495, 213368, 225320, 234103

	// FRAMES -> BYTES

	SECTION ( "converts 0 frames to bytes correctly" )
	{
		CHECK ( convert<UNIT::FRAMES, UNIT::BYTES>(0) == 0 );
	}

	SECTION ( "converts frames max to bytes correctly" )
	{
		CHECK ( convert<UNIT::FRAMES, UNIT::BYTES>(cdda_max<UNIT::FRAMES>)
				== cdda_max<UNIT::BYTES>);
	}

	SECTION ( "converts frames to bytes correctly" )
	{
		CHECK ( convert<UNIT::FRAMES, UNIT::BYTES>(5225) == 12289200 );
		// TODO more...
	}

	// FRAMES -> SAMPLES

	SECTION ( "converts 0 frames to samples correctly" )
	{
		CHECK ( convert<UNIT::FRAMES, UNIT::SAMPLES>(0) == 0 );
	}

	SECTION ( "converts frames max to samples correctly" )
	{
		CHECK ( convert<UNIT::FRAMES, UNIT::SAMPLES>(cdda_max<UNIT::FRAMES>)
				== cdda_max<UNIT::SAMPLES>);
	}

	SECTION ( "converts frames to samples correctly" )
	{
		CHECK ( convert<UNIT::FRAMES, UNIT::SAMPLES>(  5225) ==   3072300 );
		CHECK ( convert<UNIT::FRAMES, UNIT::SAMPLES>(253038) == 148786344 );
	}

	// SAMPLES -> FRAMES

	SECTION ( "converts 0 samples to frames correctly" )
	{
		CHECK ( convert<UNIT::SAMPLES, UNIT::FRAMES>(0) == 0 );
	}

	SECTION ( "converts samples max to frames correctly" )
	{
		CHECK ( convert<UNIT::SAMPLES, UNIT::FRAMES>(cdda_max<UNIT::SAMPLES>)
				== cdda_max<UNIT::FRAMES>);
	}

	SECTION ( "converts samples to frames correctly" )
	{
		CHECK ( convert<UNIT::SAMPLES, UNIT::FRAMES>(  3072300) ==   5225 );
		CHECK ( convert<UNIT::SAMPLES, UNIT::FRAMES>(148786344) == 253038 );
	}

	// SAMPLES -> BYTES

	SECTION ( "converts 0 samples to bytes correctly" )
	{
		CHECK ( convert<UNIT::SAMPLES, UNIT::BYTES>(0) == 0 );
	}

	SECTION ( "converts samples max to bytes correctly" )
	{
		CHECK ( convert<UNIT::SAMPLES, UNIT::BYTES>(cdda_max<UNIT::SAMPLES>)
				== cdda_max<UNIT::BYTES>);
	}

	SECTION ( "converts samples to bytes correctly" )
	{
		CHECK ( convert<UNIT::SAMPLES, UNIT::BYTES>(3072300) == 12289200 );
	}

    // BYTES -> FRAMES

	SECTION ( "converts 0 bytes to frames correctly" )
	{
		CHECK ( convert<UNIT::BYTES, UNIT::FRAMES>(0) == 0 );
	}

	SECTION ( "converts bytes max to frames correctly" )
	{
		CHECK ( convert<UNIT::BYTES, UNIT::FRAMES>(cdda_max<UNIT::BYTES>)
				== cdda_max<UNIT::FRAMES>);
	}

	SECTION ( "converts bytes to frames correctly" )
	{
		CHECK ( convert<UNIT::BYTES, UNIT::FRAMES>(12289200) == 5225 );
	}

    // BYTES -> SAMPLES

	SECTION ( "converts 0 bytes to samples correctly" )
	{
		CHECK ( convert<UNIT::BYTES, UNIT::SAMPLES>(0) == 0 );
	}

	SECTION ( "converts bytes max to samples correctly" )
	{
		CHECK ( convert<UNIT::BYTES, UNIT::SAMPLES>(cdda_max<UNIT::BYTES>)
				== cdda_max<UNIT::SAMPLES>);
	}

	SECTION ( "converts bytes to samples correctly" )
	{
		CHECK ( convert<UNIT::BYTES, UNIT::SAMPLES>(12289200) == 3072300 );
	}

	// same

	SECTION ( "converts same unit correctly")
	{
		CHECK ( convert<UNIT::FRAMES,  UNIT::FRAMES> (12289237) == 12289237 );
		CHECK ( convert<UNIT::SAMPLES, UNIT::SAMPLES>(12289237) == 12289237 );
		CHECK ( convert<UNIT::BYTES,   UNIT::BYTES>  (12289237) == 12289237 );
	}
}


TEST_CASE ( "convert_to<>()", "[convert_to] [meta]" )
{
	using arcstk::convert_to;
	using arcstk::UNIT;
	using arcstk::cdda_max;

	SECTION ( "converts 0 frames to bytes correctly" )
	{
		CHECK ( convert_to<UNIT::BYTES>(0, UNIT::FRAMES) == 0 );
	}

	SECTION ( "converts frames max to bytes correctly" )
	{
		CHECK ( convert_to<UNIT::BYTES>(cdda_max<UNIT::FRAMES>, UNIT::FRAMES)
				== cdda_max<UNIT::BYTES>);
	}

	SECTION ( "converts frames to bytes correctly" )
	{
		CHECK ( convert_to<UNIT::BYTES>(5225, UNIT::FRAMES) == 12289200 );
		// TODO more...
	}

	// FRAMES -> SAMPLES

	SECTION ( "converts 0 frames to samples correctly" )
	{
		CHECK ( convert_to<UNIT::SAMPLES>(0, UNIT::FRAMES) == 0 );
	}

	SECTION ( "converts frames max to samples correctly" )
	{
		CHECK ( convert_to<UNIT::SAMPLES>(cdda_max<UNIT::FRAMES>, UNIT::FRAMES)
				== cdda_max<UNIT::SAMPLES>);
	}

	SECTION ( "converts frames to samples correctly" )
	{
		CHECK ( convert_to<UNIT::SAMPLES>(5225, UNIT::FRAMES) ==   3072300 );
		CHECK ( convert_to<UNIT::SAMPLES>(253038, UNIT::FRAMES) == 148786344 );
	}

	// SAMPLES -> FRAMES

	SECTION ( "converts 0 samples to frames correctly" )
	{
		CHECK ( convert_to<UNIT::FRAMES>(0, UNIT::SAMPLES) == 0 );
	}

	SECTION ( "converts samples max to frames correctly" )
	{
		CHECK ( convert_to<UNIT::FRAMES>(cdda_max<UNIT::SAMPLES>, UNIT::SAMPLES)
				== cdda_max<UNIT::FRAMES>);
	}

	SECTION ( "converts samples to frames correctly" )
	{
		CHECK ( convert_to<UNIT::FRAMES>(3072300, UNIT::SAMPLES) ==   5225 );
		CHECK ( convert_to<UNIT::FRAMES>(148786344, UNIT::SAMPLES) == 253038 );
	}

	// SAMPLES -> BYTES

	SECTION ( "converts 0 samples to bytes correctly" )
	{
		CHECK ( convert_to<UNIT::BYTES>(0, UNIT::SAMPLES) == 0 );
	}

	SECTION ( "converts samples max to bytes correctly" )
	{
		CHECK ( convert_to<UNIT::BYTES>(cdda_max<UNIT::SAMPLES>, UNIT::SAMPLES)
				== cdda_max<UNIT::BYTES>);
	}

	SECTION ( "converts samples to bytes correctly" )
	{
		CHECK ( convert_to<UNIT::BYTES>(3072300, UNIT::SAMPLES) == 12289200 );
	}

    // BYTES -> FRAMES

	SECTION ( "converts 0 bytes to frames correctly" )
	{
		CHECK ( convert_to<UNIT::FRAMES>(0, UNIT::BYTES) == 0 );
	}

	SECTION ( "converts bytes max to frames correctly" )
	{
		CHECK ( convert_to<UNIT::FRAMES>(cdda_max<UNIT::BYTES>, UNIT::BYTES)
				== cdda_max<UNIT::FRAMES>);
	}

	SECTION ( "converts bytes to frames correctly" )
	{
		CHECK ( convert_to<UNIT::FRAMES>(12289200, UNIT::BYTES) == 5225 );
	}

    // BYTES -> SAMPLES

	SECTION ( "converts 0 bytes to samples correctly" )
	{
		CHECK ( convert_to<UNIT::SAMPLES>(0, UNIT::BYTES) == 0 );
	}

	SECTION ( "converts bytes max to samples correctly" )
	{
		CHECK ( convert_to<UNIT::SAMPLES>(cdda_max<UNIT::BYTES>, UNIT::BYTES)
				== cdda_max<UNIT::SAMPLES>);
	}

	SECTION ( "converts bytes to samples correctly" )
	{
		CHECK ( convert_to<UNIT::SAMPLES>(12289200, UNIT::BYTES) == 3072300 );
	}

	// same

	SECTION ( "converts same unit correctly")
	{
		CHECK ( convert_to<UNIT::FRAMES>( 12289237, UNIT::FRAMES)  == 12289237 );
		CHECK ( convert_to<UNIT::SAMPLES>(12289237, UNIT::SAMPLES) == 12289237 );
		CHECK ( convert_to<UNIT::BYTES>(  12289237, UNIT::BYTES)   == 12289237 );
	}
}


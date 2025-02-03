#include "catch2/catch_test_macros.hpp"

/**
 * \file
 *
 * \brief Fixtures for metadata_details.hpp
 */

#ifndef __LIBARCSTK_METADATA_HPP__
#include "metadata.hpp"           // for AudioSize, CDDA, ToC
#endif
#ifndef __LIBARCSTK_METADATA_DETAILS_HPP__
#include "metadata_details.hpp"   // TO BE TESTED
#endif


TEST_CASE ("convert_to_bytes()", "[metadata_details]")
{
	using arcstk::details::convert_to_bytes;
	using arcstk::UNIT;

	SECTION ( "Converts samples to bytes correctly" )
	{
		CHECK ( convert_to_bytes(3072300, UNIT::SAMPLES) == 12289200 );
	}

	SECTION ( "Converts frames to bytes correctly" )
	{
		CHECK ( convert_to_bytes(5225, UNIT::FRAMES) == 12289200 );
	}

	SECTION ( "Returns bytes when bytes were passed" )
	{
		CHECK ( convert_to_bytes(12345, UNIT::BYTES) == 12345 );
	}
}

TEST_CASE ("validate_lengths()", "[metadata_details]")
{
	using arcstk::details::validate::validate_lengths;

	using arcstk::toc::construct;

	const auto toc_data { construct(
		253038,
		{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
			157863, 198495, 213368, 225320, 234103 }
	)};

	SECTION ("Validates correct distances correctly")
	{
		CHECK_NOTHROW ( validate_lengths(toc_data) );
	}

	SECTION ("Throws on leadout too short")
	{
		const auto toc_data1 { construct(
			234104, // last track is 1 frame
			{ 33, 5225, 7390, 23380, 35608, 49820, 69508, 87733, 106333, 139495,
				157863, 198495, 213368, 225320, 234103 }
		)};

		CHECK_THROWS ( validate_lengths(toc_data1) );
	}

	SECTION ("Throws on first track too short")
	{
		const auto toc_data1 { construct(
			253038,
			{ 33, 33 /* 0 length */, 7390, 23380, 35608, 49820, 69508, 87733,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		)};

		CHECK_THROWS ( validate_lengths(toc_data1) );
	}

	SECTION ("Throws on some mid track too short")
	{
		const auto toc_data1 { construct(
			253038,
			{ 33, 5225, 7390, 23380, 35608, 49820, 49825/* too short */, 87733,
				106333, 139495, 157863, 198495, 213368, 225320, 234103 }
		)};

		CHECK_THROWS ( validate_lengths(toc_data1) );
	}
}

